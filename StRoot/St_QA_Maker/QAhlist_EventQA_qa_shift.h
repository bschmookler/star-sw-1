// $Id: QAhlist_EventQA_qa_shift.h,v 2.8 2001/06/27 23:57:50 lansdell Exp $
// $Log: QAhlist_EventQA_qa_shift.h,v $
// Revision 2.8  2001/06/27 23:57:50  lansdell
// added geant-reco primvtx position histos to qa_shift list
//
// Revision 2.7  2001/05/29 23:23:05  lansdell
// removed impact param plots for FTPC from qa_shift list
//
// Revision 2.6  2001/05/25 16:31:20  lansdell
// more updates to qa shift histograms
//
// Revision 2.5  2001/05/24 01:48:13  lansdell
// qa_shift histograms updated
//
// Revision 2.4  2001/05/23 00:14:52  lansdell
// more changes for qa_shift histograms
//
// Revision 2.3  2001/05/16 20:57:02  lansdell
// new histograms added for qa_shift printlist; some histogram ranges changed; StMcEvent now used in StEventQA
//
// Revision 2.2  2001/04/24 22:53:50  lansdell
// Removed redundant radial position of first hit histograms
//
// Revision 2.1  2000/08/25 16:04:09  genevb
// Introduction of files
//
//
///////////////////////////////////////////////////////////////////////
// Names of histograms to be plotted for dir=EventQA, analType=qa_shift
///////////////////////////////////////////////////////////////////////
// Note: Editing this file means that StAnalysisUtilities/StHistUtil
// must be recompiled

  "StEQaMultClass",
  "StEQaPointTpc",
  "StEQaPointSvt",
  "StEQaPointFtpc",
  "StEQaPointZhits",
  "StEQaPointPhiT",
  "StEQaPointZhitsS",
  "StEQaPointPhiS",
  "StEQaPointPadrowT",
  "StEQaPointBarrelS",
  "StEQaPointPlaneF",
  "StEQaRichTot",
  "StEQaGtrkZfTS",
  "StEQaGtrkPhifTS",
  "StEQaGtrkZfT",
  "StEQaGtrkPhifT",
  "StEQaGtrkXfYfFE",
  "StEQaGtrkXfYfFW",
  "StEQaGtrkPadfTEW",
  "StEQaGtrkRTS",
  "StEQaGtrkPlanefF",
  "StEQaGtrkRnfTTS",
  "StEQaGtrkRnmfTTS",
  "StEQaGtrkRnmF",
  "StEQaGtrkPsiTTS",
  "StEQaGtrkPtTTS",
  "StEQaGtrkEtaTTS",
  "StEQaGtrkPsiF",
  "StEQaGtrkPtF",
  "StEQaGtrkEtaF",
  "StEQaGtrkPF",
  "StEQaGtrkChisq0TTS",
  "StEQaGtrkChisq1TTS",
  "StEQaGtrkFlag",
  "StEQaGtrkGoodTot",
  "StEQaGtrkNPntFitTTS",
  "StEQaGtrkNPntF",
  "StEQaGtrkGoodTTS",
  "StEQaGtrkGoodF",
  "StEQaGtrkFitPntLTTS",
  "StEQaGtrkImpactTTS",
  "StEQaGtrkImpactrTTS",
  "StEQaGtrkImpactF",
  "StEQaGtrkDetId",
  "StEQaGtrkTanlzf",
  "StEQaGtrkTanlzfTS",
  "StEQaPtrkPsiTTS",
  "StEQaPtrkPtTTS",
  "StEQaPtrkEtaTTS",
  "StEQaPtrkPsiF",
  "StEQaPtrkPtF",
  "StEQaPtrkEtaF",
  "StEQaPtrkMeanPtTTS",
  "StEQaPtrkMeanEtaTTS",
  "StEQaPtrkMeanPtF",
  "StEQaPtrkMeanEtaF",
  "StEQaPtrkGoodTTS",
  "StEQaPtrkGoodF",
  "StEQaPtrkChisq0TTS",
  "StEQaPtrkChisq1TTS",
  "StEQaPtrkFlag",
  "StEQaPtrkImpactTTS",
  "StEQaPtrkImpactrTTS",
  "StEQaPtrkImpactF",
  "StEQaPtrkGlob",
  "StEQaPtrkFitPntLTTS",
  "StEQaNullPrimVtx",
  "StEQaVtxPrXY",
  "StEQaVtxPrZ",
  "StEQaGtrkDcaBeamZ1",
  "StEQaGtrkDcaBeamZ2",
  "StEQaGtrkZdcaZf",
  "StEQaGtrkZdcaPsi",
  "StEQaGtrkZdcaTanl",
  "QaDedxAllSectors",
  "StEQaPidGlobtrkDstdedxPVsDedx",
  "StEQaDedxBBTTS",
  "StEQaEvsumTotChg",
  "StEQaEvsumTotChgF",
  "StEQaV0Vtx",
  "StEQaV0VtxRDist",
  "StEQaV0VtxZDist",
  "StEQaV0VtxPhiDist",
  "StEQaV0LambdaMass",
  "StEQaV0K0Mass",
  "StEQaXiVtxTot",
  "StEQaKinkTot",
  "StEQaGtrkRZf0",
  "StEQaGtrkRZf0TS",
  "StEQaPtrkRZf0",
  "StEQaPtrkRZf0TS",
  "StEbemcClNum",
  "StEbemcClEnergy",
  "StEbemcEta",
  "StEbemcPhi",
  "StEbsmdeClNum",
  "StEbsmdeEta",
  "StEbsmdpClNum",
  "StEbsmdpPhi",
  "StEEmcCat4_Point_Energy",
  "StEEmcCat4_Point_Eta",
  "StEEmcCat4_Point_Phi",
  "StEEmcCat4_Sigma_Eta",
  "StEEmcCat4_Sigma_Phi",
  "StEEmcCat4_Delta_Eta",
  "StEEmcCat4_Delta_Phi",
  "StEEmcCat4_Points_Multiplicity",
  "StEEmcCat4_Track_Momenta",
  "StE Point Flag",
  "StEQaGRpvtxDx",
  "StEQaGRpvtxDy",
  "StEQaGRpvtxDz",

  "StELMQaPointTpc",
  "StELMQaPointSvt",
  "StELMQaPointFtpc",
  "StELMQaPointZhits",
  "StELMQaPointPhiT",
  "StELMQaPointZhitsS",
  "StELMQaPointPhiS",
  "StELMQaPointPadrowT",
  "StELMQaPointBarrelS",
  "StELMQaPointPlaneF",
  "StELMQaRichTot",
  "StELMQaGtrkZfTS",
  "StELMQaGtrkPhifTS",
  "StELMQaGtrkZfT",
  "StELMQaGtrkPhifT",
  "StELMQaGtrkXfYfFE",
  "StELMQaGtrkXfYfFW",
  "StELMQaGtrkPadfTEW",
  "StELMQaGtrkRTS",
  "StELMQaGtrkPlanefF",
  "StELMQaGtrkRnfTTS",
  "StELMQaGtrkRnmfTTS",
  "StELMQaGtrkRnmF",
  "StELMQaGtrkPsiTTS",
  "StELMQaGtrkPtTTS",
  "StELMQaGtrkEtaTTS",
  "StELMQaGtrkPsiF",
  "StELMQaGtrkPtF",
  "StELMQaGtrkEtaF",
  "StELMQaGtrkPF",
  "StELMQaGtrkChisq0T",
  "StELMQaGtrkChisq1T",
  "StELMQaGtrkFlag",
  "StELMQaGtrkGoodTot",
  "StELMQaGtrkNPntFitTTS",
  "StELMQaGtrkNPntF",
  "StELMQaGtrkGoodTTS",
  "StELMQaGtrkGoodF",
  "StELMQaGtrkFitPntLTTS",
  "StELMQaGtrkImpactTTS",
  "StELMQaGtrkImpactrTTS",
  "StELMQaGtrkImpactF",
  "StELMQaGtrkDetId",
  "StELMQaGtrkTanlzf",
  "StELMQaGtrkTanlzfTS",
  "StELMQaPtrkPsiTTS",
  "StELMQaPtrkPtTTS",
  "StELMQaPtrkEtaTTS",
  "StELMQaPtrkPsiF",
  "StELMQaPtrkPtF",
  "StELMQaPtrkEtaF",
  "StELMQaPtrkMeanPtTTS",
  "StELMQaPtrkMeanEtaTTS",
  "StELMQaPtrkMeanPtF",
  "StELMQaPtrkMeanEtaF",
  "StELMQaPtrkGoodTTS",
  "StELMQaPtrkGoodF",
  "StELMQaPtrkChisq0TTS",
  "StELMQaPtrkChisq1TTS",
  "StELMQaPtrkFlag",
  "StELMQaPtrkImpactTTS",
  "StELMQaPtrkImpactrTTS",
  "StELMQaPtrkImpactF",
  "StELMQaPtrkGlob",
  "StELMQaPtrkFitPntLTTS",
  "StELMQaNullPrimVtx",
  "StELMQaVtxPrXY",
  "StELMQaVtxPrZ",
  "StELMQaGtrkDcaBeamZ1",
  "StELMQaGtrkDcaBeamZ2",
  "StELMQaGtrkZdcaZf",
  "StELMQaGtrkZdcaPsi",
  "StELMQaGtrkZdcaTanl",
  "StELMQaPidGlobtrkDstdedxPVsDedx",
  "StELMQaDedxBBTTS",
  "StELMQaEvsumTotChg",
  "StELMQaEvsumTotChgF",
  "StELMQaV0Vtx",
  "StELMQaV0VtxRDist",
  "StELMQaV0VtxZDist",
  "StELMQaV0VtxPhiDist",
  "StELMQaV0LambdaMass",
  "StELMQaV0K0Mass",
  "StELMQaXiVtxTot",
  "StELMQaKinkTot",
  "StELMQaGtrkRZf0",
  "StELMQaGtrkRZf0TS",
  "StELMQaPtrkRZf0",
  "StELMQaPtrkRZf0TS",
  "StELMbemcClNum",
  "StELMbemcClEnergy",
  "StELMbemcEta",
  "StELMbemcPhi",
  "StELMbsmdeClNum",
  "StELMbsmdeEta",
  "StELMbsmdpClNum",
  "StELMbsmdpPhi",
  "StELMEmcCat4_Point_Energy",
  "StELMEmcCat4_Point_Eta",
  "StELMEmcCat4_Point_Phi",
  "StELMEmcCat4_Sigma_Eta",
  "StELMEmcCat4_Sigma_Phi",
  "StELMEmcCat4_Delta_Eta",
  "StELMEmcCat4_Delta_Phi",
  "StELMEmc_Cat4_Points_Multiplicity",
  "StELMEmcCat4_Track_Momenta",
  "StELM Point Flag",
  "StELMQaGRpvtxDx",
  "StELMQaGRpvtxDy",
  "StELMQaGRpvtxDz",

  "StEMMQaPointTpc",
  "StEMMQaPointSvt",
  "StEMMQaPointFtpc",
  "StEMMQaPointZhits",
  "StEMMQaPointPhiT",
  "StEMMQaPointZhitsS",
  "StEMMQaPointPhiS",
  "StEMMQaPointPadrowT",
  "StEMMQaPointBarrelS",
  "StEMMQaPointPlaneF",
  "StEMMQaRichTot",
  "StEMMQaGtrkZfTS",
  "StEMMQaGtrkPhifTS",
  "StEMMQaGtrkZfT",
  "StEMMQaGtrkPhifT",
  "StEMMQaGtrkXfYfFE",
  "StEMMQaGtrkXfYfFW",
  "StEMMQaGtrkPadfTEW",
  "StEMMQaGtrkRTS",
  "StEMMQaGtrkPlanefF",
  "StEMMQaGtrkRnfTTS",
  "StEMMQaGtrkRnmfTTS",
  "StEMMQaGtrkRnmF",
  "StEMMQaGtrkPsiTTS",
  "StEMMQaGtrkPtTTS",
  "StEMMQaGtrkEtaTTS",
  "StEMMQaGtrkPsiF",
  "StEMMQaGtrkPtF",
  "StEMMQaGtrkEtaF",
  "StEMMQaGtrkPF",
  "StEMMQaGtrkChisq0T",
  "StEMMQaGtrkChisq1T",
  "StEMMQaGtrkFlag",
  "StEMMQaGtrkGoodTot",
  "StEMMQaGtrkNPntFitTTS",
  "StEMMQaGtrkNPntF",
  "StEMMQaGtrkGoodTTS",
  "StEMMQaGtrkGoodF",
  "StEMMQaGtrkFitPntLTTS",
  "StEMMQaGtrkImpactTTS",
  "StEMMQaGtrkImpactrTTS",
  "StEMMQaGtrkImpactF",
  "StEMMQaGtrkDetId",
  "StEMMQaGtrkTanlzf",
  "StEMMQaGtrkTanlzfTS",
  "StEMMQaPtrkPsiTTS",
  "StEMMQaPtrkPtTTS",
  "StEMMQaPtrkEtaTTS",
  "StEMMQaPtrkPsiF",
  "StEMMQaPtrkPtF",
  "StEMMQaPtrkEtaF",
  "StEMMQaPtrkMeanPtTTS",
  "StEMMQaPtrkMeanEtaTTS",
  "StEMMQaPtrkMeanPtF",
  "StEMMQaPtrkMeanEtaF",
  "StEMMQaPtrkGoodTTS",
  "StEMMQaPtrkGoodF",
  "StEMMQaPtrkChisq0TTS",
  "StEMMQaPtrkChisq1TTS",
  "StEMMQaPtrkFlag",
  "StEMMQaPtrkImpactTTS",
  "StEMMQaPtrkImpactrTTS",
  "StEMMQaPtrkImpactF",
  "StEMMQaPtrkGlob",
  "StEMMQaPtrkFitPntLTTS",
  "StEMMQaNullPrimVtx",
  "StEMMQaVtxPrXY",
  "StEMMQaVtxPrZ",
  "StEMMQaGtrkDcaBeamZ1",
  "StEMMQaGtrkDcaBeamZ2",
  "StEMMQaGtrkZdcaZf",
  "StEMMQaGtrkZdcaPsi",
  "StEMMQaGtrkZdcaTanl",
  "StEMMQaPidGlobtrkDstdedxPVsDedx",
  "StEMMQaDedxBBTTS",
  "StEMMQaEvsumTotChg",
  "StEMMQaEvsumTotChgF",
  "StEMMQaV0Vtx",
  "StEMMQaV0VtxRDist",
  "StEMMQaV0VtxZDist",
  "StEMMQaV0VtxPhiDist",
  "StEMMQaV0LambdaMass",
  "StEMMQaV0K0Mass",
  "StEMMQaXiVtxTot",
  "StEMMQaKinkTot",
  "StEMMQaGtrkRZf0",
  "StMMEQaGtrkRZf0TS",
  "StEMMQaPtrkRZf0",
  "StEMMQaPtrkRZf0TS",
  "StEMMbemcClNum",
  "StEMMbemcClEnergy",
  "StEMMbemcEta",
  "StEMMbemcPhi",
  "StEMMbsmdeClNum",
  "StEMMbsmdeEta",
  "StEMMbsmdpClNum",
  "StEMMbsmdpPhi",
  "StEMMEmcCat4_Point_Energy",
  "StEMMEmcCat4_Point_Eta",
  "StEMMEmcCat4_Point_Phi",
  "StEMMEmcCat4_Sigma_Eta",
  "StEMMEmcCat4_Sigma_Phi",
  "StEMMEmcCat4_Delta_Eta",
  "StEMMEmcCat4_Delta_Phi",
  "StEMMEmc_Cat4_Points_Multiplicity",
  "StEMMEmcCat4_Track_Momenta",
  "StEMM Point Flag",
  "StEMMQaGRpvtxDx",
  "StEMMQaGRpvtxDy",
  "StEMMQaGRpvtxDz",

  "StEHMQaPointTpc",
  "StEHMQaPointSvt",
  "StEHMQaPointFtpc",
  "StEHMQaPointZhits",
  "StEHMQaPointPhiT",
  "StEHMQaPointZhitsS",
  "StEHMQaPointPhiS",
  "StEHMQaPointPadrowT",
  "StEHMQaPointBarrelS",
  "StEHMQaPointPlaneF",
  "StEHMQaRichTot",
  "StEHMQaGtrkZfTS",
  "StEHMQaGtrkPhifTS",
  "StEHMQaGtrkZfT",
  "StEHMQaGtrkPhifT",
  "StEHMQaGtrkXfYfFE",
  "StEHMQaGtrkXfYfFW",
  "StEHMQaGtrkPadfTEW",
  "StEHMQaGtrkRTS",
  "StEHMQaGtrkPlanefF",
  "StEHMQaGtrkRnfTTS",
  "StEHMQaGtrkRnmfTTS",
  "StEHMQaGtrkRnmF",
  "StEHMQaGtrkPsiTTS",
  "StEHMQaGtrkPtTTS",
  "StEHMQaGtrkEtaTTS",
  "StEHMQaGtrkPsiF",
  "StEHMQaGtrkPtF",
  "StEHMQaGtrkEtaF",
  "StEHMQaGtrkPF",
  "StEHMQaGtrkChisq0T",
  "StEHMQaGtrkChisq1T",
  "StEHMQaGtrkFlag",
  "StEHMQaGtrkGoodTot",
  "StEHMQaGtrkNPntFitTTS",
  "StEHMQaGtrkNPntF",
  "StEHMQaGtrkGoodTTS",
  "StEHMQaGtrkGoodF",
  "StEHMQaGtrkFitPntLTTS",
  "StEHMQaGtrkImpactTTS",
  "StEHMQaGtrkImpactrTTS",
  "StEHMQaGtrkImpactF",
  "StEHMQaGtrkDetId",
  "StEHMQaGtrkTanlzf",
  "StEHMQaGtrkTanlzfTS",
  "StEHMQaPtrkPsiTTS",
  "StEHMQaPtrkPtTTS",
  "StEHMQaPtrkEtaTTS",
  "StEHMQaPtrkPsiF",
  "StEHMQaPtrkPtF",
  "StEHMQaPtrkEtaF",
  "StEHMQaPtrkMeanPtTTS",
  "StEHMQaPtrkMeanEtaTTS",
  "StEHMQaPtrkMeanPtF",
  "StEHMQaPtrkMeanEtaF",
  "StEHMQaPtrkGoodTTS",
  "StEHMQaPtrkGoodF",
  "StEHMQaPtrkChisq0TTS",
  "StEHMQaPtrkChisq1TTS",
  "StEHMQaPtrkFlag",
  "StEHMQaPtrkImpactTTS",
  "StEHMQaPtrkImpactrTTS",
  "StEHMQaPtrkImpactF",
  "StEHMQaPtrkGlob",
  "StEHMQaPtrkFitPntLTTS",
  "StEHMQaNullPrimVtx",
  "StEHMQaVtxPrXY",
  "StEHMQaVtxPrZ",
  "StEHMQaGtrkDcaBeamZ1",
  "StEHMQaGtrkDcaBeamZ2",
  "StEHMQaGtrkZdcaZf",
  "StEHMQaGtrkZdcaPsi",
  "StEHMQaGtrkZdcaTanl",
  "StEHMQaPidGlobtrkDstdedxPVsDedx",
  "StEHMQaDedxBBTTS",
  "StEHMQaEvsumTotChg",
  "StEHMQaEvsumTotChgF",
  "StEHMQaV0Vtx",
  "StEHMQaV0VtxRDist",
  "StEHMQaV0VtxZDist",
  "StEHMQaV0VtxPhiDist",
  "StEHMQaV0LambdaMass",
  "StEHMQaV0K0Mass",
  "StEHMQaXiVtxTot",
  "StEHMQaKinkTot",
  "StEHMQaGtrkRZf0",
  "StEHMQaGtrkRZf0TS",
  "StEHMQaPtrkRZf0",
  "StEHMQaPtrkRZf0TS",
  "StEHMbemcClNum",
  "StEHMbemcClEnergy",
  "StEHMbemcEta",
  "StEHMbemcPhi",
  "StEHMbsmdeClNum",
  "StEHMbsmdeEta",
  "StEHMbsmdpClNum",
  "StEHMbsmdpPhi",
  "StEHMEmcCat4_Point_Energy",
  "StEHMEmcCat4_Point_Eta",
  "StEHMEmcCat4_Point_Phi",
  "StEHMEmcCat4_Sigma_Eta",
  "StEHMEmcCat4_Sigma_Phi",
  "StEHMEmcCat4_Delta_Eta",
  "StEHMEmcCat4_Delta_Phi",
  "StEHMEmc_Cat4_Points_Multiplicity",
  "StEHMEmcCat4_Track_Momenta",
  "StEHM Point Flag",
  "StEHMQaGRpvtxDx",
  "StEHMQaGRpvtxDy",
  "StEHMQaGRpvtxDz"
