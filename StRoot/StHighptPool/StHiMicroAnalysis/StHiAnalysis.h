/***************************************************************************
 *
 * $Id: StHiAnalysis.h,v 1.2 2002/05/31 21:58:29 jklay Exp $                                    
 *
 * Author: Bum Choi, UT Austin, Apr 2002
 *
 ***************************************************************************
 *
 * Description:  Class to perform highpt Analysis on highpt uDSTs
 *
 *
 ***************************************************************************
 * 
 * $Log: StHiAnalysis.h,v $
 * Revision 1.2  2002/05/31 21:58:29  jklay
 * Updated analysis code to use new cut class
 *
 * Revision 1.1  2002/04/02 20:05:18  jklay
 * Bums analysis tools for highpt uDSTs
 *
 *
 **************************************************************************/
#ifndef StHiAnalysis_H
#define StHiAnalysis_H

#include "StHiBaseAnalysis.h"

class StHiAnalysis : public StHiBaseAnalysis{
 public:
  StHiAnalysis(const char* inputDir="./", 
	       const char* outRootName="hianalysis.hist.root");
  virtual ~StHiAnalysis();

 private:

  void initHistograms();
  void fillEventHistograms();
  void finishHistograms();

  void trackLoop();
  
  //***************************************************************
  // histograms
  //

  //Event level histograms
  TH3D* h3VertexXYZ;  
  TH2D* h2ZDCSumVsCTB;

  TH2D* h2NGoodGlobalsVsNch;
  TH1D* h1FlowCent;

  TH1D* h1NEvent; //! # of events for the centrality class
  TH1D* h1EtaCut; //! just in case, store the eta cut  
  
//Track level histograms
  TH1D* h1FitPts;

  //Before cut on specified variable, but all cut on |eta|<0.5
  TH2D* h2DcaGlVsSector;  
  TH2D* h2DcaXYGlVsSector;  
  TH2D* h2FitPtsVsSector;  
  TH2D* h2MaxPtsVsSector;  
  TH2D* h2AllPtsVsSector;  

  //After all track cuts
  TH1D* h1YieldVsSector;
  TH2D* h2PrPtVsSector;  
  TH2D* h2GlPtVsSector;  
  TH2D* h2ResPrPtVsSector;  
  TH2D* h2ResGlPtVsSector;  

  // 0 is plus, 1 is minus 2 is plus+minus  
  struct PlusMinus{
    TH3D* h3PhiPrDcaXYGlPtPr; // look at phi counts
    TH3D* h3PhiGlDcaXYGlPtGl;
  
    TH3D* h3DcaGlDcaXYGlPtPr; // look at dca's for different pt bins
    TH3D* h3DcaGlDcaXYGlPtGl;

    // --backgrounds
    TH2D* h2SDcaGlPtPrRebin;
    TH2D* h2DcaXYGlPtPrRebin;
    TH2D* h2DcaGlPtPrRebin;
    // --
    
    TH3D* h3PhiPrFitPtsPtPr; // phi,fit pts,pt
    TH3D* h3VtxZFitPtsPtPr; // vtx z, fit pts, pt
    TH3D* h3VtxZFitPtsEtaPr; // vtx z, fit pts, eta

    TH3D* h3PhiPrAllPtsPtPr; // phi, all pts, pt
    TH3D* h3PhiPrMaxPtsPtPr; // phi, all pts, pt
    
    TH3D* h3FlowCentFitPtsPtPr; // flow cent, fit pts, pt

    //TH2D* h2ZDCCentralityPtPr; // pt yields and centrality
    TH2D* h2CentralityPtPr;
    TH3D* h3VtxZEtaPrPtPr; // reality check of the eta vs vertex
    TH3D* h3VtxZEtaGlPtGl;

    TH1D* h1RawPtGlVarBin0; //
    TH1D* h1RawPtGlVarBin1; //
    TH1D* h1RawPtPrVarBin0; //
    TH1D* h1RawPtPrVarBin1; //
  };

  //0 is East 1 is West 2 is Both 
  struct EastWest{
    PlusMinus pm[3];   
  };

  EastWest ew[3]; //!


  TH3D* h3ResPtPrGlPtPrDcaXYGl; //!
  TH3D* h3ResPtPrGlPtGlDcaXYGl; //!
 

  ClassDef(StHiAnalysis,1)
};

#endif
