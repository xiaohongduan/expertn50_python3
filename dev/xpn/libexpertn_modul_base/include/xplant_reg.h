#define XPLANT_REG \
xpn_register_var_init_pdouble( var_list, pPl->pRoot->fDepth,"pPl.pRoot.fDepth",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pRoot->fDepthGrowR,"pPl.pRoot.fDepthGrowR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pRoot->fMaxDepth,"pPl.pRoot.fMaxDepth",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pRoot->fUptakeDay,"pPl.pRoot.fUptakeDay",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pRoot->fUptakeR,"pPl.pRoot.fUptakeR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pRoot->fTotalLength,"pPl.pRoot.fTotalLength",0.0);\
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fPotWatUpt,"pPl.pRoot.pLayerRoot.fPotWatUpt",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fActWatUpt,"pPl.pRoot.pLayerRoot.fActWatUpt",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fPotLayWatUpt,"pPl.pRoot.pLayerRoot.fPotLayWatUpt",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fActLayWatUpt,"pPl.pRoot.pLayerRoot.fActLayWatUpt",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fLengthDens,"pPl.pRoot.pLayerRoot.fLengthDens",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fLengthDensFac ,"pPl.pRoot.pLayerRoot.fLengthDensFac ",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fDeadFineRootWeight_dt ,"pPl.pRoot.pLayerRoot.fDeadFineRootWeight_dt ",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fDeadGrossRootWeight_dt ,"pPl.pRoot.pLayerRoot.fDeadGrossRootWeight_dt ",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fPotWatUptAF ,"pPl.pRoot.pLayerRoot.fPotWatUptAF ",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fActWatUptAF ,"pPl.pRoot.pLayerRoot.fActWatUptAF ",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fPotLayWatUptAF ,"pPl.pRoot.pLayerRoot.fPotLayWatUptAF ",0.0); \
xpn_register_var_init_rec_struct( var_list,STLAYERROOT, pPl->pRoot->pLayerRoot,xpn_register_var_init_pdouble,fActLayWatUptAF ,"pPl.pRoot.pLayerRoot.fActLayWatUptAF ",0.0); \
xpn_register_var_init_pdouble( var_list, pPl->pPltClimate->fPlantAlbedo,"pPl.pPltClimate.fPlantAlbedo",0.23);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fRoughnessHeat,"pPl.pCanopy.fRoughnessHeat",0.01);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fRoughnessVapor,"pPl.pCanopy.fRoughnessVapor",0.01);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fkcan,"pPl.pCanopy.fkcan",0.6);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fsto_res,"pPl.pCanopy.fsto_res",100.0);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fTillerNum,"pPl.pCanopy.fTillerNum",0.01);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fTillerNumSq,"pPl.pCanopy.fTillerNumSq",0.01);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fMainStemTillers,"pPl.pCanopy.fMainStemTillers",0.01);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fLeafAppearR,"pPl.pCanopy.fLeafAppearR",0.01);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fExpandLeafNum,"pPl.pCanopy.fExpandLeafNum",0.01);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fPlantHeight,"pPl.pCanopy.fPlantHeight",1.0e-34);\
xpn_register_var_init_pdouble( var_list, pPl->pPltWater->fPotUptake,"pPl.pPltWater.fPotUptake",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pDevelop->fStageXCERES,"pPl.pDevelop.fStageXCERES",0.0);\
xpn_register_var_init_pint( var_list, pPl->pDevelop->iStageCERES,"pPl.pDevelop.iStageCERES",0);\
xpn_register_var_init_pdouble( var_list, pPl->pDevelop->fStageWang,"pPl.pDevelop.fStageWang",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pDevelop->fStageSUCROS,"pPl.pDevelop.fStageSUCROS",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pDevelop->fDTT,"pPl.pDevelop.fDTT",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pDevelop->fSumDTT,"pPl.pDevelop.fSumDTT",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pDevelop->fDevR,"pPl.pDevelop.fDevR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pDevelop->fDaylengthEff,"pPl.pDevelop.fDaylengthEff",1.0);\
xpn_register_var_init_pdouble( var_list, pPl->pDevelop->fThermEffect,"pPl.pDevelop.fThermEff",1.0);\
xpn_register_var_init_pint( var_list, pPl->pDevelop->iDayAftEmerg,"pPl.pDevelop.iDayAftEmerg",1);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fLeafCont,"pPl.pPltNitrogen.fLeafCont",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fLeafActConc,"pPl.pPltNitrogen.fLeafActConc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fStemCont,"pPl.pPltNitrogen.fStemCont",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fStemActConc,"pPl.pPltNitrogen.fStemActConc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fRootCont,"pPl.pPltNitrogen.fRootCont",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fRootActConc,"pPl.pPltNitrogen.fRootActConc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fLeafNinc,"pPl.pPltNitrogen.fLeafNinc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fLeafNtransRate,"pPl.pPltNitrogen.fLeafNtransRate",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fLeafDeathRate,"pPl.pBiomass.fLeafDeathRate",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fLeafMinConc,"pPl.pPltNitrogen.fLeafMinConc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fStemNinc,"pPl.pPltNitrogen.fStemNinc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fStemNtransRate,"pPl.pPltNitrogen.fStemNtransRate",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fRootNinc,"pPl.pPltNitrogen.fRootNinc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fRootNtransRate,"pPl.pPltNitrogen.fRootNtransRate",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fRootDeathRate,"pPl.pBiomass.fRootDeathRate",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fRootMinConc,"pPl.pPltNitrogen.fRootMinConc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fActNUptR,"pPl.pPltNitrogen.fActNUptR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fCumActNUpt,"pPl.pPltNitrogen.fCumActNUpt",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fLeafDemand,"pPl.pPltNitrogen.fLeafDemand",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fStemDemand,"pPl.pPltNitrogen.fStemDemand",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fRootDemand,"pPl.pPltNitrogen.fRootDemand",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fTotalDemand,"pPl.pPltNitrogen.fTotalDemand",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fLeafGrowR,"pPl.pBiomass.fLeafGrowR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fStemGrowR,"pPl.pBiomass.fStemGrowR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fRootGrowR,"pPl.pBiomass.fRootGrowR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fBiomGrowR,"pPl.pBiomass.fBiomGrowR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fLeafMaxConc,"pPl.pPltNitrogen.fLeafMaxConc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fStemMaxConc,"pPl.pPltNitrogen.fStemMaxConc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fRootMaxConc,"pPl.pPltNitrogen.fRootMaxConc",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fGrainDemand,"pPl.pPltNitrogen.fGrainDemand",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fLeafTransNw,"pPl.pPltNitrogen.fLeafTransNw",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fStemTransNw,"pPl.pPltNitrogen.fStemTransNw",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fRootTransNw,"pPl.pPltNitrogen.fRootTransNw",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltNitrogen->fTotalTransNw,"pPl.pPltNitrogen.fTotalTransNw",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltCarbon->fGrossPhotosynR,"pPl.pPltCarbon.fGrossPhotosynR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fTempMinPs,"pPl.pGenotype.fTempMinPs",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fTempOptPs,"pPl.pGenotype.fTempOptPs",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fTempMaxPs,"pPl.pGenotype.fTempMaxPs",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fMaxGrossPs,"pPl.pGenotype.fMaxGrossPs",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fLightUseEff,"pPl.pGenotype.fLightUseEff",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fCO2CmpPoint,"pPl.pGenotype.fCO2CmpPoint",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fCiCaRatio,"pPl.pGenotype.fCiCaRatio",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fPartFracLeaf,"pPl.pBiomass.fPartFracLeaf",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fPartFracStem,"pPl.pBiomass.fPartFracStem",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fPartFracRoot,"pPl.pBiomass.fPartFracRoot",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fReserveTransRate,"pPl.pBiomass.fReserveTransRate",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltCarbon->fMaintRespR,"pPl.pPltCarbon.fMaintRespR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fMaintLeaf,"pPl.pGenotype.fMaintLeaf",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fMaintRoot,"pPl.pGenotype.fMaintRoot",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fMaintStem,"pPl.pGenotype.fMaintStem",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fStemReserveWeight,"pPl.pBiomass.fStemReserveWeight",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fMaintStorage,"pPl.pGenotype.fMaintStorage",0.0);\
xpn_register_var_init_pint( var_list, pPl->pDevelop->bMaturity,"pPl.pDevelop.bMaturity",FALSE);\
xpn_register_var_init_pint( var_list, pPl->pDevelop->bPlantGrowth,"pPl.pDevelop.bPlantGrowth",FALSE);\
xpn_register_var_init_pint( var_list, pPl->pDevelop->iAFTrue,"pPl.pDevelop.iAFTrue",0);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fPlantGreenLA,"pPl.pCanopy.fPlantGreenLA",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pCanopy->fPlantSenesLA,"pPl.pCanopy.fPlantSenesLA",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltWater->fPotTranspR,"pPl.pPltWater.fPotTranspR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltWater->fActTranspR,"pPl.pPltWater.fActTranspR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltWater->fPotTranspAF,"pPl.pPltWater.fPotTranspAF",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltWater->fActTranspAF,"pPl.pPltWater.fActTranspAF",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltWater->fPotUptakeR,"pPl.pPltWater.fPotUptakeR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pPltWater->fActUptakeR,"pPl.pPltWater.fActUptakeR",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fResidueAMO1Frac,"pPl.pGenotype.fResidueAMO1Frac",1.0);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fCDeadleafFrac,"pPl.pGenotype.fCDeadleafFrac",0.4);\
xpn_register_var_init_pdouble( var_list, pPl->pGenotype->fNDeadleafFrac,"pPl.pGenotype.fNDeadleafFrac",1.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fDeadLeafWeight,"pPl.pBiomass.fDeadLeafWeight",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fDeadStemWeight,"pPl.pBiomass.fDeadStemWeight",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fDeadRootWeight,"pPl.pBiomass.fDeadRootWeight",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fDeadLeafWeight_dt,"pPl.pBiomass.fDeadLeafWeight pro time step",0.0);\
xpn_register_var_init_pdouble( var_list, pPl->pBiomass->fDeadStemWeight_dt,"pPl.pBiomass.fDeadStemWeight pro time step",0.0);