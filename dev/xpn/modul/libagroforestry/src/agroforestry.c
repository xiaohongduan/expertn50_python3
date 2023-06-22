//      agroforestry.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//

#include "agroforestry.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


G_DEFINE_TYPE(agroforestry, agroforestry, EXPERTN_MODUL_BASE_TYPE);

static void agroforestry_class_init(agroforestryClass* klass)
{
}

static void agroforestry_init(agroforestry* self)
{
    //gchar* S;
    // hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
    // Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
    // Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!
    self->__ERROR = RET_SUCCESS;
    // init example var:

    self->fAzimuthAngle = 0.0;
    self->fDiffAngle = 0.0;
    self->fMaxShadeLength = 0.0;
    self->fTreeHeight = 0.0;
    self->fViewAngle = 0.0;
    self->fZenithAngle = 0.0;
    self->__INIT_DONE = 0;

    // print Output Message if Modul is used from Expert N
    //S = g_strdup_printf("%s\t%d\tInit MODUL AGROFORESTRY!", __FILE__, __LINE__);
    //fprintf(stdout, "%s\n", S);
    //fflush(stdout);
    //g_free(S);
}

int agroforestry_done(agroforestry *self)
{
int i;
g_free(self->reg_string_tree);
g_free(self->reg_string_beside_tree);
g_free(self->fDistStripAll);
g_free(self->grid_id_beside_tree);
g_free(self->grid_i_beside_tree);
g_free(self->grid_j_beside_tree);
g_free(self->grid_sub_beside_tree);
for(i=0; i < self->grid_id_beside_tree_len + 1; i++)
		g_free(self->fTreeRootsAtGridPoints[i]);
        g_free(self->fCropRootsAtGridPoints[i]);
        g_free(self->fTotalRootsAtGridPoints[i]);
g_free(self->fTreeRootsAtGridPoints);
g_free(self->fCropRootsAtGridPoints);
g_free(self->fTotalRootsAtGridPoints);
g_free( self->fTreeRootFracsAtGridPoints);
g_free( self->iRootDistributionBesideTreeAtGridPoints);

//g_free(self->TreeRootsAtGridPoint);
//g_free(self->CropRootsAtGridPoint);
//g_free(self->TotalRootsAtGridPoint);
g_free(self->xn_ids_beside_tree);
return RET_SUCCESS;
}

/*#define CHECK_ZERO_ONE(var,inif) \
	if ((var < 0.0) || (var > 1.0)) \
		{ \
			gchar *S; \
			S = g_strdup_printf  ("%s is %f, but must be between 0.0 and 1.0, check %s!",#var,var,inif); \
			PRINT_ERROR(S); \
			g_free(S); \
		} \*/


int init_agroforestry(agroforestry *self)
{
	expertn_modul_base *xpn = &(self->parent);
    expertnclass** xpn_class=(expertnclass **)xpn->pXSys->xpn_classes;
	
	char *ini_filename;
	gchar *reg_string;
	gchar* S;
    gchar *reg_string_beside_tree_temp;
	int i, j, k, check;
	double DeadLeaf_beside_frac_sum, DeadStem_beside_frac_sum, DeadBranch_beside_frac_sum,DeadFineRoot_beside_frac_sum,DeadGrossRoot_beside_frac_sum;
    double Root_beside_frac_sum, Tree_Root_frac_sum;
	
//	PTIMESTEP		pTS = xpn->pTi->pTimeStep;
	//PPLANT			pPl = xpn->pPl;
	PCHEMISTRY pCh = xpn->pCh; //added by Hong
	//PNFERTILIZER fertil_first,fertil,fertil_act;

	// Wenn die Init bereits geladen ist
	if (self->__INIT_DONE==0) {
		self->__INIT_DONE=1;
	} else {
		return RET_SUCCESS;
	}
	
	
	// Read from INI File:
	ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.agroforestry.filename");

	if (ini_filename==NULL) {
		PRINT_ERROR("Missing entry 'agroforestry.filename' in your configuration!");
	} else {

		char *S2;
		S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
		if (S2!=NULL) {
			ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
			if (agroforestry_read(self,ini_filename)!=0) {
				PRINT_ERROR("Error Read AGROFORESTRY INI File");
			}
			self->ini_filename = g_strdup_printf("%s",ini_filename);
			free(ini_filename);
			free(S2);
		}

	}
	
	self->reg_string_act = g_strdup_printf("%s",xpn->pXSys->reg_str);
	check = 0;
	
    // FH 20190730 This variables should be initialized in general, not only within a if statement
    self->fDistStrip = 0.0;
	self->DeadLeaf_beside_tree_frac = 0.0;
	self->DeadStem_beside_tree_frac = 0.0;
	self->DeadFineRoot_beside_tree_frac = 0.0;
	self->DeadGrossRoot_beside_tree_frac = 0.0;
	self->DeadLeaf_tree_frac = 0.0;
	self->DeadStem_tree_frac = 0.0;
	self->DeadBranch_tree_frac = 0.0;
	self->DeadFineRoot_tree_frac = 0.0;
	self->DeadGrossRoot_tree_frac = 0.0;
    
	DeadLeaf_beside_frac_sum = 0.0;
    DeadStem_beside_frac_sum = 0.0;
	DeadBranch_beside_frac_sum = 0.0;
	DeadFineRoot_beside_frac_sum = 0.0;
	DeadGrossRoot_beside_frac_sum = 0.0;
	//Allocate the variables to the individual grid points
	for(i = 0; i < self->grid_id_beside_tree_len; i++)
	{	
		self->reg_string_beside_tree = g_strdup_printf("%d_%d_%d_%d",self->grid_id_beside_tree[i],self->grid_i_beside_tree[i],self->grid_j_beside_tree[i],self->grid_sub_beside_tree[i]);
		DeadLeaf_beside_frac_sum += self->DeadLeaf_beside_tree_array[i];
		DeadStem_beside_frac_sum += self->DeadStem_beside_tree_array[i];
		DeadBranch_beside_frac_sum += self->DeadBranch_beside_tree_array[i];
		DeadFineRoot_beside_frac_sum += self->DeadFineRoot_beside_tree_array[i];
		DeadGrossRoot_beside_frac_sum += self->DeadGrossRoot_beside_tree_array[i];
		if (strcmp(self->reg_string_act,self->reg_string_beside_tree)==0)
		{
			self->fDistStrip = self->fDistStripAll[i];
			self->DeadLeaf_beside_tree_frac = self->DeadLeaf_beside_tree_array[i];
			self->DeadStem_beside_tree_frac = self->DeadStem_beside_tree_array[i];
			self->DeadFineRoot_beside_tree_frac = self->DeadFineRoot_beside_tree_array[i];
			self->DeadGrossRoot_beside_tree_frac = self->DeadGrossRoot_beside_tree_array[i];
/*			CHECK_ZERO_ONE(self->DeadLeaf_beside_tree_frac,self->ini_filename);
			CHECK_ZERO_ONE(self->DeadStem_beside_tree_frac,self->ini_filename);
			CHECK_ZERO_ONE(self->DeadRoot_beside_tree_frac,self->ini_filename);*/
			check = 1;
			break;
		}
	}	
	

    // FH 20191017 +1 because we have the crops grid points + 1 tree grid point
    self->fTreeRootsAtGridPoints = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
    self->fCropRootsAtGridPoints = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
    self->fTotalRootsAtGridPoints = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
    self->fTreeRootFracsAtGridPoints = (double*)g_malloc0((self->grid_id_beside_tree_len + 1)*sizeof(double));
    self->iRootDistributionBesideTreeAtGridPoints = (int*)g_malloc0((self->grid_id_beside_tree_len + 1)*sizeof(int));
    for(i = 0; i < self->grid_id_beside_tree_len + 1; i++)
        {
        self->fTreeRootsAtGridPoints[i] = (double*)g_malloc0(xpn->pSo->iLayers*sizeof(double));
        self->fCropRootsAtGridPoints[i] = (double*)g_malloc0(xpn->pSo->iLayers*sizeof(double));
        self->fTotalRootsAtGridPoints[i] = (double*)g_malloc0(xpn->pSo->iLayers*sizeof(double));
        }
    
    //self->TreeRootsAtGridPoint = (double *)g_malloc0(sizeof(double)*xpn->pSo->iLayers);
    //self->CropRootsAtGridPoint = (double *)g_malloc0(sizeof(double)*xpn->pSo->iLayers);
    //self->TotalRootsAtGridPoint = (double *)g_malloc0(sizeof(double)*xpn->pSo->iLayers);
	//printf("%f %f \n",self->DeadLeaf_beside_tree_frac, Leaf_beside_frac_sum);

	g_free(self->DeadLeaf_beside_tree_array);
	g_free(self->DeadStem_beside_tree_array);
	g_free(self->DeadBranch_beside_tree_array);
	g_free(self->DeadFineRoot_beside_tree_array);
	g_free(self->DeadGrossRoot_beside_tree_array);
	g_free(self->reg_string_act);	
	
    
    
    
	if(self->iTree == 1)
		{
		check = 1;
		self->DeadLeaf_tree_frac = 1.0 - DeadLeaf_beside_frac_sum;
		self->DeadStem_tree_frac = 1.0 - DeadStem_beside_frac_sum;
		self->DeadBranch_tree_frac = 1.0 - DeadBranch_beside_frac_sum;
		self->DeadFineRoot_tree_frac = 1.0 - DeadFineRoot_beside_frac_sum;
		self->DeadGrossRoot_tree_frac = 1.0 - DeadGrossRoot_beside_frac_sum;
		}
	
	// Check if grid point is included into agroforestry module
	if(check != 1)
	{
		S = g_strdup_printf("Actual grid point is not included into agroforestry configuration, check [grid_positions] in %s", self->ini_filename);
		PRINT_ERROR(S);
		g_free(S);
		return -1;		
	}


	// Allocate internal Expert-N ID to the grid points and connect tree strip with the strips beside
	self->reg_string_tree = g_strdup_printf("%d_%d_%d_%d",self->grid_id_tree,self->grid_i_tree,self->grid_j_tree,self->grid_sub_tree);
	
	self->xn_id_tree = -99;
	self->xn_id_beside_tree = -99;



	for(i = 0; i < xpn->pXSys->id_N; i++)
	{	
		reg_string = g_strdup_printf(xpn_class[i]->pXSys->reg_str);
		//printf("%s %s \n", self->reg_string_beside_tree, reg_string);
		if (strcmp(reg_string,self->reg_string_tree)==0)
			{
			self->xn_id_tree = (int)xpn_class[i]->pXSys->id;
			S  = g_strdup_printf("Agroforestry: The internal Id of the tree strip is %d with reg string %s \n",i, reg_string);
			PRINT_MESSAGE(xpn,3,S);
			g_free(S);
			}
		if (strcmp(reg_string,self->reg_string_beside_tree)==0)
			{
			self->xn_id_beside_tree = (int)xpn_class[i]->pXSys->id;
			S  = g_strdup_printf("Agroforestry: The internal Id of the strip next to the trees is %d with reg string %s \n",i, reg_string);
			PRINT_MESSAGE(xpn,3,S);
			g_free(S);
			}
		g_free(reg_string);
		if ((self->xn_id_tree != -99) && (self->xn_id_beside_tree != -99))
			break;
	}
	
        //FH 20191015 to get all the ids beside tree
        self->xn_ids_beside_tree = (int*)g_malloc0(sizeof(int)*self->grid_id_beside_tree_len);
        j = 0;
        Root_beside_frac_sum = 0.0;
    	for(i = 0; i < self->grid_id_beside_tree_len; i++)
            {	
            Root_beside_frac_sum += self->Roots_beside_tree_frac_array[i];    
            reg_string_beside_tree_temp = g_strdup_printf("%d_%d_%d_%d",self->grid_id_beside_tree[i],self->grid_i_beside_tree[i],self->grid_j_beside_tree[i],self->grid_sub_beside_tree[i]);
            for(k = 0; k < xpn->pXSys->id_N; k++)
                {	
                reg_string = g_strdup_printf(xpn_class[k]->pXSys->reg_str);
                if (strcmp(reg_string,reg_string_beside_tree_temp)==0)
                    {
                    self->xn_ids_beside_tree[j] = (int)xpn_class[k]->pXSys->id;
                    // FH 20191022 j+1 because for this variable tree is [0]
                    self->fTreeRootFracsAtGridPoints[j+1] =  self->Roots_beside_tree_frac_array[i];
                    self->iRootDistributionBesideTreeAtGridPoints[j+1] =  self->iRootDistributionBesideTree_array[i];
                    j += 1;
                    }
                g_free(reg_string);
                }
            g_free(reg_string_beside_tree_temp);
            }
      self->fTreeRootFracsAtGridPoints[0] = 1.0 - Root_beside_frac_sum;
      // FH 20190105 not needed at tree grid point
      self->iRootDistributionBesideTreeAtGridPoints[0] = -99;
	
    Tree_Root_frac_sum = 0.0;
    for(i = 0; i < self->grid_id_beside_tree_len + 1; i++)
        {
        if(self->fTreeRootFracsAtGridPoints[i] < 0.0)
            {
            S = g_strdup_printf("Some tree root fraction is negative. Check Roots_beside_tree_frac at [grid_positions] in the file %s . This parameter just accepts positive values and the sum of all values must not exceed 1.0.", self->ini_filename);
            PRINT_ERROR(S);
            g_free(S);
            }
        Tree_Root_frac_sum += self->fTreeRootFracsAtGridPoints[i];
        //printf("id %s i %d Tree_Root_frac_at_GP %f \n", xpn->pXSys->reg_str,i, self->fTreeRootFracsAtGridPoints[i]);
        }
    //printf("id %s Tree_Root_frac_sum %f \n", xpn->pXSys->reg_str, Tree_Root_frac_sum);
    
	if(self->xn_id_tree == -99)
	{
		S = g_strdup_printf("Error Grid position of tree strip is not correctly defined. Check %s at [grid_positions]", self->ini_filename);
		PRINT_ERROR(S);
		g_free(S);
	}

	if(self->xn_id_beside_tree == -99)
	{
		S = g_strdup_printf("Error Grid position of strip next to trees is not correctly defined. Check %s at [grid_positions]", self->ini_filename);
		PRINT_ERROR(S);
		g_free(S);
	}
	
	if (self->xn_id_beside_tree == self->xn_id_tree)
	{
		S = g_strdup_printf("Error Tree strip and the strip next to the trees must not have the same grid position. Check %s at [grid_positions]", self->ini_filename);
		PRINT_ERROR(S);
		g_free(S);
		return -1;
	}

	pCh->pCProfile->fUsedLeafWeight = 0.0;
	pCh->pCProfile->fUsedStemWeight = 0.0;
	pCh->pCProfile->fUsedRootWeight = 0.0;


    g_free(self->Roots_beside_tree_frac_array);	
    g_free(self->iRootDistributionBesideTree_array);	
	//printf("pCh->pCProfile->fUsedLeafWeight = 0.0 \n");
return RET_SUCCESS;
}


#define GET_INI_DOUBLE(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_double (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		} \

#define GET_INI_DOUBLE_OPTIONAL(var,groupname,key,std_value)\
{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (key_exists==FALSE) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (in file %s) is missing. Standard Value (%f) taken instead!\n",groupname,key,filename,std_value);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{\
				var = std_value;\
			} else\
			{\
				GET_INI_DOUBLE(var,groupname,key);\
			}\
	}\
    
 #define GET_INI_INT(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_integer (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		}\
    

 #define GET_INI_INT_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_integer_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(xpn,S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\
	 
 #define GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_double_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(xpn,S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\
	

 #define GET_INI_DOUBLE_ARRAY_OPTIONAL(var,var_size,expected_len,std_value,groupname,key) \
	{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(xpn,S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{ \
				int i;\
				var_size=expected_len;\
				var = g_malloc0(sizeof(double)*expected_len);\
				for (i=0;i<expected_len;i++)\
					{\
						var[i] = std_value;\
					}\
			} else\
			{\
				GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key);\
			}\
	}\
	
 #define CHECK_LEN(var1,var2)\
	{\
		if (var1!=var2)\
			{\
				gchar *S;\
				S = g_strdup_printf  ("%s is not %s. Check your configuration in file: %s!",#var1,#var2,filename);\
				PRINT_ERROR(S);\
				g_free(S);\
				return -1;\
			}\
	}\

agroforestry_read(agroforestry *self,const char* configfile)
{
expertn_modul_base* xpn = EXPERTN_MODUL_BASE(self);
	
GKeyFile *keyfile;
GKeyFileFlags flags;
GError *error = NULL;
const char* filename=configfile;
keyfile = g_key_file_new ();
flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	/* Load the GKeyFile from keyfile.conf or return. */
if (!g_key_file_load_from_file (keyfile, configfile, flags, &error))
		{
			PRINT_ERROR(error->message);
			return -1;
		};


 GET_INI_INT( self->iAverageShading,"grid_positions","AverageShading");
 GET_INI_DOUBLE( self->fViewAngle,"grid_positions","ViewAngle");  
 GET_INI_INT(self->iTreeHeightData,"agroforestry","TreeHeightData");
 GET_INI_DOUBLE( self->fTreeHeight,"agroforestry","TreeHeight");
 GET_INI_DOUBLE( self->fTreeWidth,"agroforestry","TreeStripWidth");
 GET_INI_INT( self->iCalcLightAttenuation,"agroforestry","LightAttenuationCalc");
 GET_INI_DOUBLE_OPTIONAL( self->fFixedLightAttenuationFactor,"agroforestry","LightAttenuationFactor",0.5);
 GET_INI_INT( self->iCalcExtinction,"agroforestry","ExtinctionCalculation");
 GET_INI_DOUBLE_OPTIONAL(self->fLAI,"agroforestry","LAI",3.0);
 GET_INI_DOUBLE_OPTIONAL( self->fExtinctionCoefficient,"agroforestry","ExtinctionCoefficient",1.0);
 GET_INI_INT( self->CN_calculate,"agroforestry","CN_calculate");
 GET_INI_DOUBLE_OPTIONAL(self->CN_fromDeadLeaf, "agroforestry","CN_fromDeadLeaf",0.0);
 GET_INI_DOUBLE_OPTIONAL(self->CN_fromDeadBranch, "agroforestry","CN_fromDeadBranch",0.0);
 GET_INI_DOUBLE_OPTIONAL(self->CN_fromDeadStem, "agroforestry","CN_fromDeadStem",0.0);
 GET_INI_DOUBLE_OPTIONAL(self->CN_fromDeadFineRoot, "agroforestry","CN_fromDeadFineRoot",0.0);
 GET_INI_DOUBLE_OPTIONAL(self->CN_fromDeadGrossRoot, "agroforestry","CN_fromDeadGrossRoot",0.0);
 
 GET_INI_INT(self->iTree,"grid_positions","tree");
 GET_INI_INT(self->grid_id_tree,"grid_positions","grid_id_tree")
 GET_INI_INT(self->grid_i_tree,"grid_positions","grid_i_tree")
 GET_INI_INT(self->grid_j_tree,"grid_positions","grid_j_tree")
 GET_INI_INT(self->grid_sub_tree,"grid_positions","grid_sub_tree")
 GET_INI_INT_ARRAY(self->grid_id_beside_tree,self->grid_id_beside_tree_len,"grid_positions","grid_ids_beside_tree")
 GET_INI_INT_ARRAY(self->grid_i_beside_tree,self->grid_i_beside_tree_len,"grid_positions","grid_is_beside_tree")
 GET_INI_INT_ARRAY(self->grid_j_beside_tree,self->grid_j_beside_tree_len,"grid_positions","grid_js_beside_tree")
 GET_INI_INT_ARRAY(self->grid_sub_beside_tree,self->grid_sub_beside_tree_len,"grid_positions","grid_subs_beside_tree")
 GET_INI_DOUBLE_ARRAY_OPTIONAL(self->fDistStripAll,self->fDistStripAllLen,1,-99.9,"grid_positions","DistancesfromTreeStrip");  
 GET_INI_DOUBLE_ARRAY(self->DeadLeaf_beside_tree_array,self->DeadLeaf_beside_tree_len,"grid_positions","DeadLeaf_beside_tree_frac")
 GET_INI_DOUBLE_ARRAY(self->DeadStem_beside_tree_array,self->DeadStem_beside_tree_len,"grid_positions","DeadStem_beside_tree_frac")
 GET_INI_DOUBLE_ARRAY(self->DeadBranch_beside_tree_array,self->DeadBranch_beside_tree_len,"grid_positions","DeadBranch_beside_tree_frac")
 GET_INI_DOUBLE_ARRAY(self->DeadFineRoot_beside_tree_array,self->DeadFineRoot_beside_tree_len,"grid_positions","DeadFineRoot_beside_tree_frac")
 GET_INI_DOUBLE_ARRAY(self->DeadGrossRoot_beside_tree_array,self->DeadGrossRoot_beside_tree_len,"grid_positions","DeadGrossRoot_beside_tree_frac")
 GET_INI_DOUBLE_ARRAY(self->Roots_beside_tree_frac_array,self->Roots_beside_tree_frac_len,"grid_positions","Roots_beside_tree_frac")
 GET_INI_INT_ARRAY(self->iRootDistributionBesideTree_array,self->iRootDistributionBesideTree_len,"grid_positions","Root_distribution_beside_tree")
 
 GET_INI_DOUBLE_OPTIONAL(self->fMaxShadeLength,"grid_positions","MaxShadeLength",10.0);  

 
 CHECK_LEN(self->grid_id_beside_tree_len,self->grid_i_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->grid_j_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->grid_sub_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->fDistStripAllLen);
 CHECK_LEN(self->grid_id_beside_tree_len,self->DeadLeaf_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->DeadStem_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->DeadBranch_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->DeadFineRoot_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->DeadGrossRoot_beside_tree_len);
 CHECK_LEN(self->grid_i_beside_tree_len,self->Roots_beside_tree_frac_len);

g_key_file_free(keyfile);
  
return RET_SUCCESS; 
}

// Our Modul Functions:

int agroforestry_run(agroforestry* self)
{
    expertn_modul_base* xpn = EXPERTN_MODUL_BASE(self);
    expertnclass** xpn_class=(expertnclass **)xpn->pXSys->xpn_classes;
    PCLIMATE pCl = xpn->pCl;
	PCPROFILE	pCP = xpn->pCh->pCProfile;
	PPLANT pPl = xpn->pPl;
	PCBALANCE	    pCB = xpn->pCh->pCBalance; //Added by Hong on 20181126
	PSPROFILE pSo = xpn->pSo;//Added by Hong on 20181127
	PSLAYER         pSL     =pSo->pSLayer->pNext;//Added by Hong on 20181127
	PCLAYER pCL;
	PCHEMISTRY pCh = xpn->pCh;
	PLAYERROOT      pLR     =pPl->pRoot->pLayerRoot; //Added by Hong on 20190221
//	double DELT = (double)xpn->pTi->pTimeStep->fAct; //Hong on 20181127
	int i2;

    double fReductionFactor, fShadedFraction, fShadedLength, fAttenuationFactor;
    double fAverageLengthThroughTrees, fWidth0, fHeight0,fZenithTan;
	double fLAIeff;
	double fTreeDeadLeafWeight, fTreeDeadStemWeight, fTreeDeadBranchWeight, fTreeDeadRootWeight;
	double fCNTreeDeadLeaf, fCNTreeDeadStem, fCNTreeDeadBranch;
    //double fTreeDeadFineRootWeight_Layer[200], fTreeDeadGrossRootWeight_Layer[200], fCNTreeDeadFineRoot_Layer[200], fCNTreeDeadGrossRoot_Layer[200]; 
    // FH 20190802 made double* vairables instead of [200] -> flexible memory allocation depending on the amount of soil layers.
    double *fTreeDeadFineRootWeight_Layer, *fTreeDeadGrossRootWeight_Layer, *fCNTreeDeadFineRoot_Layer, *fCNTreeDeadGrossRoot_Layer; 
	double fCumDepth;
    
	fTreeDeadLeafWeight=0.0;
    fTreeDeadStemWeight=0.0;
    fTreeDeadRootWeight=0.0;
	//memset(fTreeDeadFineRootWeight_Layer, 0.0, sizeof fTreeDeadFineRootWeight_Layer);
	//memset(fTreeDeadGrossRootWeight_Layer, 0.0, sizeof fTreeDeadGrossRootWeight_Layer);
    //memset(fCNTreeDeadFineRoot_Layer, 0.0, sizeof fCNTreeDeadFineRoot_Layer);
	//memset(fCNTreeDeadGrossRoot_Layer, 0.0, sizeof fCNTreeDeadGrossRoot_Layer);
	fTreeDeadFineRootWeight_Layer = g_malloc0((pSo->iLayers )*sizeof(double));
    fTreeDeadGrossRootWeight_Layer =  g_malloc0((pSo->iLayers)*sizeof(double));
    fCNTreeDeadFineRoot_Layer =  g_malloc0((pSo->iLayers)*sizeof(double));
    fCNTreeDeadGrossRoot_Layer =  g_malloc0((pSo->iLayers)*sizeof(double));
    
    
	fTreeDeadLeafWeight = max(xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadLeafWeight_dt, (double)0.0);
	
	fTreeDeadStemWeight = max(xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadStemWeight_dt, (double)0.0);
	
	fTreeDeadBranchWeight = max(xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadBranchWeight_dt, (double)0.0);
	
	if (self->CN_calculate == 1)
	    {
			fCNTreeDeadLeaf = xpn_class[self->xn_id_tree]->pCh->pCProfile->fCNLeafLitterSurf;
			fCNTreeDeadStem = xpn_class[self->xn_id_tree]->pCh->pCProfile->fCNStemLitterSurf;
			fCNTreeDeadBranch = xpn_class[self->xn_id_tree]->pCh->pCProfile->fCNBranchLitterSurf;
		}
	else
	   {
		   fCNTreeDeadLeaf =self->CN_fromDeadLeaf;
		   fCNTreeDeadStem =self->CN_fromDeadStem;
           fCNTreeDeadBranch= self->CN_fromDeadBranch;
	   }
	
	//Added by Hong
	
	pSo = xpn_class[self->xn_id_tree]->pSo;
	pLR = xpn_class[self->xn_id_tree]->pPl->pRoot->pLayerRoot;
	pCL = xpn_class[self->xn_id_tree]->pCh->pCLayer;
	for (pSL = pSo->pSLayer->pNext,
		     pLR = pLR->pNext, pCL= pCL->pNext, i2=0;
		     ((pSL->pNext != NULL)&&
		      (pLR->pNext != NULL));
		     pSL = pSL->pNext,
		     pLR = pLR->pNext,pCL= pCL->pNext,i2++)  {
		
			fTreeDeadFineRootWeight_Layer[i2] = pLR->fDeadFineRootWeight_dt;  
			fTreeDeadGrossRootWeight_Layer[i2] = pLR->fDeadGrossRootWeight_dt;
			if (self->CN_calculate == 1)
	          {
			   fCNTreeDeadFineRoot_Layer[i2] = pCL->fCNFineRootLitter;
			   fCNTreeDeadGrossRoot_Layer[i2] = pCL->fCNGrossRootLitter;
		       }
		    else
			  {
				fCNTreeDeadFineRoot_Layer[i2] = self->CN_fromDeadFineRoot;
				fCNTreeDeadGrossRoot_Layer[i2] = self->CN_fromDeadGrossRoot;
			  }
			  
			fTreeDeadRootWeight += fTreeDeadFineRootWeight_Layer[i2]+fTreeDeadGrossRootWeight_Layer[i2];
			
		}
		
	//pSo = xpn->pSo;
	//End of Hong
	
    //FH 20191001 add function for root competition; if self->fTreeRootFracsAtGridPoints[0] < 1.0 root competition takes place:
    // then there are roots at other grid points than the tree grid point
    
    if(self->fTreeRootFracsAtGridPoints[0] < 1.0)
    {
        agroforestry_root_competition(self);
    }
    
    //End of FH
	
	if(self->iTree == 1) //Tree strip
	{
	fReductionFactor = 0.0;
	
/*	// Litter and Manure from trees to soil surface
	pCP->fCLitterSurf    += (fTreeDeadLeafWeight * self->C_fromDeadLeaf_frac * self->LeaftoLitter_frac * self->DeadLeaf_tree_frac
	                         +fTreeDeadStemWeight * self->C_fromDeadStem_frac * self->StemtoLitter_frac * self->DeadStem_tree_frac);
	pCP->fCManureSurf	 += (fTreeDeadLeafWeight * self->C_fromDeadLeaf_frac * self->LeaftoManure_frac * self->DeadLeaf_tree_frac
							 +fTreeDeadStemWeight * self->C_fromDeadStem_frac * self->StemtoManure_frac * self->DeadStem_tree_frac);
	pCP->fNLitterSurf    += (fTreeDeadLeafWeight * self->N_fromDeadLeaf_frac * self->LeaftoLitter_frac * self->DeadLeaf_tree_frac
	                         +fTreeDeadStemWeight * self->N_fromDeadStem_frac * self->StemtoLitter_frac * self->DeadStem_tree_frac);
	pCP->fNManureSurf	 += (fTreeDeadLeafWeight * self->N_fromDeadLeaf_frac * self->LeaftoManure_frac * self->DeadLeaf_tree_frac
							 +fTreeDeadStemWeight * self->N_fromDeadStem_frac * self->StemtoManure_frac * self->DeadStem_tree_frac);
							 
*/							 
	// Litter from trees reduced due to distribution to the field
		pCP->fCLitterSurf    -= fTreeDeadLeafWeight *  (1-self->DeadLeaf_tree_frac)
	                         +fTreeDeadStemWeight * (1-self->DeadStem_tree_frac)         +fTreeDeadBranchWeight * (1-self->DeadBranch_tree_frac);
	
	    pCP->fNLitterSurf    -= fTreeDeadLeafWeight /fCNTreeDeadLeaf * (1-self->DeadLeaf_tree_frac)
	                         + fTreeDeadStemWeight /fCNTreeDeadStem * (1-self->DeadStem_tree_frac) + fTreeDeadBranchWeight /fCNTreeDeadBranch * (1-self->DeadBranch_tree_frac);
			 
	
/*    pCB->dCInputCum -= fTreeDeadLeafWeight * (1-self->DeadLeaf_tree_frac)
	                         +fTreeDeadStemWeight * (1-self->DeadStem_tree_frac)         +fTreeDeadBranchWeight  * (1-self->DeadBranch_tree_frac); */
pCB->dCInputSurf -= fTreeDeadLeafWeight * (1-self->DeadLeaf_tree_frac)
	                         +fTreeDeadStemWeight * (1-self->DeadStem_tree_frac)         +fTreeDeadBranchWeight  * (1-self->DeadBranch_tree_frac);			 

	// Calculate "used" weights for each grid point and each plant compartment separately
	pCh->pCProfile->fUsedLeafWeight += fTreeDeadLeafWeight * (1-self->DeadLeaf_tree_frac);
	pCh->pCProfile->fUsedStemWeight += fTreeDeadStemWeight * (1-self->DeadStem_tree_frac);
	pCh->pCProfile->fUsedBranchWeight += fTreeDeadBranchWeight * (1-self->DeadBranch_tree_frac);
	
/*	gchar* S;
	S = g_strdup_printf(" pPl->pBiomass->fUsedLeafWeight %f \n", pPl->pBiomass->fUsedLeafWeight);
	PRINT_MESSAGE(xpn,3,S);
*/	
/*	if ((xpn_time_compare_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,
		    2019,8,15) ==0 ))
		//if (xpn->pTi->pSimTime->fTimeDay>0.5&&xpn->pTi->pSimTime->fTimeDay<0.6)
 {		
    gchar* S;
	S = g_strdup_printf(" pCh->pCProfile->fUsedLeafWeight: %f, fTreeDeadLeafWeight:  %f, (1-self->DeadLeaf_tree_frac: %f \n", pCh->pCProfile->fUsedLeafWeight, fTreeDeadLeafWeight, (1-self->DeadLeaf_tree_frac));
	PRINT_MESSAGE(xpn,3,S);	
	
}
	
if ((xpn_time_compare_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,
		    2019,8,16) ==0 ))
		//if (xpn->pTi->pSimTime->fTimeDay>0.5&&xpn->pTi->pSimTime->fTimeDay<0.6)
 {		
    gchar* S;
	S = g_strdup_printf(" pCh->pCProfile->fUsedLeafWeight: %f, fTreeDeadLeafWeight:  %f, (1-self->DeadLeaf_tree_frac: %f \n", pCh->pCProfile->fUsedLeafWeight, fTreeDeadLeafWeight, (1-self->DeadLeaf_tree_frac));

	PRINT_MESSAGE(xpn,3,S);	
	
}	
*/

	/* dead roots distributed to field*/
	pSo = xpn->pSo;
	//pLR = xpn->pPl->pRoot->pLayerRoot;
	fCumDepth= 0.0;
    //FH 20190808 added LR for root competition
		for (pSL = pSo->pSLayer->pNext, pCL = pCh->pCLayer->pNext, pLR = pPl->pRoot->pLayerRoot->pNext ,i2=0;
		     ((pSL->pNext != NULL)&&(pCL->pNext != NULL)&&(pLR->pNext != NULL));
		     pSL = pSL->pNext, pCL = pCL->pNext, pLR = pLR->pNext, i2++) {
			//Check the whether there are roots in this layer:
			//if (pLR->fLengthDens==(double)0.0)           break;
			pCL->fCLitter -= fTreeDeadFineRootWeight_Layer[i2] * (1-self->DeadFineRoot_tree_frac)+
			                 fTreeDeadGrossRootWeight_Layer[i2] * (1-self->DeadGrossRoot_tree_frac);
			  
			  pCL->fNLitter -= fTreeDeadFineRootWeight_Layer[i2] / fCNTreeDeadFineRoot_Layer[i2] * (1-self->DeadFineRoot_tree_frac) +              fTreeDeadGrossRootWeight_Layer[i2] / fCNTreeDeadGrossRoot_Layer[i2] * (1-self->DeadGrossRoot_tree_frac);
			  
			/*  pCB->dCInputCum -= fTreeDeadFineRootWeight_Layer[i2] * (1-self->DeadFineRoot_tree_frac)+
			                 fTreeDeadGrossRootWeight_Layer[i2] * (1-self->DeadGrossRoot_tree_frac); */
							
			  pCB->dCInputProfile -= fTreeDeadFineRootWeight_Layer[i2] * (1-self->DeadFineRoot_tree_frac)+
			                 fTreeDeadGrossRootWeight_Layer[i2] * (1-self->DeadGrossRoot_tree_frac);
			//Hong 20190507: balance for 0-30 cm profile:
			fCumDepth +=(double)0.1*pSL->fThickness; //cm
			if (fCumDepth <=30.0)
					{
						pCB->dCInputProfile_30 -= fTreeDeadFineRootWeight_Layer[i2] * (1-self->DeadFineRoot_tree_frac)+
			                 fTreeDeadGrossRootWeight_Layer[i2] * (1-self->DeadGrossRoot_tree_frac);
					}
            
		}
	
	
	}
	else //Strip next to trees
	{

    fReductionFactor = 0.0;
    fShadedFraction = 0.0;
    fShadedLength = 0.0;
    fAttenuationFactor = 1.0;

    // Trees are always in the right of the viewer, the view angle fViewAngle is defined as the angle of the tree strip
    // turning around clockwise with 0°
    // in the North. Thus, when the tree row is from North to South and the viewer is in the West, the view angle is 0°
    // while it is
    // 180° when the viewer is in the East.
    // The azimuth angle fAzimuthAngle depends on the time of the day and is also counted clockwise with 0° in the North
    // fDiffAngle is the measure if the tree strip is completely sunlit or not.

    sunpos(self);

    //printf("fTimeDay %f SolRad at start %f \n", xpn->pTi->pSimTime->fTimeDay, pCl->pWeather->fSolRad);

    self->fDiffAngle = 360 + self->fAzimuthAngle - self->fViewAngle;

    while(self->fDiffAngle >= 360) {
        self->fDiffAngle -= 360;
    }
    
    if(self->fZenithAngle < 0) {
        self->fZenithAngle = -self->fZenithAngle;
        }

    fZenithTan = tan(self->fZenithAngle*PI/180.) ;

	if(self->iTreeHeightData == 1) //Tree height from other XN grid point is used, else the input value from project__agroforestry.ini
	{
		self->fTreeHeight = xpn_class[self->xn_id_tree]->pPl->pCanopy->fPlantHeight;
	}

    if((self->fDiffAngle >= 180) && (self->fDiffAngle <= 360)) {
        // pCl->pWeather->fSolRad *= 0.5;
        if(self->fZenithAngle < 90) {
            fShadedLength = fZenithTan * self->fTreeHeight;
			if (self->iAverageShading == 1){
				if(fShadedLength < self->fMaxShadeLength) {
					fShadedFraction = fShadedLength / self->fMaxShadeLength;
				} else {
					fShadedFraction = 1.0;
				}
			} else if (self->iAverageShading == 0) {
				if(fShadedLength < self->fDistStrip) {
					fShadedFraction = 0.0;
				} else {
					fShadedFraction = 1.0;
				}			
			}
        } else {
            fShadedFraction = 1.0;
        }
        //printf("fTimeDay %f Azimuth %f %f ShadedFrac %f Shaded Length %f \n", xpn->pTi->pSimTime->fTimeDay, self->fAzimuthAngle, self->fDiffAngle, fShadedFraction, fShadedLength);
        //printf("fTimeDay %f SolRad %f \n", xpn->pTi->pSimTime->fTimeDay, pCl->pWeather->fSolRad);
        if (self->iCalcLightAttenuation == 1)
        {
			fAverageLengthThroughTrees = 0.0;
			if (self->iAverageShading == 1){
				fWidth0 = fZenithTan * self->fTreeHeight;
				if ((fWidth0 <= self->fTreeWidth)&&(fWidth0 > 0.0))
					{
					fAverageLengthThroughTrees = sqrt(1 + fZenithTan * fZenithTan) * 0.5 * self->fTreeHeight;
					}
				else if (fWidth0 > self->fTreeWidth)
					{
					fHeight0 = self->fTreeWidth / fZenithTan;
					fAverageLengthThroughTrees = 
					(self->fTreeHeight - fHeight0) * sqrt(self->fTreeWidth * self->fTreeWidth + fHeight0 * fHeight0)/self->fTreeHeight 
					+ 0.5 * sqrt(1 + fZenithTan * fZenithTan) * fHeight0 * fHeight0 / self->fTreeHeight;
					}
				else
					{
					fAverageLengthThroughTrees = 0.0;
					}
			} else  {
				fHeight0 = (self->fTreeWidth + self->fDistStrip)/ fZenithTan;
				if ((fHeight0 <= self->fTreeHeight)&&(fHeight0 > 0.0))
					{
					fAverageLengthThroughTrees = self->fTreeWidth / sin(self->fZenithAngle*PI/180.);
					}
				else if (fHeight0 > self->fTreeHeight)
					{
					fAverageLengthThroughTrees = (self->fTreeHeight - self->fDistStrip/fZenithTan) / cos(self->fZenithAngle*PI/180.);
					}
				else
					{
					fAverageLengthThroughTrees = 0.0;
					}
				//if (fShadedFraction == 1)
				//	printf("%f \n", fAverageLengthThroughTrees);
			}
        
		if(self->iCalcExtinction == 1) //LAI from other XN grid point is used to calculate the extinction coefficient, else the input value from project__agroforestry.ini
		{
			self->fLAI = xpn_class[self->xn_id_tree]->pPl->pCanopy->fLAI;
		}		
		
		if(self->fTreeHeight <= 0.0)
		{
		fLAIeff = 0.0;
		}
		else
		{
		fLAIeff = self->fLAI * fAverageLengthThroughTrees / self->fTreeHeight;
		}
        
		fAttenuationFactor = exp(- self->fExtinctionCoefficient * fLAIeff );
		//printf("LAI %f Height %f Ext %f LAIeff %f \n",self->fLAI, self->fTreeHeight, self->fExtinctionCoefficient, fLAIeff);		
        } else { //end self->iCalcLightAttenuation == 1
        fAttenuationFactor = self->fFixedLightAttenuationFactor;
        }    
    
    
    
    } else {
        fShadedFraction  = 0.0;
    }

        
        fReductionFactor = fAttenuationFactor * fShadedFraction + 1.0 * (1 - fShadedFraction);
        //printf("Red %f ShadeF %f ShadeL %f \n", fReductionFactor, fShadedFraction, fShadedLength);
        pCl->pWeather->fSolRad *= fReductionFactor;
        pCl->pWeather->fPAR *= fReductionFactor;

	//Added by Hong on 20181127
	/* actualize the Soil Organic Matter Pools over soil depth
		 */

	// Litter from trees to soil surface
	pCP->fCLitterSurf    += fTreeDeadLeafWeight * self->DeadLeaf_beside_tree_frac
	                         +fTreeDeadStemWeight * self->DeadStem_beside_tree_frac 
							 + fTreeDeadBranchWeight * self->DeadBranch_beside_tree_frac;
	
	pCP->fNLitterSurf    += fTreeDeadLeafWeight /fCNTreeDeadLeaf * self->DeadLeaf_beside_tree_frac
	                         +fTreeDeadStemWeight /fCNTreeDeadStem * self->DeadStem_beside_tree_frac
							 + fTreeDeadBranchWeight /fCNTreeDeadBranch * self->DeadBranch_beside_tree_frac;
							 
							 
	//Added by Hong on 20181126
	pCB->dCInputSurf += fTreeDeadLeafWeight * self->DeadLeaf_beside_tree_frac
						+fTreeDeadStemWeight * self->DeadStem_beside_tree_frac  
                         + fTreeDeadBranchWeight * self->DeadBranch_beside_tree_frac;
	
/*	pCB->dCInputCum += fTreeDeadLeafWeight * self->DeadLeaf_beside_tree_frac
						+fTreeDeadStemWeight * self->DeadStem_beside_tree_frac  
                         + fTreeDeadBranchWeight * self->DeadBranch_beside_tree_frac;*/
    //End of Hong						 
  
	// Calculate "used" weights for each grid point and each plant compartment separately
	pCh->pCProfile->fUsedLeafWeight += fTreeDeadLeafWeight * self->DeadLeaf_beside_tree_frac;
	pCh->pCProfile->fUsedStemWeight += fTreeDeadStemWeight * self->DeadStem_beside_tree_frac;
    pCh->pCProfile->fUsedBranchWeight += fTreeDeadBranchWeight * self->DeadBranch_beside_tree_frac;
	
/*	gchar* S;
	S = g_strdup_printf(" pPl->pBiomass->fUsedLeafWeight %f self->DeadLeaf_beside_tree_frac %f \n", pPl->pBiomass->fUsedLeafWeight, self->DeadLeaf_beside_tree_frac);
	PRINT_MESSAGE(xpn,3,S);
 */   
/*if ((xpn_time_compare_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,
		    2019,8,15) ==0 ))
		//if (xpn->pTi->pSimTime->fTimeDay>0.5&&xpn->pTi->pSimTime->fTimeDay<0.6)
 {		
    gchar* S;
	S = g_strdup_printf(" pCh->pCProfile->fUsedLeafWeight: %f, fTreeDeadLeafWeight:  %f, beside_tree_frac: %f \n", pCh->pCProfile->fUsedLeafWeight, fTreeDeadLeafWeight, self->DeadLeaf_beside_tree_frac);
	PRINT_MESSAGE(xpn,3,S);	
	
}
	
if ((xpn_time_compare_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,
		    2019,8,16) ==0 ))
		//if (xpn->pTi->pSimTime->fTimeDay>0.5&&xpn->pTi->pSimTime->fTimeDay<0.6)
 {		
    gchar* S;
	S = g_strdup_printf(" pCh->pCProfile->fUsedLeafWeight: %f, fTreeDeadLeafWeight:  %f, beside_tree_frac: %f \n", pCh->pCProfile->fUsedLeafWeight, fTreeDeadLeafWeight, self->DeadLeaf_beside_tree_frac);

	PRINT_MESSAGE(xpn,3,S);	
	
}	
*/

		/* root litter */
        // FH 20190808 ...and calculation of root water/nitrogen competition
		fCumDepth =0.0;	
		for (pSL = pSo->pSLayer->pNext,pCL = pCh->pCLayer->pNext,pLR = pPl->pRoot->pLayerRoot->pNext, i2=0;
		     ((pSL->pNext != NULL)&&(pCL->pNext != NULL)&&(pLR->pNext != NULL));
		     pSL = pSL->pNext,pCL = pCL->pNext,pLR = pLR->pNext,i2++) {
				 
			 pCL->fCLitter += fTreeDeadFineRootWeight_Layer[i2] * self->DeadFineRoot_beside_tree_frac +fTreeDeadGrossRootWeight_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;
			  
			  
			  pCL->fNLitter += fTreeDeadFineRootWeight_Layer[i2] /fCNTreeDeadFineRoot_Layer[i2]  * self->DeadFineRoot_beside_tree_frac +fTreeDeadGrossRootWeight_Layer[i2] /fCNTreeDeadGrossRoot_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;
					
			/*  pCB->dCInputCum += fTreeDeadFineRootWeight_Layer[i2] * self->DeadFineRoot_beside_tree_frac +fTreeDeadGrossRootWeight_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;*/
			  
			  pCB->dCInputProfile += fTreeDeadFineRootWeight_Layer[i2] * self->DeadFineRoot_beside_tree_frac +fTreeDeadGrossRootWeight_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;
			  //Hong 20190507: balance for 0-30 cm profile:	
	  fCumDepth +=(double)0.1*pSL->fThickness; //cm
	  if (fCumDepth <=30.0)
	    {   
			pCB->dCInputProfile_30 += fTreeDeadFineRootWeight_Layer[i2] * self->DeadFineRoot_beside_tree_frac +fTreeDeadGrossRootWeight_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;
		}
              pCh->pCProfile->fUsedRootWeight +=	fTreeDeadFineRootWeight_Layer[i2] * self->DeadFineRoot_beside_tree_frac + fTreeDeadGrossRootWeight_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;			

             //pLR->fWaterUptakeFacAfTree = self->Root_beside_tree_frac/(1.0 + self->Root_beside_tree_frac);
              //pLR->fWaterUptakeFacAfbesideTree = 1.0 / (1.0 + self->Root_beside_tree_frac);

		}

            
			
	} //end self->iTree (Strip next to trees)

//out-noted by Hong on 20190221, not necessary
/*
	self->UsedLeafWeight_Sum = 0.0;
	self->UsedStemWeight_Sum = 0.0;
	self->UsedRootWeight_Sum = 0.0;
	
  for(i = 0; i < xpn->pXSys->id_N; i++)
		{	
		self->UsedLeafWeight_Sum += xpn_class[i]->pPl->pBiomass->fUsedLeafWeight;
		self->UsedStemWeight_Sum += xpn_class[i]->pPl->pBiomass->fUsedStemWeight;
		self->UsedRootWeight_Sum += xpn_class[i]->pPl->pBiomass->fUsedRootWeight;
		}

	if(self->UsedLeafWeight_Sum > fTreeDeadLeafWeight)
		{
		xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadLeafWeight = 0.0; /
		}

   else 
		{
		xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadLeafWeight -= self->UsedLeafWeight_Sum;
		} 

	if(self->UsedStemWeight_Sum > fTreeDeadStemWeight)
		{			
		xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadStemWeight = 0.0;
		}
	if(self->UsedRootWeight_Sum > fTreeDeadRootWeight)
		{		
		xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadRootWeight = 0.0;
		}
*/
//End of Hong
     //g_free(fTreeDeadFineRootWeight_Layer);
    g_free(fTreeDeadFineRootWeight_Layer);
    g_free(fTreeDeadGrossRootWeight_Layer);
    g_free(fCNTreeDeadFineRoot_Layer);
    g_free(fCNTreeDeadGrossRoot_Layer);

    return RET_SUCCESS;
}

void sunpos(agroforestry* self)
{
    expertn_modul_base* xpn = EXPERTN_MODUL_BASE(self);
    PSIMTIME pST = xpn->pTi->pSimTime;
    PLOCATION pLo = xpn->pLo;

    // Time variables
    int year, month, day, hour, minutes, seconds;

    year = pST->year;
    month = pST->mon;
    day = pST->mday;
    hour = pST->hour;
    minutes = pST->min;
    seconds = pST->sec;

    // Main variables
    double dElapsedJulianDays;
    double dDecimalHours;
    double dEclipticLongitude;
    double dEclipticObliquity;
    double dRightAscension;
    double dDeclination;

    // Auxiliary variables
    double dY;
    double dX;

    // Declaration of some constants
    double pi = 3.14159265358979323846;
    double twopi = (2 * pi);
    double rad = (pi / 180);
    double dEarthMeanRadius = 6371.01;    // In km
    double dAstronomicalUnit = 149597890; // In km

    // Calculate difference in days between the current Julian Day
    // and JD 2451545.0, which is noon 1 January 2000 Universal Time
    {
        double dJulianDate;
        long int liAux1;
        long int liAux2;
        // Calculate time of the day in UT decimal hours
        dDecimalHours = hour + (minutes + seconds / 60.0) / 60.0;
        // Calculate current Julian Day
        liAux1 = (month - 14) / 12;
        liAux2 = (1461 * (year + 4800 + liAux1)) / 4 + (367 * (month - 2 - 12 * liAux1)) / 12 -
                 (3 * ((year + 4900 + liAux1) / 100)) / 4 + day - 32075;
        dJulianDate = (double)(liAux2) - 0.5 + dDecimalHours / 24.0;
        // Calculate difference between current Julian Day and JD 2451545.0
        dElapsedJulianDays = dJulianDate - 2451545.0;
    }

    // Calculate ecliptic coordinates (ecliptic longitude and obliquity of the
    // ecliptic in radians but without limiting the angle to be less than 2*Pi
    // (i.e., the result may be greater than 2*Pi)
    {
        double dMeanLongitude;
        double dMeanAnomaly;
        double dOmega;
        dOmega = 2.1429 - 0.0010394594 * dElapsedJulianDays;
        dMeanLongitude = 4.8950630 + 0.017202791698 * dElapsedJulianDays; // Radians
        dMeanAnomaly = 6.2400600 + 0.0172019699 * dElapsedJulianDays;
        dEclipticLongitude = dMeanLongitude + 0.03341607 * sin(dMeanAnomaly) + 0.00034894 * sin(2 * dMeanAnomaly) -
                             0.0001134 - 0.0000203 * sin(dOmega);
        dEclipticObliquity = 0.4090928 - 6.2140e-9 * dElapsedJulianDays + 0.0000396 * cos(dOmega);
    }

    // Calculate celestial coordinates ( right ascension and declination ) in radians
    // but without limiting the angle to be less than 2*Pi (i.e., the result may be
    // greater than 2*Pi)
    {
        double dSin_EclipticLongitude;
        dSin_EclipticLongitude = sin(dEclipticLongitude);
        dY = cos(dEclipticObliquity) * dSin_EclipticLongitude;
        dX = cos(dEclipticLongitude);
        dRightAscension = atan2(dY, dX);
        if(dRightAscension < 0.0) {
            dRightAscension = dRightAscension + twopi;
        }
        dDeclination = asin(sin(dEclipticObliquity) * dSin_EclipticLongitude);
    }

    // Calculate local coordinates ( azimuth and zenith angle ) in degrees
    {
        double dGreenwichMeanSiderealTime;
        double dLocalMeanSiderealTime;
        double dLatitudeInRadians;
        double dHourAngle;
        double dCos_Latitude;
        double dSin_Latitude;
        double dCos_HourAngle;
        double dParallax;
        dGreenwichMeanSiderealTime = 6.6974243242 + 0.0657098283 * dElapsedJulianDays + dDecimalHours;
        dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime * 15 + pLo->pFarm->fLongitude) * rad;
        dHourAngle = dLocalMeanSiderealTime - dRightAscension;
        dLatitudeInRadians = pLo->pFarm->fLatitude * rad;
        dCos_Latitude = cos(dLatitudeInRadians);
        dSin_Latitude = sin(dLatitudeInRadians);
        dCos_HourAngle = cos(dHourAngle);
        self->fZenithAngle =
            (acos(dCos_Latitude * dCos_HourAngle * cos(dDeclination) + sin(dDeclination) * dSin_Latitude));
        dY = -sin(dHourAngle);
        dX = tan(dDeclination) * dCos_Latitude - dSin_Latitude * dCos_HourAngle;
        self->fAzimuthAngle = atan2(dY, dX);
        if(self->fAzimuthAngle < 0.0) {
            self->fAzimuthAngle = self->fAzimuthAngle + twopi;
        }
        self->fAzimuthAngle = self->fAzimuthAngle / rad;
        // Parallax Correction
        dParallax = (dEarthMeanRadius / dAstronomicalUnit) * sin(self->fZenithAngle);
        self->fZenithAngle = (self->fZenithAngle + dParallax) / rad;
    }
}

 void agroforestry_root_competition(agroforestry* self)
{
    expertn_modul_base* xpn = EXPERTN_MODUL_BASE(self);
    expertnclass** xpn_class=(expertnclass **)xpn->pXSys->xpn_classes;
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    PLAYERROOT      pLR       = pPl->pRoot->pLayerRoot; // Tree root layer
    PLAYERROOT      pLR2    = pPl->pRoot->pLayerRoot; // Crop root layer
    PSPROFILE            pSo      = xpn->pSo;
    PWATER                  pWa    = xpn->pWa;
    PSLAYER 	              pSL	    = pSo->pSLayer;
	PSWATER		      pSWL	= pSo->pSWater;
	PWLAYER	   	      pSLW	= pWa->pWLayer;
    PPLTWATER		pPltW	=pPl->pPltWater;
    int i, i2, L;
    int *iIDs;
    int Lmax;
    double PotTranspTree, PotTranspCrop, PotTranspTotal;
    // TranspFromGP: Tree only transpires from Tree grid point, crop only from crop grid points
    double *PotTranspFromGP, *ActTranspFromGP, *RWUMX, *PotLayWaterUptTreeTotal, *ActLayWaterUptTreeTotal;
    // Total in the following variables mean: from the respective grid point which is in the end important for the water uptake simulation
    double **PotWaterUptTree, **PotLayWaterUptTree, **PotWaterUptCrop, **PotLayWaterUptCrop, **PotWaterUptTotal, **PotLayWaterUptTotal;
    double **ActWaterUptTree, **ActLayWaterUptTree, **ActWaterUptCrop, **ActLayWaterUptCrop, **ActWaterUptTotal, **ActLayWaterUptTotal;
    
    double fThickness, SoilWaterAv, TRWUTree, WUFTree, WUFCrop, fContAct;
    double *TRWUAtGP, *TRWUTreeAtGP, *TRWUCropAtGP;
    
    //do the calculation just once per time step -> set pTi->pSimTime->fCompTimeY for all relevant grid points to the same time at each time step
    if (pTi->pSimTime->fCompTimeY != pTi->pSimTime->fTimeY)
        {
         iIDs = (int *)g_malloc0(sizeof(int)*(self->grid_id_beside_tree_len+1));   
         PotTranspFromGP = (double *)g_malloc0(sizeof(double)*(self->grid_id_beside_tree_len+1));   
         ActTranspFromGP = (double *)g_malloc0(sizeof(double)*(self->grid_id_beside_tree_len+1));   
         TRWUAtGP = (double *)g_malloc0(sizeof(double)*(self->grid_id_beside_tree_len+1));   
         TRWUTreeAtGP = (double *)g_malloc0(sizeof(double)*(self->grid_id_beside_tree_len+1));   
         TRWUCropAtGP = (double *)g_malloc0(sizeof(double)*(self->grid_id_beside_tree_len+1));   
         RWUMX = (double *)g_malloc0(sizeof(double)*(self->grid_id_beside_tree_len+1));   
         
         iIDs[0] = self->xn_id_tree;
        PotTranspFromGP[0]  = xpn_class[self->xn_id_tree]->pPl->pPltWater->fPotTranspR;
        
        Lmax = pSo->iLayers-2;
        
        for (pLR = xpn_class[self->xn_id_tree]->pPl->pRoot->pLayerRoot->pNext, L=1;(pLR->pNext != NULL);pLR = pLR->pNext,L++)
            {
            self->fTreeRootsAtGridPoints[0][L] =  pLR->fLengthDens * self->fTreeRootFracsAtGridPoints[0];
            self->fCropRootsAtGridPoints[0][L] = 0.0;
            self->fTotalRootsAtGridPoints[0][L] = self->fTreeRootsAtGridPoints[0][L] + self->fCropRootsAtGridPoints[0][L];
            //printf("%d RLD AF %f \n", L,  self->fTotalRootsAtGridPoints[0][L]);
            //Lmax = L;
            //printf("id tree %d layer %d LengthDens %f RootTreefrac %f TreeRootsAtGP %f CropRootsAtGP %f \n", self->xn_id_tree, L, pLR->fLengthDens, self->fTreeRootFracsAtGridPoints[0], self->fTreeRootsAtGridPoints[0][j], self->fCropRootsAtGridPoints[0][j]);
            }
        xpn_class[self->xn_id_tree]->pTi->pSimTime->fCompTimeY = pTi->pSimTime->fTimeY;
    
        for(i = 0; i < self->grid_id_beside_tree_len; i++)
                {
                iIDs[i+1] = self->xn_ids_beside_tree[i];   
                PotTranspFromGP[i+1] = xpn_class[self->xn_ids_beside_tree[i]]->pPl->pPltWater->fPotTranspR;
                //printf("AF %s %f \n", xpn_class[self->xn_ids_beside_tree[i]]->pXSys->reg_str, xpn_class[self->xn_ids_beside_tree[i]]->pPl->pPltWater->fPotTranspR);
                for (pLR = xpn_class[self->xn_id_tree]->pPl->pRoot->pLayerRoot->pNext, pLR2 = xpn_class[self->xn_ids_beside_tree[i]]->pPl->pRoot->pLayerRoot, L=1;
                        ((pLR->pNext != NULL)&&(pLR2->pNext != NULL)); pLR = pLR->pNext,pLR2 = pLR2->pNext,L++)
                    {
                    self->fTreeRootsAtGridPoints[i+1][L] =  pLR->fLengthDens * self->fTreeRootFracsAtGridPoints[i+1];
                    self->fCropRootsAtGridPoints[i+1][L] = pLR2->fLengthDens;
                    self->fTotalRootsAtGridPoints[i+1][L] = self->fTreeRootsAtGridPoints[i+1][L] + self->fCropRootsAtGridPoints[i+1][L];
                    // get the maximum amount of soil layers
/*                    if (L > Lmax)
                        {
                        Lmax = L;
                        }*/
                   //printf("id beside tree %d layer %d LengthDens %f LengthDens2 %f RootTreefrac %f TreeRootsAtGP %f CropRootsAtGP %f \n", self->xn_ids_beside_tree[i], L, pLR->fLengthDens , pLR2->fLengthDens, self->fTreeRootFracsAtGridPoints[i+1], self->fTreeRootsAtGridPoints[i+1][L], self->fCropRootsAtGridPoints[i+1][L]);
                   }
                xpn_class[self->xn_ids_beside_tree[i]]->pTi->pSimTime->fCompTimeY = pTi->pSimTime->fTimeY;
                //printf("%f id_beside %d i %d PotTransp %f\n", xpn->pTi->pSimTime->fTimeY, self->xn_ids_beside_tree[i], i, PotTranspFromGP[i+1]);
                }
        //printf("%f id_tree %d PotTransp %f\n", xpn->pTi->pSimTime->fTimeY,self->xn_id_tree, PotTranspFromGP[0]);
        
        PotLayWaterUptTreeTotal = (double*)g_malloc0(Lmax*sizeof(double));
        ActLayWaterUptTreeTotal = (double*)g_malloc0(Lmax*sizeof(double));
        TRWUTree = 0.0;
        


        // We declare the variables separately for the grids and the different platns
        PotWaterUptTree = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        PotLayWaterUptTree = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        PotWaterUptCrop = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        PotLayWaterUptCrop = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        PotWaterUptTotal = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        PotLayWaterUptTotal = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        ActWaterUptTree = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        ActLayWaterUptTree = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        ActWaterUptCrop = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        ActLayWaterUptCrop = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        ActWaterUptTotal = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        ActLayWaterUptTotal = (double**)g_malloc((self->grid_id_beside_tree_len + 1)*sizeof(double*));
        for(i = 0; i < self->grid_id_beside_tree_len + 1; i++)
            {
           PotWaterUptTree[i] = (double*)g_malloc0(Lmax*sizeof(double));
           PotLayWaterUptTree[i] = (double*)g_malloc0(Lmax*sizeof(double));
           PotWaterUptCrop[i] = (double*)g_malloc0(Lmax*sizeof(double));
           PotLayWaterUptCrop[i] = (double*)g_malloc0(Lmax*sizeof(double));
           PotWaterUptTotal[i] = (double*)g_malloc0(Lmax*sizeof(double));
           PotLayWaterUptTotal[i] = (double*)g_malloc0(Lmax*sizeof(double));
           ActWaterUptTree[i] = (double*)g_malloc0(Lmax*sizeof(double));
           ActLayWaterUptTree[i] = (double*)g_malloc0(Lmax*sizeof(double));
           ActWaterUptCrop[i] = (double*)g_malloc0(Lmax*sizeof(double));
           ActLayWaterUptCrop[i] = (double*)g_malloc0(Lmax*sizeof(double));
           ActWaterUptTotal[i] = (double*)g_malloc0(Lmax*sizeof(double));
           ActLayWaterUptTotal[i] = (double*)g_malloc0(Lmax*sizeof(double));
            }
        
        
        
         // from here we loop over the tree grid point [0] and the beside tree grid points [1]...,
        //  therefore new variable iIDs to rarely use [i+1] in the loop
        // pLR always describes the tree root layers, pLR2 the crop root layers       
        
        for(i = 0; i <= self->grid_id_beside_tree_len; i++)
            {
            //printf("id_beside %d i %d TreeDistr %d CropRoots %f\n", iIDs[i], i, self->iRootDistributionBesideTreeAtGridPoints[i],self->fTreeRootFracsAtGridPoints[i] );
                
            pPl = xpn_class[iIDs[i]]->pPl;
            pSo = xpn_class[iIDs[i]]->pSo;
            pWa = xpn_class[iIDs[i]]->pWa;
			pSL  = pSo->pSLayer->pNext;
			pSWL = pSo->pSWater->pNext;
			pSLW = pWa->pWLayer->pNext;
            // 20191206 in Treemix we start with pNext, but not in the other models. :-(
            if (i == 0)
                pLR	 = pPl->pRoot->pLayerRoot->pNext;
            else
                pLR	 = pPl->pRoot->pLayerRoot;
            
            pPl->pDevelop->iAFTrue = 1;
            
            // we can do it like this, because we'll always have the tree grid point first (i=0)
            // SPASS has a variable RWUMX depending on the phenological development, should adress that later
            RWUMX[i] = xpn_class[iIDs[i]]->pPl->pGenotype->fMaxWuptRate;
            // no fMaxWuptRate in HPM, address that later
            if (RWUMX[i] == 0.0)
                {
                RWUMX[i] = RWUMX[0];
                }
            
            TRWUAtGP[i] = 0.0;
            TRWUTreeAtGP[i] = 0.0;
            TRWUCropAtGP[i] = 0.0;
            L = 1;
            while ( (self->fTotalRootsAtGridPoints[i][L] != (double) 0.0) && (L < Lmax))
                {
                    
                 fThickness = (double)0.1*pSL->fThickness; //mm to cm   
                    
                SoilWaterAv = max(0.0, (pSLW->fContAct - pSWL->fContPWP));    
                
                //printf("%f \n", SoilWaterAv);
                
                // water uptake per unit root length, I suppose
                if ((pSLW->fContAct<=pSWL->fContPWP) ||  (self->fTotalRootsAtGridPoints[i][L] <= 0.0))
                    {
                    PotWaterUptTree[i][L]=(double)0.0;
                    PotWaterUptCrop[i][L]=(double)0.0;
                    PotWaterUptTotal[i][L]=(double)0.0;
                    }
                else
                    {
/*                    if (self->fTreeRootsAtGridPoints[i][L] > 0.0)
                        {
                        PotWaterUptTree[L] = (double)(2.67*1E-3*exp(min(62.0*(double)SoilWaterAv,10.0))
            								/(6.68-log((double) self->fTreeRootsAtGridPoints[i][L])));
                        PotWaterUptTree[L]=min(RWUMX[0],PotWaterUptTree[L]);
                        CHECK_VALID(PotWaterUptTree[L])
                        }
                    else //no uptake if no roots
                        {
                        PotWaterUptTree[L] = 0.0;
                        }
                        
                    if (self->fCropRootsAtGridPoints[i][L] > 0.0)
                        {
                        PotWaterUptCrop[L] = (double)(2.67*1E-3*exp(min(62.0*(double)SoilWaterAv,10.0))
            								/(6.68-log((double) self->fCropRootsAtGridPoints[i][L])));
                        PotWaterUptCrop[L]=min(RWUMX[i],PotWaterUptCrop[L]);
                        CHECK_VALID(PotWaterUptTree[L])
                        }
                    else //no uptake if no roots
                        {
                        PotWaterUptCrop[L] = 0.0;
                        }*/
                        
                    //if (self->fTotalRootsAtGridPoints[i][L] > 0.0){
                        //PotWaterUptTotal[i][L] = (double)(2.67*1E-3*exp(min(35.0*(double)SoilWaterAv,10.0))
            			//					/(6.68-log((double) self->fTotalRootsAtGridPoints[i][L])));
                        PotWaterUptTotal[i][L] = (double)(2.67*1E-3*exp(min(62.0*(double)SoilWaterAv,10.0))
            								/(6.68-log((double) self->fTotalRootsAtGridPoints[i][L])));
                        if (self->fTreeRootsAtGridPoints[i][L] > 0.0)
                            {
                            PotWaterUptTree[i][L] = min(RWUMX[0],PotWaterUptTotal[i][L]);   
                            }
                            else
                            {
                            PotWaterUptTree[i][L] = 0.0;
                            }
                        if (self->fCropRootsAtGridPoints[i][L] > 0.0)
                            {
                            PotWaterUptCrop[i][L] = min(RWUMX[i],PotWaterUptTotal[i][L]);   
                            }
                            else
                            {
                            PotWaterUptCrop[i][L] = 0.0;
                            }

                        CHECK_VALID(PotWaterUptTree[i][L])
                        CHECK_VALID(PotWaterUptCrop[i][L])
                        CHECK_VALID(PotWaterUptTotal[i][L])
                        //}
                    }
                
                // these equations are slightly different than in FAM-Bericht 60, I guess,
                // leaveing out line 2 and 3 makes the relation linear, do we want that?
                PotLayWaterUptTree[i][L] = PotWaterUptTree[i][L]*fThickness* self->fTreeRootsAtGridPoints[i][L]*(double)10.0;
								//*((double)0.18+(double)0.00272*( self->fTreeRootsAtGridPoints[i][L] - (double)18.0)
			 					//*( self->fTreeRootsAtGridPoints[i][L] - (double)18.0));
                PotLayWaterUptTreeTotal[L] += PotLayWaterUptTree[i][L];
                
                if (self->iRootDistributionBesideTreeAtGridPoints[i] == 1)
                    {//SPASS and CERES
                    PotLayWaterUptCrop[i][L] = PotWaterUptCrop[i][L]*fThickness* self->fCropRootsAtGridPoints[i][L]*(double)10.0
								*((double)0.18+(double)0.00272*( self->fCropRootsAtGridPoints[i][L] - (double)18.0)
			 					*( self->fCropRootsAtGridPoints[i][L] - (double)18.0)); 
                    if ((xpn_class[iIDs[i]]->pPl->pDevelop->fCumPhyll<(double)4.0)&&(xpn_class[iIDs[i]]->pPl->pCanopy->fLAI<(double)1.0))
                            {
                             PotLayWaterUptCrop[i][L]  *= ((double)3.0-(double)2.0*xpn_class[iIDs[i]]->pPl->pCanopy->fLAI);
                            }
                    }
                else
                    {//other
                    PotLayWaterUptCrop[i][L] = PotWaterUptCrop[i][L]*fThickness* self->fCropRootsAtGridPoints[i][L]*(double)10.0;
								//*((double)0.18+(double)0.00272*( self->fCropRootsAtGridPoints[i][L] - (double)18.0)
			 					//*( self->fCropRootsAtGridPoints[i][L] - (double)18.0)); 
                    }            

                PotLayWaterUptTotal[i][L] = PotLayWaterUptTree[i][L] + PotLayWaterUptCrop[i][L];
                

                
                //printf("%d %d %d %f %f %f %f %f %f %f %f %f %f \n", i, self->iRootDistributionBesideTreeAtGridPoints[i], L,  self->fTreeRootsAtGridPoints[i][L],  self->fCropRootsAtGridPoints[i][L],  self->fTotalRootsAtGridPoints[i][L], PotWaterUptTree[L], PotWaterUptCrop[L],PotWaterUptTotal[L],PotLayWaterUptTree[L], PotLayWaterUptCrop[L],  PotLayWaterUptTotal[L], PotLayWaterUptTree[L] + PotLayWaterUptCrop[L] );
                
                TRWUAtGP[i] +=  PotLayWaterUptTotal[i][L];
                TRWUTreeAtGP[i] += PotLayWaterUptTree[i][L];
                TRWUCropAtGP[i] += PotLayWaterUptCrop[i][L];
                
                //write variables to XN structure
                pLR->fPotWatUptAF = PotWaterUptTotal[i][L];
                pLR->fPotLayWatUptAF = PotLayWaterUptTotal[i][L];
                //if (i != 0)
                 //   printf("AF %f \n",PotLayWaterUptTotal[i][L]);
                
                pSL =pSL ->pNext;
                pSWL=pSWL->pNext;
                pSLW=pSLW->pNext;
                pLR = pLR->pNext;
                L += 1;
                }
        
                // summing up all tree TRWU
                TRWUTree += TRWUTreeAtGP[i];
                pPl->pPltWater->fPotUptakeAF = TRWUCropAtGP[i];
                
               //printf("%d %f \n", i, TRWU);
                
            } //end looping over the grid points
        
            
            // We use the TRWUTree outside the loop and then reuse the variable
            if (PotTranspFromGP[0] < TRWUTree)
                {
                WUFTree = PotTranspFromGP[0]/TRWUTree;
                }
            else
                {
                WUFTree = (double)1.0;
                }   
            TRWUTree = 0.0;    
            
            // Here we have to start a new loop			
	xpn_class[self->xn_id_tree]->pPl->pPltWater->fWaterGainAF = 0.0;//added by Hong on 20200220 for water balance
        for(i = 0; i <= self->grid_id_beside_tree_len; i++)
            {
                
            pPl = xpn_class[iIDs[i]]->pPl;
            pSo = xpn_class[iIDs[i]]->pSo;
            pWa = xpn_class[iIDs[i]]->pWa;
			pSL  = pSo->pSLayer->pNext;
			pSWL = pSo->pSWater->pNext;
			pSLW = pWa->pWLayer->pNext;
            if (i == 0)
                pLR	= pPl->pRoot->pLayerRoot->pNext;
            else
                pLR =  pPl->pRoot->pLayerRoot;
            
            if (PotTranspFromGP[i] < TRWUCropAtGP[i])
                {
                WUFCrop =PotTranspFromGP[i]/TRWUCropAtGP[i];
                }
            else
                {
                WUFCrop = (double)1.0;    
                }
                
           TRWUCropAtGP[i] = 0.0;
		   
           pPl->pPltWater->fWaterLossAF = 0.0; //added by Hong on 20200220 for water balance
           for (L=1;L<=Lmax;L++)
                {
                ActWaterUptTree[i][L]	=  PotWaterUptTree[i][L]   * WUFTree;
                ActLayWaterUptTree[i][L]	=  PotLayWaterUptTree[i][L]   * WUFTree;
                ActWaterUptCrop[i][L]	=  PotWaterUptCrop[i][L]   * WUFCrop;
                ActLayWaterUptCrop[i][L]	=  PotLayWaterUptCrop[i][L]   * WUFCrop;
                ActWaterUptTotal[i][L]	=  ActWaterUptTree[i][L] + ActWaterUptCrop[i][L];
                ActLayWaterUptTotal[i][L]	=  ActLayWaterUptTree[i][L] + ActLayWaterUptCrop[i][L];
               
                //if (i == 0)
                    //printf("AF 0 %d %d %f %f\n", i, L, ActLayWaterUptTotal[i][L], WUFTree);     
                    
                fContAct  = pSLW->fContAct;
                fContAct -= (ActLayWaterUptTotal[i][L]*pTi->pTimeStep->fAct/pSL->fThickness); //mm/mm

                if (fContAct<pSWL->fContPWP)
                    {
                    ActWaterUptTotal[i][L]	      *= (fContAct/pSWL->fContPWP);
                    ActLayWaterUptTotal[i][L] *= (fContAct/pSWL->fContPWP);
                    ActLayWaterUptTree[i][L] *=  (fContAct/pSWL->fContPWP);
                    ActLayWaterUptCrop[i][L] *= (fContAct/pSWL->fContPWP);
//			    pSLW->fContAct=pSWL->fContPWP;
                    }     
                    
                CHECK_VALID(ActLayWaterUptTotal[i][L])
                CHECK_VALID(ActLayWaterUptTree[i][L])
                CHECK_VALID(ActLayWaterUptCrop[i][L])
                    
                TRWUTree += ActLayWaterUptTree[i][L];
                TRWUCropAtGP[i] += ActLayWaterUptCrop[i][L];
                    
                //write variables to XN structure
                pLR->fActWatUptAF = ActWaterUptTotal[i][L];
                pLR->fActLayWatUptAF = ActLayWaterUptTotal[i][L];  
                
				//added by Hong on 20200219	for water balance
                if (i!=0)			
				{
					xpn_class[self->xn_id_tree]->pPl->pPltWater->fWaterGainAF += ActLayWaterUptTree[i][L];
					pPl->pPltWater->fWaterLossAF += ActLayWaterUptTree[i][L];
				   }
                    
                pSL =pSL ->pNext;
                pSWL=pSWL->pNext;
                pSLW=pSLW->pNext;
                pLR =pLR ->pNext;
                    
                    
                }//end looping over soil layers
            }// end looping over the grid points for actual uptake

            //start another loop for the final transpiration calculation
        for(i = 0; i <= self->grid_id_beside_tree_len; i++)
            { 

            pPl = xpn_class[iIDs[i]]->pPl;
            pPltW = pPl->pPltWater;    
       
            if (i == 0)
                {
                if (PotTranspFromGP[0] >= TRWUTree)
                    {
                    ActTranspFromGP[0] = TRWUTree;
                    }
                    else
                    {
                    ActTranspFromGP[0] = PotTranspFromGP[0];
                    }
                }
            else
                {
                if (PotTranspFromGP[i] >= TRWUCropAtGP[i])
                    {
                    ActTranspFromGP[i] = TRWUCropAtGP[i];
                    }
                else
                    {
                    ActTranspFromGP[i] = PotTranspFromGP[i];
                    }
                }
            
            //write variables to XN structure
            pPltW->fPotTranspAF = PotTranspFromGP[i];
            pPltW->fActTranspAF = ActTranspFromGP[i];    
            //printf("%d %f  %f %f %f\n", i, pPltW->fActTranspAF, pPltW->fPotTranspR , TRWUTree, TRWUCropAtGP[i]);

            }// end looping over the grid points for actual uptake


        for(i=0; i < self->grid_id_beside_tree_len + 1; i++)
                {
                g_free(PotWaterUptTree[i]);
                g_free(PotLayWaterUptTree[i]);
                g_free(PotWaterUptCrop[i]);
                g_free(PotLayWaterUptCrop[i]);
                g_free(PotWaterUptTotal[i]);
                g_free(PotLayWaterUptTotal[i]);
                g_free(ActWaterUptTree[i]);
                g_free(ActLayWaterUptTree[i]);
                g_free(ActWaterUptCrop[i]);
                g_free(ActLayWaterUptCrop[i]);
                g_free(ActWaterUptTotal[i]);
                g_free(ActLayWaterUptTotal[i]);
                }
        g_free(PotWaterUptTree);
        g_free(PotLayWaterUptTree);
        g_free(PotWaterUptCrop);
        g_free(PotLayWaterUptCrop);
        g_free(PotWaterUptTotal);
        g_free(PotLayWaterUptTotal);
        g_free(ActWaterUptTree);
        g_free(ActLayWaterUptTree);
        g_free(ActWaterUptCrop);
        g_free(ActLayWaterUptCrop);
        g_free(ActWaterUptTotal);
        g_free(ActLayWaterUptTotal);
        
        
        g_free(PotLayWaterUptTreeTotal);
        g_free(ActLayWaterUptTreeTotal);
        g_free(PotTranspFromGP);
        g_free(ActTranspFromGP);
        g_free(TRWUAtGP);
        g_free(TRWUTreeAtGP);
        g_free(TRWUCropAtGP);
        g_free(iIDs);
        g_free(RWUMX);
        }// ending to do the calculation only once per time step, no matter which grid point is simulated first
    
/*    if(self->iTree == 1) //Tree strip
	{
    // root length densities at grid point 
        for (pLR = pPl->pRoot->pLayerRoot->pNext, i2=0;(pLR->pNext != NULL);pLR = pLR->pNext,i2++) 
                 {
                self->TreeRootsAtGridPoint[i2] = pLR->fLengthDens * self->Root_tree_frac ;
                self->CropRootsAtGridPoint[i2] = 0.0;
                self->TotalRootsAtGridPoint[i2] = self->TreeRootsAtGridPoint[i2] + self->CropRootsAtGridPoint[i2];
                //printf("Tree %d %f %f %f %f \n",i2, self->TreeRootsAtGridPoint[i2],   self->CropRootsAtGridPoint[i2], self->Root_tree_frac, pLR->fLengthDens);
                 }
         PotTranspTree = pPl->pPltWater->fPotTranspR;
         PotTranspCrops = (double *)g_malloc0(sizeof(double)*self->grid_id_beside_tree_len);
         for(i = 0; i < self->grid_id_beside_tree_len; i++)
            {
            PotTranspCrops[i] = xpn_class[self->xn_ids_beside_tree[i]]->pPl->pPltWater->fPotTranspR;
            //printf("id_beside %d i %d PotTransp %f\n", self->xn_ids_beside_tree[i], i, PotTranspCrops[i]);
            }
    }
    else // Strip next to trees
    {
    // root length densities at grid point 
        for (pLR = xpn_class[self->xn_id_tree]->pPl->pRoot->pLayerRoot->pNext, i2=0;(pLR->pNext != NULL);pLR = pLR->pNext,i2++) 
                 {
                self->TreeRootsAtGridPoint[i2] = pLR->fLengthDens * self->Root_beside_tree_frac ;
                //printf("Tree beside %d %f %f %f\n",i2, self->TreeRootsAtGridPoint[i2],   self->Root_beside_tree_frac, pLR->fLengthDens);
                 }
        for (pLR = pPl->pRoot->pLayerRoot->pNext, i2=0;(pLR->pNext != NULL);pLR = pLR->pNext,i2++) 
                 {
                self->CropRootsAtGridPoint[i2] = pLR->fLengthDens;
                self->TotalRootsAtGridPoint[i2] = self->TreeRootsAtGridPoint[i2] + self->CropRootsAtGridPoint[i2];
                //printf("Crop %d %f %f\n",i2, self->CropRootsAtGridPoint[i2], pLR->fLengthDens);
                 }
        //printf("crop id %d PotTransp %f \n", xpn->pXSys->id  ,xpn->pPl->pPltWater->fPotTranspR);
    }*/
    
    
    // water demand for each grid point and layer
    
    // water availability for each grid point and layer
    
    // comparison to distribute the water between tree and other species        
    
    

        
 }