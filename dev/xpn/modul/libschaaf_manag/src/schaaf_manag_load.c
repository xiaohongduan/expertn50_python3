
#include "schaaf_manag_load.h"
#include "ini_macros.h"

int schaaf_manag_load_ini(schaaf_manag *self)

{
	expertn_modul_base *xpn = &(self->parent);
		
	char *ini_filename;
	
	// Wenn die Init bereits geladen ist
	if (self->__LOAD_DONE==0) {
		self->__LOAD_DONE=1;
	} else {
		return RET_SUCCESS;
	}
	
	
	if (self->ini_filename==NULL)
		{
			// Read from INI File:
			ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.schaaf_manag.ini_filename");

			if (ini_filename==NULL)
				{
					PRINT_ERROR("Missing entry 'schaaf_manag.ini_filename' in your configuration!");
				}
			else
				{
					char *S2;
					S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
					if (S2!=NULL)
						{
							ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
							self->ini_filename = g_strdup_printf("%s",ini_filename);
							schaaf_tillage_load_config(self);
							schaaf_irrigation_load_config(self);
							PRINT_MESSAGE(xpn,5,"project_schaaf_manag.ini geladen!")
							free(ini_filename);
							free(S2);
						}

				}

		}

	return RET_SUCCESS;
}


int schaaf_tillage_load_config(schaaf_manag *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	//tillage
	PTILLAGE tillage_first, tillage, tillage_act;
		
	GError *error = NULL;
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	char *filename;
	int i;

	filename=g_strdup_printf("%s",self->ini_filename);

	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, filename, flags, &error))
		{
			PRINT_ERROR("Error Read schaaf_manag INI File");
			return -1;
		}
//---------------------BEGINN OF READ TILLAGE-----------------------------------------------------
//Hong
//[tillage]
// get input
    GET_INI_STRING_ARRAY_OPTIONAL(self->tillage_date, self->tillage_date_len, 1, "9999-09-09", "tillage", "date");
    GET_INI_STRING_ARRAY_OPTIONAL(self->tillage_implement, self->tillage_implement_len, 1, "no implement", "tillage", "implement");
    GET_INI_STRING_ARRAY_OPTIONAL(self->tillage_code, self->tillage_code_len, 1, "no implement", "tillage", "code");
    GET_INI_DOUBLE_ARRAY_OPTIONAL(self->tillage_depth, self->tillage_depth_len, 1, 0.0, "tillage", "depth");

// convert string to gdate format
    self->tillage_gdate = g_malloc0(sizeof(GDate*) * self->tillage_date_len);
    for (i = 0; i < self->tillage_date_len; i++) 
       {
        deleteSpaceBegEnd(self->tillage_date[i]);
        self->tillage_gdate[i] = convert_str_to_gdate(self->tillage_date[i]);
       }

// CHECK_LEN
	 CHECK_LEN(self->tillage_date_len, self->tillage_implement_len);
	 CHECK_LEN(self->tillage_date_len, self->tillage_code_len);
	 CHECK_LEN(self->tillage_date_len, self->tillage_depth_len);
	 
// in die XPN struktur schreiben
	 for (i = 0; i < self->tillage_date_len; i++) 
		 {
                tillage = g_malloc0_n(1, sizeof(STTILLAGE));
                tillage->Day = g_date_get_day(self->tillage_gdate[i]);
                tillage->Month = g_date_get_month(self->tillage_gdate[i]);
                tillage->Year = g_date_get_year(self->tillage_gdate[i]);
				tillage->acName = self->tillage_implement[i];
				deleteSpaceBegEnd(self->tillage_code[i]); //Added by Hong on 20191001
				tillage->acCode = self->tillage_code[i];
                tillage->fDepth = self->tillage_depth[i]*(double)10.0; //[cm] -> [mm]

                //relocated by Hong in Okt. 2019, InitOrgDuenger should by done during reading the input. 
				InitBBGeraet(tillage);
               
				if (i == 0) {
                    tillage_first = tillage;
                    tillage_act = tillage;
                } else {
                    tillage_act->pNext = tillage;
                    tillage->pBack = tillage_act;
                    tillage_act = tillage_act->pNext;
                }
		 }
			
	 if (self->tillage_date_len != 0) // Hong: why?
			{
                tillage = g_malloc0_n(1, sizeof(STTILLAGE));
                tillage->Day = 0;
                tillage->Month = 0;
                tillage->Year = 0;
                tillage_act->pNext = tillage;
                tillage->pBack = tillage_act;
                tillage_act = tillage_act->pNext;
            }
			
            pMa->pTillage = tillage_first;
			self->tillage_done = 0;
			
/////////////--------END OF READ TILLAGE--------------------			
			
//---------------------BEGINN OF READ IRRIGATION-----------------------------------------------------
//Hong
//[irrigation]
// get input
    GET_INI_STRING_ARRAY_OPTIONAL(self->tillage_date, self->tillage_date_len, 1, "9999-09-09", "tillage", "date");
    GET_INI_STRING_ARRAY_OPTIONAL(self->tillage_implement, self->tillage_implement_len, 1, "no implement", "tillage", "implement");
    GET_INI_STRING_ARRAY_OPTIONAL(self->tillage_code, self->tillage_code_len, 1, "no implement", "tillage", "code");
    GET_INI_DOUBLE_ARRAY_OPTIONAL(self->tillage_depth, self->tillage_depth_len, 1, 0.0, "tillage", "depth");			

/////////////--------END OF READ IRRIGATION--------------------			

	 	 
// Free the variables	 
	 g_free(self->tillage_date);
	 g_free(self->tillage_implement);
	 g_free(self->tillage_code);
	 g_free(self->tillage_depth);	
	 g_free(self->tillage_gdate);
//	 g_date_free(self->tillage_gdate);
            
 
// Auf das neueste Datum stellen:
/*
        {
            PTILLAGE tillage = xpn->pMa->pTillage;

            if ((tillage != NULL) && (tillage->Day > 0)) {
                while ((tillage == NULL) || (xpn_time_compare_date(xpn->pTi->pSimTime->year,
                                         xpn->pTi->pSimTime->mon,
                                         xpn->pTi->pSimTime->mday,
                                         tillage->Year,
                                         tillage->Month,
                                         tillage->Day) > 0)) {
                    xpn->pMa->pTillage = xpn->pMa->pTillage->pNext;
                    tillage = xpn->pMa->pTillage;
                }
            }
        }
*/			
						
	if (error != NULL)
		g_error_free(error);

	g_key_file_free(keyfile);
	g_free(filename);

	return RET_SUCCESS;
}


// INI IRRIGATION

int schaaf_irrigation_load_ini(schaaf_manag *self)

{
	expertn_modul_base *xpn = &(self->parent);
		
	char *ini_filename;
	
	// Wenn die Init bereits geladen ist
	if (self->__LOAD_DONE==0) {
		self->__LOAD_DONE=1;
	} else {
		return RET_SUCCESS;
	}
	
	
	if (self->ini_filename==NULL)
		{
			// Read from INI File:
			ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.schaaf_manag.ini_filename");

			if (ini_filename==NULL)
				{
					PRINT_ERROR("Missing entry 'schaaf_manag.ini_filename' in your configuration!");
				}
			else
				{
					char *S2;
					S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
					if (S2!=NULL)
						{
							ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
							self->ini_filename = g_strdup_printf("%s",ini_filename);
							schaaf_irrigation_load_config(self);
							PRINT_MESSAGE(xpn,3,"INI schaaf irrigation!")
							free(ini_filename);
							free(S2);
						}

				}

		}

	return RET_SUCCESS;
}


int schaaf_irrigation_load_config(schaaf_manag *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	
	//irrigation
	PMIRRIGATION irrigation_first, irrigation, irrigation_act;
		
	GError *error = NULL;
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	char *filename;
	int i;

	filename=g_strdup_printf("%s",self->ini_filename);

	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, filename, flags, &error))
		{
			PRINT_ERROR("Error Read schaaf_manag INI File");
			return -1;
		}
//---------------------BEGINN OF READ IRRIGATION-----------------------------------------------------
//Hong
//[irrigation]
// get input
    GET_INI_STRING_ARRAY_OPTIONAL(self->irrigation_date, self->irrigation_date_len, 1, "9999-09-09", "irrigation", "date");
    GET_INI_STRING_ARRAY_OPTIONAL(self->irrigation_type, self->irrigation_type_len, 1, "no irrigation", "irrigation", "type");
    GET_INI_STRING_ARRAY_OPTIONAL(self->irrigation_code, self->irrigation_code_len, 1, "no irrigation", "irrigation", "code");
    GET_INI_DOUBLE_ARRAY_OPTIONAL(self->irrigation_amount, self->irrigation_amount_len, 1, 0.0, "irrigation", "amount");
    GET_INI_DOUBLE_ARRAY_OPTIONAL(self->irrigation_rate, self->irrigation_rate_len, 1, 0.0, "irrigation", "rate");
    GET_INI_DOUBLE_ARRAY_OPTIONAL(self->irrigation_no3n, self->irrigation_no3n_len, 1, 0.0, "irrigation", "no3n"); 
    GET_INI_DOUBLE_ARRAY_OPTIONAL(self->irrigation_nh4n, self->irrigation_nh4n_len, 1, 0.0, "irrigation", "nh4n"); 
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->irrigation_org_n, self->irrigation_org_n_len, 1, 0.0, "irrigation", "org_n");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->irrigation_org_c, self->irrigation_org_c_len, 1, 0.0, "irrigation", "org_c");
	
// convert string to gdate format
    self->irrigation_gdate = g_malloc0(sizeof(GDate*) * self->irrigation_date_len);
    for (i = 0; i < self->irrigation_date_len; i++) 
       {
        deleteSpaceBegEnd(self->irrigation_date[i]);
        self->irrigation_gdate[i] = convert_str_to_gdate(self->irrigation_date[i]);
       }

// CHECK_LEN
	 CHECK_LEN(self->irrigation_date_len, self->irrigation_type_len);
	 CHECK_LEN(self->irrigation_date_len, self->irrigation_code_len);
	 CHECK_LEN(self->irrigation_date_len, self->irrigation_amount_len);
	 CHECK_LEN(self->irrigation_date_len, self->irrigation_rate_len);
	 CHECK_LEN(self->irrigation_date_len, self->irrigation_no3n_len);
	 CHECK_LEN(self->irrigation_date_len, self->irrigation_nh4n_len);
	 CHECK_LEN(self->irrigation_date_len, self->irrigation_org_n_len);
	 CHECK_LEN(self->irrigation_date_len, self->irrigation_org_c_len);
	 
// in die XPN struktur schreiben
	 for (i = 0; i < self->irrigation_date_len; i++) 
		 {
                irrigation = g_malloc0_n(1, sizeof(STIRRIGATION));
                irrigation->Day = g_date_get_day(self->irrigation_gdate[i]);
                irrigation->Month = g_date_get_month(self->irrigation_gdate[i]);
                irrigation->Year = g_date_get_year(self->irrigation_gdate[i]);
				irrigation->acName = self->irrigation_type[i];
				deleteSpaceBegEnd(self->irrigation_code[i]); //Added by Hong on 20191001
				irrigation->acCode = self->irrigation_code[i];
                irrigation->fAmount = self->irrigation_amount[i];
				irrigation->fRate = self->irrigation_rate[i];
				irrigation->fNO3N = self->irrigation_no3n[i];
				irrigation->fNH4N = self->irrigation_nh4n[i];
				irrigation->fNorgManure = self->irrigation_org_n[i];
				irrigation->fCorgManure = self->irrigation_org_c[i];
				
				
				
                if (i == 0) {
                    irrigation_first = irrigation;
                    irrigation_act = irrigation;
                } else {
                    irrigation_act->pNext = irrigation;
                    irrigation->pBack = irrigation_act;
                    irrigation_act = irrigation_act->pNext;
                }
		 }
			
	 if (self->irrigation_date_len != 0) // Hong: why?
			{
                irrigation = g_malloc0_n(1, sizeof(STIRRIGATION));
                irrigation->Day = 0;
                irrigation->Month = 0;
                irrigation->Year = 0;
                irrigation_act->pNext = irrigation;
                irrigation->pBack = irrigation_act;
                irrigation_act = irrigation_act->pNext;
            }
			
            pMa->pIrrigation = irrigation_first;
			self->irrigation_done = 0;
			
/////////////--------END OF READ IRRIGATION--------------------			

	 	 
// Free the variables	 
	 g_free(self->irrigation_date);
	 g_free(self->irrigation_type);
	 g_free(self->irrigation_code);
	 g_free(self->irrigation_amount);	
	 g_free(self->irrigation_rate);
	 g_free(self->irrigation_no3n);
	 g_free(self->irrigation_nh4n);
	 g_free(self->irrigation_org_n);
	 g_free(self->irrigation_org_c);
	 g_free(self->irrigation_gdate);

            
 
// Auf das neueste Datum stellen:
/*
        {
            PTILLAGE tillage = xpn->pMa->pTillage;

            if ((tillage != NULL) && (tillage->Day > 0)) {
                while ((tillage == NULL) || (xpn_time_compare_date(xpn->pTi->pSimTime->year,
                                         xpn->pTi->pSimTime->mon,
                                         xpn->pTi->pSimTime->mday,
                                         tillage->Year,
                                         tillage->Month,
                                         tillage->Day) > 0)) {
                    xpn->pMa->pTillage = xpn->pMa->pTillage->pNext;
                    tillage = xpn->pMa->pTillage;
                }
            }
        }
*/			
						
	if (error != NULL)
		g_error_free(error);

	g_key_file_free(keyfile);
	g_free(filename);

	return RET_SUCCESS;
}

//Relocated by Hong 
int InitBBGeraet(PTILLAGE tillage)
 {
	if (strcmp(tillage->acCode,"TI047\0")==0)        
	{ //Schaelpflug
		tillage->fEffMix = (double)0.4;
		tillage->fEffLoose = (double)0.75;
		tillage->fEffIncorp = (double)0.75;
	}

	if (strcmp(tillage->acCode,"TI048\0")==0)        	
	{ //Scharpflug
		tillage->fEffMix = (double)0.4;
		tillage->fEffLoose = (double)0.75;
		tillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI005\0")==0)        	
	{ //Volldrehpflug
		tillage->fEffMix = (double)0.4;
		tillage->fEffLoose = (double)0.95;
		tillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI049\0")==0)        	
	{ //Scheibenpflug
		tillage->fEffMix = (double)0.4;
		tillage->fEffLoose = (double)0.75;
		tillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI007\0")==0)        	
	{ //Scheibenpflug
		tillage->fEffMix = (double)0.4;
		tillage->fEffLoose = (double)0.75;
		tillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI030\0")==0)        	
	{ //Schichtengubber
		tillage->fEffMix = (double)0.6;
		tillage->fEffLoose = (double)0.55;
		tillage->fEffIncorp = (double)0.55;
	}
	if (strcmp(tillage->acCode,"TI010\0")==0)        	
	{ //Schaelgrubber
		tillage->fEffMix = (double)0.6;
		tillage->fEffLoose = (double)0.55;
		tillage->fEffIncorp = (double)0.35;
	}
	if (strcmp(tillage->acCode,"TI012\0")==0)        	
	{ //Federzahnegge
		tillage->fEffMix = (double)0.2;
		tillage->fEffLoose = (double)0.05;
		tillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI037\0")==0)        	
	{ //Feingrubber
		tillage->fEffMix = (double)0.2;
		tillage->fEffLoose = (double)0.05;
		tillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI032\0")==0)        	
	{ //Spatenrollegge
		tillage->fEffMix = (double)0.2;
		tillage->fEffLoose = (double)0.05;
		tillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI042\0")==0)        	
	{ //Ackerfraese
		tillage->fEffMix = (double)0.8;
		tillage->fEffLoose = (double)0.75;
		tillage->fEffIncorp = (double)0.64;
	}
	if (strcmp(tillage->acCode,"TI036\0")==0)        	
	{ //Striegelegge
		tillage->fEffMix = (double)0.2;
		tillage->fEffLoose = (double)0.0;
		tillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI033\0")==0)        	
	{ //Loeffelegge
		tillage->fEffMix = (double)0.5;
		tillage->fEffLoose = (double)0.0;
		tillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI035\0")==0)        	
	{ //Ruettelegge
		tillage->fEffMix = (double)0.7;
		tillage->fEffLoose = (double)0.0;
		tillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI034\0")==0)        	
	{ //Kreiselegge
		tillage->fEffMix = (double)0.7;
		tillage->fEffLoose = (double)0.05;
		tillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI013\0")==0)        	
	{ //Saategge
		tillage->fEffMix = (double)0.2;
		tillage->fEffLoose = (double)0.0;
		tillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI031\0")==0)        	
	{ //Scheibenegge
		tillage->fEffMix = (double)0.7;
		tillage->fEffLoose = (double)0.5;
		tillage->fEffIncorp = (double)0.5;
	}
	if (strcmp(tillage->acCode,"TI043\0")==0)        	
	{ //Zinkenroter
		tillage->fEffMix = (double)0.8;
		tillage->fEffLoose = (double)0.64;
		tillage->fEffIncorp = (double)0.64;
	}
	if (strcmp(tillage->acCode,"TI015\0")==0)        	
	{ //Rotortiller
		tillage->fEffMix = (double)0.8;
		tillage->fEffLoose = (double)0.64;
		tillage->fEffIncorp = (double)0.64;
	}
	if (strcmp(tillage->acCode,"TI024\0")==0)        	
	{ //Pflug mit Untergrundlockerer
		tillage->fEffMix = (double)0.4;
		tillage->fEffLoose = (double)0.95;
		tillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI025\0")==0)        	
	{ // Volldrehpflug mit Packer
		tillage->fEffMix = (double)0.4;
		tillage->fEffLoose = (double)0.55;
		tillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI004\0")==0)        	
	{ // Beetpflug
		tillage->fEffMix = (double)0.4;
		tillage->fEffLoose = (double)0.65;
		tillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI027\0")==0)        	
	{ // Spatenmaschine
		tillage->fEffMix = (double)0.3;
		tillage->fEffLoose = (double)0.75;
		tillage->fEffIncorp = (double)0.5;
	}
	if (strcmp(tillage->acCode,"TI029\0")==0)
	{ // Schwergrubber
		tillage->fEffMix = (double)0.65;
		tillage->fEffLoose = (double)0.65;
		tillage->fEffIncorp = (double)0.650;
	}
	if (strcmp(tillage->acCode,"TI038\0")==0)        	
	{ // Schleppegge
		tillage->fEffMix = (double)0.0;
		tillage->fEffLoose = (double)-0.05;
		tillage->fEffIncorp = (double)0.0;
	}
	if (strcmp(tillage->acCode,"TI039\0")==0)        	
	{ // Cambridge-Walze
		tillage->fEffMix = (double)0.0;
		tillage->fEffLoose = (double)-0.5;
		tillage->fEffIncorp = (double)0.0;
	}
	if (strcmp(tillage->acCode,"TI040\0")==0)        	
	{ // Crosskill-Walze
		tillage->fEffMix = (double)0.0;
		tillage->fEffLoose = (double)-0.5;
		tillage->fEffIncorp = (double)0.0;
	}
	if (strcmp(tillage->acCode,"TI041\0")==0)        	
	{ // Ringel-Walze
		tillage->fEffMix = (double)0.0;
		tillage->fEffLoose = (double)-0.5;
		tillage->fEffIncorp = (double)0.0;
	}
	if (strcmp(tillage->acCode,"TI044\0")==0)        	
	{ // Dammfraese
		tillage->fEffMix = (double)0.55;
		tillage->fEffLoose = (double)0.65;
		tillage->fEffIncorp = (double)0.15;
	}
	if (strcmp(tillage->acCode,"TI045\0")==0)        	
	{ // Tiefenmeissel
		tillage->fEffMix = (double)0.0;
		tillage->fEffLoose = (double)-0.65;
		tillage->fEffIncorp = (double)0.0;
	}
	if (strcmp(tillage->acCode,"TI046\0")==0)        	
	{ // Mulchgeraet
		tillage->fEffMix = (double)0.15;
		tillage->fEffLoose = (double)0.0;
		tillage->fEffIncorp = (double)0.25;
	}

      /* Wenn fuer das gewaehlte Geraet keine Zuordnung stattgefunden 
           hat wird es hier mit Standarwerten belegt. */
		if ((tillage->fEffMix ==(double)0.0) &&
		   (tillage->fEffLoose ==(double)0.0) &&
		   (tillage->fEffIncorp ==(double)0.0))
		   {
			tillage->fEffMix = (double)0.1;
			tillage->fEffLoose = (double)0.1;
			tillage->fEffIncorp = (double)0.1;
		   }


  // PRINT_MESSAGE(xpn,4,"InitBBGeraet geladen!")
   
   return RET_SUCCESS;
 }      /*===== Ende der Initialisierung der Bodenbearbeitungsgeraete =============*/
 