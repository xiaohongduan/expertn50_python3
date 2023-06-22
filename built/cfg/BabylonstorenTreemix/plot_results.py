#!/usr/bin/python
# -*- coding: utf-8 -*-
from cgeolib import *
from pylab import *
import os
import numpy as np
import matplotlib.ticker
import matplotlib.dates as mdates
import csv
import math
import time as ti
import datetime
from dateutil.relativedelta import relativedelta


matplotlib.rcParams['mathtext.fontset'] = 'stixsans'
matplotlib.rcParams['text.usetex'] = False
matplotlib.rcParams['text.latex.unicode'] = True
params = {'mathtext.default': 'regular' }          
matplotlib.rcParams.update(params)
matplotlib.rcParams.update({'font.size': 30})

act_path = os.path.dirname(os.path.realpath(__file__))

data_path = os.path.join(act_path,'output')
fig_path_base = os.path.join(act_path,'Graphs')

looping = [('1','Irrigation7'),
#('2','Irrigation50')
]

def plot_low(tf, tr, varFull, varRed, col, ylab, ymax, xtickloc, figfilename):
    matplotlib.rcParams.update({'font.size': 30})
    fig=plt.figure(figsize=(32,12),facecolor='w')
    ax1 = subplot(111)
    
    ax1.plot(tf, varFull, color = col, lw = 2, linestyle = '--', label = 'Full Wind Speed')
    ax1.plot(tr, varRed, color = col, lw = 2, linestyle = '-', label = 'Reduced Wind Speed')
    
    if ymax != -99:
        ax1.set_ylim(0,ymax)
    
    ax1.set_ylabel(ylab, labelpad = 10)

    ax1.set_xticks(xtickloc, minor = True)
    ax1.set_xticks(xtickloc[::2])
    myFmt = mdates.DateFormatter('%d-%m-%y')
    plt.setp(ax1.xaxis.set_major_formatter(myFmt))
    plt.setp(ax1.xaxis.get_majorticklabels(), rotation=90, ha = "center") 
    ax1.grid(which='both')
    
    handles, labels = ax1.get_legend_handles_labels()
    lg = fig.legend(handles, labels, loc = "upper center", ncol=2)
    lg.draw_frame(False)
        
    subplots_adjust(left=0.04,right=0.98,bottom=0.18,top=0.9,wspace=0.2,hspace=0.2)
    
    savefig(figfilename)
    close()

def plot_high(plotlist, ylab, ymax, xtickloc, legncol, topspace, figfilename):
    matplotlib.rcParams.update({'font.size': 36})
    fig=plt.figure(figsize=(32,18),facecolor='w')
    ax1 = subplot(111)
    
    
    # time, var, color, linestyle
    for ti, var, col, ls, lab in plotlist:
        ax1.plot(ti, var, color=col, lw=3, linestyle = ls, label = lab)
    
    
    #ax1.plot(tf, varFull, color = col, lw = 2, linestyle = '--', label = 'Full Wind Speed')
    #ax1.plot(tr, varRed, color = col, lw = 2, linestyle = '-', label = 'Reduced Wind Speed')
    
    if ymax != -99:
        ax1.set_ylim(0,ymax)
    
    ax1.set_ylabel(ylab, labelpad = 20)

    ax1.set_xticks(xtickloc, minor = True)
    ax1.set_xticks(xtickloc[::2])
    myFmt = mdates.DateFormatter('%d-%m-%y')
    plt.setp(ax1.xaxis.set_major_formatter(myFmt))
    plt.setp(ax1.xaxis.get_majorticklabels(), rotation=90, ha = "center") 
    ax1.grid(which='both')
    
    handles, labels = ax1.get_legend_handles_labels()
    lg = fig.legend(handles, labels, loc = "upper center", ncol=legncol)
    lg.draw_frame(False)
        
    subplots_adjust(left=0.1,right=0.98,bottom=0.14,top=topspace,wspace=0.2,hspace=0.2)
    
    savefig(figfilename)
    close()

def plot_low2(plotlist, ylab, ymax, xtickloc, legncol, topspace, figfilename):
    matplotlib.rcParams.update({'font.size': 30})
    fig=plt.figure(figsize=(32,12),facecolor='w')
    ax1 = subplot(111)
    
    
    # time, var, color, linestyle
    for ti, var, col, ls, lab in plotlist:
        ax1.plot(ti, var, color=col, lw=3, linestyle = ls, label = lab)
    
    
    #ax1.plot(tf, varFull, color = col, lw = 2, linestyle = '--', label = 'Full Wind Speed')
    #ax1.plot(tr, varRed, color = col, lw = 2, linestyle = '-', label = 'Reduced Wind Speed')
    
    if ymax != -99:
        ax1.set_ylim(0,ymax)
    
    ax1.set_ylabel(ylab, labelpad = 20)

    ax1.set_xticks(xtickloc, minor = True)
    ax1.set_xticks(xtickloc[::2])
    myFmt = mdates.DateFormatter('%d-%m-%y')
    plt.setp(ax1.xaxis.set_major_formatter(myFmt))
    plt.setp(ax1.xaxis.get_majorticklabels(), rotation=90, ha = "center") 
    ax1.grid(which='both')
    
    handles, labels = ax1.get_legend_handles_labels()
    lg = fig.legend(handles, labels, loc = "upper center", ncol=legncol)
    lg.draw_frame(False)
        
    subplots_adjust(left=0.1,right=0.98,bottom=0.18,top=topspace,wspace=0.2,hspace=0.2)
    
    savefig(figfilename)
    close()

for num, irr in looping:
    
    fig_path = os.path.join(fig_path_base,irr)
    try:
        os.makedirs(fig_path)
    except:
        pass
    
    fnWindFull = os.path.join(data_path,'BabylonstorenTreemix_1_'+num+'_0_0.xno')
    fnWindRed = os.path.join(data_path,'BabylonstorenTreemix_1_'+num+'_1_0.xno')

    WindFullData = xpn_data(fnWindFull,spacer=',',dateformat='%Y-%m-%d %H:%M:%S',merge_delimiters=False)
    WindRedData = xpn_data(fnWindRed,spacer=',',dateformat='%Y-%m-%d %H:%M:%S',merge_delimiters=False)

    TimeFull, WindSpeedFull, LAIFull, HeightFull, DevStageFull, RootingDepthFull = WindFullData['Time and Date','output.Climate.Wind Speed.Wind Speed [m/s]','output.Plant.Leaf Area Index.LAI [m2/m2]','output.Plant.Height.Height [m]','output.Plant.Development Stage.BBCH Stage [-]','output.Plant.Rooting Depth.Depth [mm]']
    RootsFull, StemFull, LeavesFull, FruitsFull, WoodFull = WindFullData['output.Plant.Biomass.Roots [kg/ha]','output.Plant.Biomass.Stem [kg/ha]','output.Plant.Biomass.Leaves [kg/ha]','output.Plant.Biomass.Fruits [kg/ha]','output.Plant.Biomass.Wood above Ground [kg/ha]']
#    EtPotDayFull, EtActDayFull, EPotDayFull, EActDayFull, TPotDayFull, TActDayFull = WindFullData['output.Water.Evapotranspiration Day.Potential Evapotranspiration [mm]','output.Water.Evapotranspiration Day.Actual Evapotranspiration [mm]','output.Water.Evaporation and Transpiration Day.Potential Evaporation [mm]','output.Water.Evaporation and Transpiration Day.Actual Evaporation [mm]','output.Plant.Transpiration Day.Potential Transpiration [mm]','output.Plant.Transpiration Day.Actual Transpiration [mm]']
    EPotCumFull, EActCumFull, TPotCumFull, TActCumFull = WindFullData['output.Water.Water Compartments Cumulative.Potential Evaporation [mm]','output.Water.Water Compartments Cumulative.Actual Evaporation [mm]','output.Water.Water Compartments Cumulative.Potential Transpiration [mm]','output.Water.Water Compartments Cumulative.Actual Transpiration [mm]']
    SWC30Full, SWC60Full, SWC90Full = WindFullData['output.Water.Water Content at Depth.theta at 30 cm [cm3/cm3]','output.Water.Water Content at Depth.theta at 60 cm [cm3/cm3]','output.Water.Water Content at Depth.theta at 90 cm [cm3/cm3]']
    SWC0to30Full, SWC30to60Full, SWC60to90Full = WindFullData['output.Water.Water Content in Profile.theta 0-30 cm [cm3/cm3]','output.Water.Water Content in Profile.theta 30-60 cm [cm3/cm3]','output.Water.Water Content in Profile.theta 60-90 cm [cm3/cm3]']
    WCProfileCumFull, LeachingCumFull, ETActCumFull, WaterInputFull,WaterBalanceFull = WindFullData['output.Balance.Soil Water Balance.WC in Profile [mm]','output.Balance.Soil Water Balance.Leaching [mm]','output.Balance.Soil Water Balance.Evapotranspiration [mm]','output.Balance.Soil Water Balance.Water input (including from root competition) [mm]','output.Balance.Soil Water Balance.Water Balance [mm]']
    PrecipitationCumFull, DrainAgeCumFull = WindFullData['output.Water.Water Compartments Cumulative.Precipitation [mm]','output.Water.Water Compartments Cumulative.Drainage [mm]']

    TimeRed, WindSpeedRed, LAIRed, HeightRed, DevStageRed, RootingDepthRed = WindRedData['Time and Date','output.Climate.Wind Speed.Wind Speed [m/s]','output.Plant.Leaf Area Index.LAI [m2/m2]','output.Plant.Height.Height [m]','output.Plant.Development Stage.BBCH Stage [-]','output.Plant.Rooting Depth.Depth [mm]']
    RootsRed, StemRed, LeavesRed, FruitsRed, WoodRed = WindRedData['output.Plant.Biomass.Roots [kg/ha]','output.Plant.Biomass.Stem [kg/ha]','output.Plant.Biomass.Leaves [kg/ha]','output.Plant.Biomass.Fruits [kg/ha]','output.Plant.Biomass.Wood above Ground [kg/ha]']
#    EtPotDayRed, EtActDayRed, EPotDayRed, EActDayRed, TPotDayRed, TActDayRed = WindRedData['output.Water.Evapotranspiration Day.Potential Evapotranspiration [mm]','output.Water.Evapotranspiration Day.Actual Evapotranspiration [mm]','output.Water.Evaporation and Transpiration Day.Potential Evaporation [mm]','output.Water.Evaporation and Transpiration Day.Actual Evaporation [mm]','output.Plant.Transpiration Day.Potential Transpiration [mm]','output.Plant.Transpiration Day.Actual Transpiration [mm]']
    EPotCumRed, EActCumRed, TPotCumRed, TActCumRed = WindRedData['output.Water.Water Compartments Cumulative.Potential Evaporation [mm]','output.Water.Water Compartments Cumulative.Actual Evaporation [mm]','output.Water.Water Compartments Cumulative.Potential Transpiration [mm]','output.Water.Water Compartments Cumulative.Actual Transpiration [mm]']
    SWC30Red, SWC60Red, SWC90Red = WindRedData['output.Water.Water Content at Depth.theta at 30 cm [cm3/cm3]','output.Water.Water Content at Depth.theta at 60 cm [cm3/cm3]','output.Water.Water Content at Depth.theta at 90 cm [cm3/cm3]']
    SWC0to30Red, SWC30to60Red, SWC60to90Red = WindRedData['output.Water.Water Content in Profile.theta 0-30 cm [cm3/cm3]','output.Water.Water Content in Profile.theta 30-60 cm [cm3/cm3]','output.Water.Water Content in Profile.theta 60-90 cm [cm3/cm3]']
    WCProfileCumRed, LeachingCumRed, ETActCumRed, WaterInputRed,WaterBalanceRed = WindRedData['output.Balance.Soil Water Balance.WC in Profile [mm]','output.Balance.Soil Water Balance.Leaching [mm]','output.Balance.Soil Water Balance.Evapotranspiration [mm]','output.Balance.Soil Water Balance.Water input (including from root competition) [mm]','output.Balance.Soil Water Balance.Water Balance [mm]']
    PrecipitationCumRed, DrainAgeCumRed = WindRedData['output.Water.Water Compartments Cumulative.Precipitation [mm]','output.Water.Water Compartments Cumulative.Drainage [mm]']

    Date = np.array([t.date() for t in TimeFull if t.hour == 1])
    DateNoon = np.array([datetime.datetime.combine(d, datetime.time.min) + datetime.timedelta(0.5) for d in Date])
    TimeDate = np.array([t.date() for t in TimeFull])
    
    Months = np.array([t.date() + relativedelta(months=+1) for t in TimeFull if (t.day == 1 and t.hour == 1)])
    
    ## Wind Speed
    
    WindSpeedDayFull = np.array([sum(WindSpeedFull[TimeDate == d])/len(WindSpeedFull[TimeDate == d]) for d in Date])
    WindSpeedDayRed = np.array([sum(WindSpeedRed[TimeDate == d])/len(WindSpeedFull[TimeDate == d]) for d in Date])

    
 #   figfilename=os.path.join(fig_path,'WindSpeed'+irr+'.png')
#    plot_low(DateNoon, DateNoon, WindSpeedDayFull, WindSpeedDayRed, 'r', 'Wind Speed [m/s]', -99, Months, figfilename)
    
#    figfilename=os.path.join(fig_path,'TPotDay'+irr+'.png')
        # Time Full, Time Red, Variable Full, Variable Red, Color, y-Axis label, y maximum, x-tick-location, filename of figure)
 #   plot_low(DateNoon, DateNoon, TPotDayFull[::24], TPotDayRed[::24], '#FFBD00', 'Daily Potential Transpiration [mm]', 12, Months, figfilename)
    
#    figfilename=os.path.join(fig_path,'TActDay'+irr+'.png')
 #   plot_low(DateNoon, DateNoon, TActDayFull[::24], TActDayRed[::24], 'g', 'Daily Actual Transpiration [mm]', 12, Months, figfilename)

 #   figfilename=os.path.join(fig_path,'EPotDay'+irr+'.png')
 #   plot_low(DateNoon, DateNoon, EPotDayFull[::24], EPotDayRed[::24], 'r', 'Daily Potential Evaporation [mm]', 5, Months, figfilename)
    
 #   figfilename=os.path.join(fig_path,'EActDay'+irr+'.png')
  #  plot_low(DateNoon, DateNoon, EActDayFull[::24], EActDayRed[::24], 'b', 'Daily Actual Evaporation [mm]', 5, Months, figfilename)

 #   figfilename=os.path.join(fig_path,'ETPotDay'+irr+'.png')
 #   plot_low(DateNoon, DateNoon, EtPotDayFull[::24], EtPotDayRed[::24], '#FF00FF', 'Daily Potential Evapotranspiration [mm]', 12, Months, figfilename)
    
    figfilename=os.path.join(fig_path,'ETActDay'+irr+'.png')
    plot_low(DateNoon, DateNoon, EtActDayFull[::24], EtActDayRed[::24], 'c', 'Daily Actual Evapotranspiration [mm]', 12, Months, figfilename)


    ETPotCumFull = EPotCumFull + TPotCumFull
    ETPotCumRed = EPotCumRed + TPotCumRed
    
    plotlist = [
    # time, var, color, linestyle, label
    (TimeRed,ETPotCumRed, '#FF00FF', '-', u'ET$\mathrm{_{pot,red}}$ [mm]'),
    (TimeRed,EPotCumRed, 'r', '-', u'E$\mathrm{_{pot,red}}$ [mm]'),
    (TimeRed,TPotCumRed, '#FFBD00', '-', u'T$\mathrm{_{pot,red}}$ [mm]'),
    (TimeRed,ETActCumRed, 'c', '-', u'ET$\mathrm{_{act,red}}$ [mm]'),
    (TimeRed,EActCumRed, 'b', '-', u'E$\mathrm{_{act,red}}$ [mm]'),
    (TimeRed,TActCumRed, 'g', '-', u'T$\mathrm{_{act,red}}$ [mm]'),
    (TimeFull,ETPotCumFull, '#FF00FF', '--', u'ET$\mathrm{_{pot,full}}$ [mm]'),
    (TimeFull,EPotCumFull, 'r', '--', u'E$\mathrm{_{pot,full}}$ [mm]'),
    (TimeFull,TPotCumFull, '#FFBD00', '--', u'T$\mathrm{_{pot,full}}$ [mm]'),
    (TimeFull,ETActCumFull, 'c', '--', u'ET$\mathrm{_{act,full}}$ [mm]'),
    (TimeFull,EActCumFull, 'b', '--', u'E$\mathrm{_{act,full}}$ [mm]'),
    (TimeFull,TActCumFull, 'g', '--', u'T$\mathrm{_{act,full}}$ [mm]'),
    ]

    figfilename=os.path.join(fig_path,'ETCumAll'+irr+'.png')
    # list of plotted varibales, y-Axis label, y maximum, x-tick-location, number of columns legend, topspace filename of figure
    plot_high(plotlist, 'Cumulative Evapotranspiration [mm]', -99, Months, 4, 0.82, figfilename)

    figfilename=os.path.join(fig_path,'ETCum'+irr+'.png')
    redList = [0,3,6,9]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, 'Cumulative Evapotranspiration [mm]', -99, Months, 4, 0.92, figfilename)

    figfilename=os.path.join(fig_path,'ECum'+irr+'.png')
    redList = [1,4,7,10]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, 'Cumulative Evaporation [mm]', -99, Months, 4, 0.92, figfilename)
    
    figfilename=os.path.join(fig_path,'TCum'+irr+'.png')
    redList = [2,5,8,11]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, 'Cumulative Transpiration [mm]', -99, Months, 4, 0.92, figfilename)

    figfilename=os.path.join(fig_path,'ETCumRed'+irr+'.png')
    redList = [0,3,1,4,2,5]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, 'Cumulative Evapotranspiration [mm]', -99, Months, 3, 0.87, figfilename)

    figfilename=os.path.join(fig_path,'ETCumFull'+irr+'.png')
    redList = [6,9,7,10,8,11]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, 'Cumulative Evapotranspiration [mm]', -99, Months, 3, 0.87, figfilename)

    figfilename=os.path.join(fig_path,'ETCumAct'+irr+'.png')
    redList = [3,9,4,10,5,11]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, 'Cumulative Evapotranspiration [mm]', -99, Months, 3, 0.87, figfilename)
    
    figfilename=os.path.join(fig_path,'ETCumPot'+irr+'.png')
    redList = [0,6,1,7,2,8]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, 'Cumulative Evapotranspiration [mm]', -99, Months, 3, 0.87, figfilename)

    plotlist = [
    # time, var, color, linestyle, label
    (TimeRed,SWC30Red, 'b', '-', u'$\mathrm{\\theta_{30,red}}$'),
    (TimeRed,SWC60Red, 'g', '-', u'$\mathrm{\\theta_{60,red}}$'),
    (TimeRed,SWC90Red, '#FFBD00', '-', u'$\mathrm{\\theta_{90,red}}$'),
    (TimeRed,SWC0to30Red, 'b', '-', u'$\mathrm{\\theta_{0-30,red}}$'),
    (TimeRed,SWC30to60Red, 'g', '-', u'$\mathrm{\\theta_{30-60,red}}$'),
    (TimeRed,SWC60to90Red, '#FFBD00', '-', u'$\mathrm{\\theta_{60-90,red}}$'),
    (TimeFull,SWC30Full, 'b', '-', u'$\mathrm{\\theta_{30,full}}$'),
    (TimeFull,SWC60Full, 'g', '-', u'$\mathrm{\\theta_{60,full}}$'),
    (TimeFull,SWC90Full, '#FFBD00', '-', u'$\mathrm{\\theta_{90,full}}$'),
    (TimeFull,SWC0to30Full, 'b', '-', u'$\mathrm{\\theta_{0-30,full}}$'),
    (TimeFull,SWC30to60Full, 'g', '-', u'$\mathrm{\\theta_{30-60,full}}$'),
    (TimeFull,SWC60to90Full, '#FFBD00', '-', u'$\mathrm{\\theta_{60-90,full}}$'),
    ]

    figfilename=os.path.join(fig_path,'WCDepthRed'+irr+'.png')
    redList = [0,1,2]
    plotlistRed = [plotlist[i] for i in redList]
    plot_low2(plotlistRed, 'Soil Water Content [$\mathrm{cm^3/cm^3}$]', -99, Months, 3, 0.9, figfilename)

    figfilename=os.path.join(fig_path,'WCDepthFull'+irr+'.png')
    redList = [6,7,8]
    plotlistRed = [plotlist[i] for i in redList]
    plot_low2(plotlistRed, 'Soil Water Content [$\mathrm{cm^3/cm^3}$]', -99, Months, 3, 0.9, figfilename)

    figfilename=os.path.join(fig_path,'WCLayerRed'+irr+'.png')
    redList = [3,4,5]
    plotlistRed = [plotlist[i] for i in redList]
    plot_low2(plotlistRed, 'Soil Water Content [$\mathrm{cm^3/cm^3}$]', -99, Months, 3, 0.9, figfilename)
    
    figfilename=os.path.join(fig_path,'WCLayerFull'+irr+'.png')
    redList = [9,10,11]
    plotlistRed = [plotlist[i] for i in redList]
    plot_low2(plotlistRed, 'Soil Water Content [$\mathrm{cm^3/cm^3}$]', -99, Months, 3, 0.9, figfilename)



    plotlist = [
    # time, var, color, linestyle, label
    (TimeRed,WCProfileCumRed, '#FF00FF', '-', u'WaterContentProfile$\mathrm{_{red}}$'),
    (TimeRed,WaterInputRed, 'b', '-', u'SoilWaterInput$\mathrm{_{red}}$'),
    (TimeRed,LeachingCumRed, '#FFBD00', '-', u'Percolation$\mathrm{_{red}}$'),
    (TimeRed,ETActCumRed, 'c', '-', u'ET$\mathrm{_{act,red}}$'),
    (TimeRed,WaterBalanceRed, 'r', '-', u'Balance$\mathrm{_{red}}$'),
    (TimeFull,WCProfileCumFull, '#FF00FF', '--', u'WaterContentProfile$\mathrm{_{full}}$'),
    (TimeFull,WaterInputFull, 'b', '--', u'SoilWaterInput$\mathrm{_{full}}$'),
    (TimeFull,LeachingCumFull, '#FFBD00', '--', u'Percolation$\mathrm{_{full}}$'),
    (TimeFull,ETActCumFull, 'c', '--', u'ET$\mathrm{_{act,full}}$'),
    (TimeFull,WaterBalanceFull, 'r', '--', u'Balance$\mathrm{_{full}}$'),     
    ]

    figfilename=os.path.join(fig_path,'WaterBalanceRed'+irr+'.png')
    redList = [0,1,2,3,4]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, 'Soil Water Balance [mm]', -99, Months, 3, 0.87, figfilename)

    figfilename=os.path.join(fig_path,'WaterBalanceFull'+irr+'.png')
    redList = [5,6,7,8,9]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, 'Soil Water Balance [mm]', -99, Months, 3, 0.87, figfilename)
    
    figfilename=os.path.join(fig_path,'WaterBalanceAll'+irr+'.png')
    redList = [0,5,1,2,7,6,3,8,4,9]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, 'Soil Water Balance [mm]', -99, Months, 4, 0.82, figfilename)



    plotlist = [
    (TimeFull,LAIFull, 'g', '-', u'LAI [$\mathrm{m^2/m^2}$]'),     
    (TimeFull,HeightFull, 'b', '-', u'Height [m]'),     
    (TimeFull,RootsFull, 'r', '-', u'Roots [kg/ha]'),     
    (TimeFull,StemFull, 'c', '-', u'Stem [kg/ha]'),     
    (TimeFull,WoodFull, '#934408', '-', u'Wood [kg/ha]'),     
    (TimeFull,LeavesFull, 'g', '-', u'Leaves [kg/ha]'),     
    (TimeFull,FruitsFull, '#FFBD00', '-', u'Fruits [kg/ha]'),     
    ]

    figfilename=os.path.join(fig_path,'LAI'+irr+'.png')
    redList = [0]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, u'LAI [$\mathrm{m^2/m^2}$]', -99, Months, 1, 0.92, figfilename)
    
    figfilename=os.path.join(fig_path,'Height'+irr+'.png')
    redList = [1]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, u'Height [m]', -99, Months, 1, 0.92, figfilename)
    
    figfilename=os.path.join(fig_path,'LAI+Height'+irr+'.png')
    redList = [0,1]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, u'LAI [$\mathrm{m^2/m^2}$] and Height [m]', -99, Months, 2, 0.92, figfilename)

    figfilename=os.path.join(fig_path,'Biomass1'+irr+'.png')
    redList = [2,3,4,5,6]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, u'Biomass [kg/ha]', -99, Months, 3, 0.87, figfilename)
    
    figfilename=os.path.join(fig_path,'Biomass2'+irr+'.png')
    redList = [2,4,5,6]
    plotlistRed = [plotlist[i] for i in redList]
    plot_high(plotlistRed, u'Biomass [kg/ha]', -99, Months, 2, 0.87, figfilename)


#a = [-2,1,5,3,8,5,6]
#b = [1,2,5]
#c = [ a[i] for i in b]

exit()
    
    
    
        ## Daily Actual Evaporation

    #fig=plt.figure(figsize=(32,12),facecolor='w')
    #ax1 = subplot(111)
    
    #ax1.plot(TimeFull, EActDayFull, color = "b", lw = 2, linestyle = '--', label = 'Full Wind Speed')
    #ax1.plot(TimeRed, EActDayRed, color = "b", lw = 2, linestyle = '-', label = 'Reduced Wind Speed')
    
    #ax1.set_ylim(0,8)
    
    #ax1.set_ylabel('Daily Actual Evaporation [mm]', labelpad = 20)

    #ax1.set_xticks(Months)
    #myFmt = mdates.DateFormatter('%d-%m-%y')
    #plt.setp(ax1.xaxis.set_major_formatter(myFmt))
    #plt.setp(ax1.xaxis.get_majorticklabels(), rotation=90, ha = "center") 
    #ax1.grid()
    
    #handles, labels = ax1.get_legend_handles_labels()
    #lg = fig.legend(handles, labels, loc = "upper center", ncol=2)
    #lg.draw_frame(False)
        
    #subplots_adjust(left=0.04,right=0.98,bottom=0.18,top=0.9,wspace=0.2,hspace=0.2)
    
    #figfile=os.path.join(fig_path,'EvaporationActDay'+irr+'.png')
    
    #savefig(figfile)

    ## Daily Potential Evaporation

    #fig=plt.figure(figsize=(32,12),facecolor='w')
    #ax1 = subplot(111)
    
    #ax1.plot(TimeFull, EPotDayFull, color = "r", lw = 2, linestyle = '--', label = 'Full Wind Speed')
    #ax1.plot(TimeRed, EPotDayRed, color = "r", lw = 2, linestyle = '-', label = 'Reduced Wind Speed')
    
    #ax1.set_ylim(0,8)
    
    #ax1.set_ylabel('Daily Potential Evaporation [mm]', labelpad = 20)

    #ax1.set_xticks(Months)
    #myFmt = mdates.DateFormatter('%d-%m-%y')
    #plt.setp(ax1.xaxis.set_major_formatter(myFmt))
    #plt.setp(ax1.xaxis.get_majorticklabels(), rotation=90, ha = "center") 
    #ax1.grid()
    
    #handles, labels = ax1.get_legend_handles_labels()
    #lg = fig.legend(handles, labels, loc = "upper center", ncol=2)
    #lg.draw_frame(False)
        
    #subplots_adjust(left=0.04,right=0.98,bottom=0.18,top=0.9,wspace=0.2,hspace=0.2)
    
    #figfile=os.path.join(fig_path,'EvaporationPotDay'+irr+'.png')
    
    #savefig(figfile)
    
    
    ## Daily Actual Transpiration

    #fig=plt.figure(figsize=(32,12),facecolor='w')
    #ax1 = subplot(111)
    
    #ax1.plot(TimeFull, TActDayFull, color = "g", lw = 2, linestyle = '--', label = 'Full Wind Speed')
    #ax1.plot(TimeRed, TActDayRed, color = "g", lw = 2, linestyle = '-', label = 'Reduced Wind Speed')
    
    #ax1.set_ylim(0,7)
    
    #ax1.set_ylabel('Daily Actual Transpiration [mm]', labelpad = 20)

    #ax1.set_xticks(Months)
    #myFmt = mdates.DateFormatter('%d-%m-%y')
    #plt.setp(ax1.xaxis.set_major_formatter(myFmt))
    #plt.setp(ax1.xaxis.get_majorticklabels(), rotation=90, ha = "center") 
    #ax1.grid()
    
    #handles, labels = ax1.get_legend_handles_labels()
    #lg = fig.legend(handles, labels, loc = "upper center", ncol=2)
    #lg.draw_frame(False)
        
    #subplots_adjust(left=0.04,right=0.98,bottom=0.18,top=0.9,wspace=0.2,hspace=0.2)
    
    #figfile=os.path.join(fig_path,'TranspirationActDay'+irr+'.png')
    
    #savefig(figfile)

    ## Daily Potential Transpiration

    #fig=plt.figure(figsize=(32,12),facecolor='w')
    #ax1 = subplot(111)
    
    #ax1.plot(TimeFull, TPotDayFull, color = "#FFBD00", lw = 2, linestyle = '--', label = 'Full Wind Speed')
    #ax1.plot(TimeRed, TPotDayRed, color = "#FFBD00", lw = 2, linestyle = '-', label = 'Reduced Wind Speed')
    
    #ax1.set_ylim(0,7)
    
    #ax1.set_ylabel('Daily Potential Transpiration [mm]', labelpad = 20)

    #ax1.set_xticks(Months)
    #myFmt = mdates.DateFormatter('%d-%m-%y')
    #plt.setp(ax1.xaxis.set_major_formatter(myFmt))
    #plt.setp(ax1.xaxis.get_majorticklabels(), rotation=90, ha = "center") 
    #ax1.grid()
    
    #handles, labels = ax1.get_legend_handles_labels()
    #lg = fig.legend(handles, labels, loc = "upper center", ncol=2)
    #lg.draw_frame(False)
        
    #subplots_adjust(left=0.04,right=0.98,bottom=0.18,top=0.9,wspace=0.2,hspace=0.2)
    
    #figfile=os.path.join(fig_path,'TranspirationPotDay'+irr+'.png')
    
    #savefig(figfile)



#exit()

#dir_path = os.path.join(act_path,"WeatherDataProcessed")

#filename = os.path.join(act_path,"locations.csv")
#location_data = xpn_csv(filename, spacer=',', merge_delimiters=False,encoding='utf-8')
#location_data.convert_col(("altitude_m_asl","diff_utc"),float)
#locations, alts, diff_utcs = location_data["name","altitude_m_asl","diff_utc"]

#altitudes = dict(zip(locations,alts))
#diff_utc = dict(zip(locations,diff_utcs))
## make dictiorary of altitudes above sea level with respect to locations

##locations = ["scheyern_2013","scheyern_2014"]

## FAO ET0-Calculation: http://www.fao.org/docrep/X0490E/x0490e05.htm#TopOfPage

#for location_name in locations:
        #print location_name
        
        #if location_name not in diff_utc:
                #diff_utc.update({location_name: 1.0})
        
        #cum_hourly_filename = os.path.join(act_path,"ET/ET_cum_hourly_"+location_name+".csv")
        #cum_hourly_data = xpn_data(cum_hourly_filename,spacer=',',dateformat='%Y-%m-%d %H:%M:%S',merge_delimiters=False)
        #time,ET0,precips,balance,ET0_cum,precip_cum,balance_cum = cum_hourly_data["Time and Date","ET0","Precipitation","Balance","ET0_cum","Precipitation_cum","Balance_cum"]
        
        #daily_filename = os.path.join(act_path,"ET/ET_day_"+location_name+".csv")
        #daily_data = xpn_data(daily_filename,spacer=',',dateformat='%Y-%m-%d',merge_delimiters=False)
        #date,ET0_day,precip_day,balance_day = daily_data["Date","ET0_day","Precipitation_day","Balance_day"]
        
        #date_noon = np.array([datetime.datetime.combine(d, datetime.time.min) + datetime.timedelta(0.5) for d in date])
        #local_time =np.array([t + datetime.timedelta(hours=diff_utc[location_name]) for t in time])    
        
        ##### Plot
        
        #precips = -precips
        #precip_day = -precip_day
        #precip_cum = -precip_cum
        ## for nicer plotting
        
        #fig=plt.figure(figsize=(16,9),facecolor='w')
        
        #ax1 = subplot(111)
        #ax2 = twinx()
        #ax1.plot(local_time,ET0, color = "g", label = u"$ET_0$ [mm/h]")
        #ax1.plot(local_time,precips, color = "b", label = u"Precipitation [mm/h]")
        #ax1.plot(local_time,balance, color = "r", label = u"Balance [mm/h]")
        #ax1.axhline(y=0.0, color = "k")
        ##plot(time,ET0_cum)
        #ax2.bar(date_noon,ET0_day, color = "g", width = -0.3, align = "edge", edgecolor = "", alpha = 0.3, label = u"$ET_0$ [mm/d]")
        #ax2.bar(date_noon,precip_day, color = "b", width = -0.3, align = "edge", edgecolor = "", alpha = 0.3, label = u"Precipitation [mm/d]")
        #ax2.bar(date_noon,balance_day, color = "r", width = 0.3, align = "edge", edgecolor = "", alpha = 0.3, label = u"Balance [mm/d]")

        #l = ax1.get_ylim()
        #l2 = ax2.get_ylim()
        #ymin_1 = l[0]
        #ymax_1 = l[1]
        #ymin_2 = l2[0]
        #ymax_2 = l2[1]
        
        #if ymin_2 > -10.0 and ymin_2 % 2 != 0:
                #ymin_2 = math.floor(ymin_2/2)*2
        #if ymax_2 < 10.0 and ymax_2 % 2 != 0:
                #ymax_2 = math.ceil(ymax_2/2)*2
        #ax2.set_ylim(ymin_2,ymax_2)
        
        #tick_dist = ax2.get_yticks()[1] - ax2.get_yticks()[0]
        #tick_test_list = range(int(tick_dist) - 1, 0, -1)
        #if tick_dist == 1:
                #tick_test_list = [0.75,0.5,0.25]
        #if tick_dist == 0.5:
                #tick_test_list = [0.4,0.3,0.2,0.1]

        ##print tick_dist, tick_test_list
        
        #scale_fac = 1
        #for test in tick_test_list:
          #if min(precips) >= ymin_2 * test/tick_dist and max(ET0) <= ymax_2 * test/tick_dist:
                  #scale_fac = test/tick_dist
          #else:
                  #break
        #ymin_1 = scale_fac * ymin_2
        #ymax_1 = scale_fac * ymax_2
        
        #ax1.set_ylim(ymin_1,ymax_1)
        #ax1.set_yticks(ax2.get_yticks()*scale_fac)
        
        #matplotlib.rcParams.update({'font.size': 18})
        #setp(ax1.xaxis.get_majorticklabels(), rotation=30, ha = "right")
        #setp(ax2.xaxis.get_majorticklabels(), rotation=30, ha = "right")
        
        #ax1.grid()
        #ax2.grid()
        
        #ax2.set_ylabel(u"Daily water balance", labelpad = 25)
        #ax1.set_ylabel(u"Hourly water balance", labelpad = 25)
        
        #handles1, labels1 = ax1.get_legend_handles_labels()
        #handles2, labels2 = ax2.get_legend_handles_labels()
        #handles = [hand for handle in zip(handles1, handles2) for hand in handle]
        #labels = [lab for label in zip(labels1, labels2) for lab in label]
        #lg = fig.legend(handles, labels, loc = "upper center", ncol=3)
        #lg.draw_frame(False)
        
        #subplots_adjust(left=0.1,right=0.9,bottom=0.15,top=0.85,wspace=0.2,hspace=0.2)
        
        #figfile = "ET/ET_day_hourly_"+location_name+".png"
        #savefig(figfile)
        
        #close()
        
        
        #fig=plt.figure(figsize=(16,9),facecolor='w')
        
        #ET0_cum = np.array(ET0_cum,dtype=float)
        #precip_cum = np.array(precip_cum,dtype=float)
        #balance_cum = np.array(balance_cum,dtype=float)
        #balance_cum_pos = np.array([b if b >= 0 else 0.0 for b in balance_cum],dtype=float)
        #balance_cum_neg = np.array([b if b <= 0 else 0.0 for b in balance_cum],dtype=float)
        
        #ax1 = subplot(111)
        #ax1.plot(local_time,ET0_cum, color = "g", label = u"$ET_0$ [mm]")
        #ax1.fill_between(local_time,ET0_cum,balance_cum_pos, color = "g", alpha = 0.3)
        #ax1.plot(local_time,precip_cum, color = "b", label = u"Precipitation [mm]")
        #ax1.fill_between(local_time,precip_cum,balance_cum_neg, color = "b", alpha = 0.3)
        #ax1.plot(local_time,balance_cum, color = "r", label = u"Balance [mm]")
        #ax1.fill_between(local_time,balance_cum, 0.0, color = "r", alpha = 0.3)
        #ax1.axhline(y=0.0, color = "k")
        #ax1.grid()

        #setp(ax1.xaxis.get_majorticklabels(), rotation=30, ha = "right")
        
        #ax1.set_ylabel(u"Cumulative water balance", labelpad = 25)
        
        #handles, labels = ax1.get_legend_handles_labels()
        #lg = fig.legend(handles, labels, loc = "upper center", ncol=3)
        #lg.draw_frame(False)   
        
        #subplots_adjust(left=0.1,right=0.9,bottom=0.15,top=0.9,wspace=0.2,hspace=0.2)
        
        #figfile = "ET/ET_cum_"+location_name+".png"
        #savefig(figfile)
        
        #close()
        
#exit()
