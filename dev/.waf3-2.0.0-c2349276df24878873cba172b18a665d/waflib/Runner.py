#! /usr/bin/env python
# encoding: utf-8
# WARNING! Do not edit! https://waf.io/book/index.html#_obtaining_the_waf_file

import heapq,traceback
try:
	from queue import Queue
except ImportError:
	from Queue import Queue
from waflib import Utils,Task,Errors,Logs
GAP=20
class PriorityTasks(object):
	def __init__(self):
		self.lst=[]
	def __len__(self):
		return len(self.lst)
	def __iter__(self):
		return iter(self.lst)
	def clear(self):
		self.lst=[]
	def append(self,task):
		heapq.heappush(self.lst,task)
	def appendleft(self,task):
		heapq.heappush(self.lst,task)
	def pop(self):
		return heapq.heappop(self.lst)
	def extend(self,lst):
		if self.lst:
			for x in lst:
				self.append(x)
		else:
			if isinstance(lst,list):
				self.lst=lst
				heapq.heapify(lst)
			else:
				self.lst=lst.lst
class Consumer(Utils.threading.Thread):
	def __init__(self,spawner,task):
		Utils.threading.Thread.__init__(self)
		self.task=task
		self.spawner=spawner
		self.setDaemon(1)
		self.start()
	def run(self):
		try:
			if not self.spawner.master.stop:
				self.spawner.master.process_task(self.task)
		finally:
			self.spawner.sem.release()
			self.spawner.master.out.put(self.task)
			self.task=None
			self.spawner=None
class Spawner(Utils.threading.Thread):
	def __init__(self,master):
		Utils.threading.Thread.__init__(self)
		self.master=master
		self.sem=Utils.threading.Semaphore(master.numjobs)
		self.setDaemon(1)
		self.start()
	def run(self):
		try:
			self.loop()
		except Exception:
			pass
	def loop(self):
		master=self.master
		while 1:
			task=master.ready.get()
			self.sem.acquire()
			if not master.stop:
				task.log_display(task.generator.bld)
			Consumer(self,task)
class Parallel(object):
	def __init__(self,bld,j=2):
		self.numjobs=j
		self.bld=bld
		self.outstanding=PriorityTasks()
		self.postponed=PriorityTasks()
		self.incomplete=set()
		self.ready=Queue(0)
		self.out=Queue(0)
		self.count=0
		self.processed=0
		self.stop=False
		self.error=[]
		self.biter=None
		self.dirty=False
		self.revdeps=Utils.defaultdict(set)
		self.spawner=Spawner(self)
	def get_next_task(self):
		if not self.outstanding:
			return None
		return self.outstanding.pop()
	def postpone(self,tsk):
		self.postponed.append(tsk)
	def refill_task_list(self):
		while self.count>self.numjobs*GAP:
			self.get_out()
		while not self.outstanding:
			if self.count:
				self.get_out()
				if self.outstanding:
					break
			elif self.postponed:
				try:
					cond=self.deadlock==self.processed
				except AttributeError:
					pass
				else:
					if cond:
						msg='check the build order for the tasks'
						for tsk in self.postponed:
							if not tsk.run_after:
								msg='check the methods runnable_status'
								break
						lst=[]
						for tsk in self.postponed:
							lst.append('%s\t-> %r'%(repr(tsk),[id(x)for x in tsk.run_after]))
						raise Errors.WafError('Deadlock detected: %s%s'%(msg,''.join(lst)))
				self.deadlock=self.processed
			if self.postponed:
				self.outstanding.extend(self.postponed)
				self.postponed.clear()
			elif not self.count:
				if self.incomplete:
					for x in self.incomplete:
						for k in x.run_after:
							if not k.hasrun:
								break
						else:
							self.incomplete.remove(x)
							self.outstanding.append(x)
							break
					else:
						raise Errors.WafError('Broken revdeps detected on %r'%self.incomplete)
				else:
					tasks=next(self.biter)
					ready,waiting=self.prio_and_split(tasks)
					self.outstanding.extend(ready)
					self.incomplete.update(waiting)
					self.total=self.bld.total()
					break
	def add_more_tasks(self,tsk):
		if getattr(tsk,'more_tasks',None):
			ready,waiting=self.prio_and_split(tsk.more_tasks)
			self.outstanding.extend(ready)
			self.incomplete.update(waiting)
			self.total+=len(tsk.more_tasks)
	def mark_finished(self,tsk):
		def try_unfreeze(x):
			if x in self.incomplete:
				for k in x.run_after:
					if not k.hasrun:
						break
				else:
					self.incomplete.remove(x)
					self.outstanding.append(x)
		if tsk in self.revdeps:
			for x in self.revdeps[tsk]:
				if isinstance(x,Task.TaskGroup):
					x.prev.remove(tsk)
					if not x.prev:
						for k in x.next:
							k.run_after.remove(x)
							try_unfreeze(k)
						x.next=[]
				else:
					try_unfreeze(x)
			del self.revdeps[tsk]
	def get_out(self):
		tsk=self.out.get()
		if not self.stop:
			self.add_more_tasks(tsk)
		self.mark_finished(tsk)
		self.count-=1
		self.dirty=True
		return tsk
	def add_task(self,tsk):
		self.ready.put(tsk)
	def process_task(self,tsk):
		tsk.process()
		if tsk.hasrun!=Task.SUCCESS:
			self.error_handler(tsk)
	def skip(self,tsk):
		tsk.hasrun=Task.SKIPPED
		self.mark_finished(tsk)
	def cancel(self,tsk):
		tsk.hasrun=Task.CANCELED
		self.mark_finished(tsk)
	def error_handler(self,tsk):
		if not self.bld.keep:
			self.stop=True
		self.error.append(tsk)
	def task_status(self,tsk):
		try:
			return tsk.runnable_status()
		except Exception:
			self.processed+=1
			tsk.err_msg=traceback.format_exc()
			if not self.stop and self.bld.keep:
				self.skip(tsk)
				if self.bld.keep==1:
					if Logs.verbose>1 or not self.error:
						self.error.append(tsk)
					self.stop=True
				else:
					if Logs.verbose>1:
						self.error.append(tsk)
				return Task.EXCEPTION
			tsk.hasrun=Task.EXCEPTION
			self.error_handler(tsk)
			return Task.EXCEPTION
	def start(self):
		self.total=self.bld.total()
		while not self.stop:
			self.refill_task_list()
			tsk=self.get_next_task()
			if not tsk:
				if self.count:
					continue
				else:
					break
			if tsk.hasrun:
				self.processed+=1
				continue
			if self.stop:
				break
			st=self.task_status(tsk)
			if st==Task.RUN_ME:
				self.count+=1
				self.processed+=1
				if self.numjobs==1:
					tsk.log_display(tsk.generator.bld)
					try:
						self.process_task(tsk)
					finally:
						self.out.put(tsk)
				else:
					self.add_task(tsk)
			elif st==Task.ASK_LATER:
				self.postpone(tsk)
			elif st==Task.SKIP_ME:
				self.processed+=1
				self.skip(tsk)
				self.add_more_tasks(tsk)
			elif st==Task.CANCEL_ME:
				if Logs.verbose>1:
					self.error.append(tsk)
				self.processed+=1
				self.cancel(tsk)
		while self.error and self.count:
			self.get_out()
		self.ready.put(None)
		if not self.stop:
			assert not self.count
			assert not self.postponed
			assert not self.incomplete
	def prio_and_split(self,tasks):
		for x in tasks:
			x.visited=0
		reverse=self.revdeps
		for x in tasks:
			for k in x.run_after:
				if isinstance(k,Task.TaskGroup):
					if k.done:
						pass
					else:
						k.done=True
						for j in k.prev:
							reverse[j].add(k)
				else:
					reverse[k].add(x)
		def visit(n):
			if isinstance(n,Task.TaskGroup):
				return sum(visit(k)for k in n.next)
			if n.visited==0:
				n.visited=1
				if n in reverse:
					rev=reverse[n]
					n.prio_order=n.tree_weight+len(rev)+sum(visit(k)for k in rev)
				else:
					n.prio_order=n.tree_weight
				n.visited=2
			elif n.visited==1:
				raise Errors.WafError('Dependency cycle found!')
			return n.prio_order
		for x in tasks:
			if x.visited!=0:
				continue
			try:
				visit(x)
			except Errors.WafError:
				self.debug_cycles(tasks,reverse)
		ready=[]
		waiting=[]
		for x in tasks:
			for k in x.run_after:
				if not k.hasrun:
					waiting.append(x)
					break
			else:
				ready.append(x)
		return(ready,waiting)
	def debug_cycles(self,tasks,reverse):
		tmp={}
		for x in tasks:
			tmp[x]=0
		def visit(n,acc):
			if isinstance(n,Task.TaskGroup):
				for k in n.next:
					visit(k,acc)
				return
			if tmp[n]==0:
				tmp[n]=1
				for k in reverse.get(n,[]):
					visit(k,[n]+acc)
				tmp[n]=2
			elif tmp[n]==1:
				lst=[]
				for tsk in acc:
					lst.append(repr(tsk))
					if tsk is n:
						break
				raise Errors.WafError('Task dependency cycle in "run_after" constraints: %s'%''.join(lst))
		for x in tasks:
			visit(x,[])
