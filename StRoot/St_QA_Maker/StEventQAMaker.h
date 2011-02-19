/*!
  \class StEventQAMaker
                                                                     
  StEventQAMaker class for QA Histograms using StEvent                 
  adapted from St_QA_Maker                                          
 
*/

#ifndef STAR_StEventQAMaker
#define STAR_StEventQAMaker

#include "StQAMakerBase.h"

class StEvent;
class StPrimaryVertex;
class HitHistograms;
class StPmdGeom;
class StPmdMapUtil;
class StTrackDetectorInfo;

//////////////////////////////////////////////////////////////////////////

class StEventQAMaker : public StQAMakerBase {
 private:
 
  StEvent *event;          //! pointer to current event
  StPrimaryVertex *primVtx;//! pointer to the first primary vertex
  HitHistograms *mHitHist; //!
  Int_t n_prim_good;
  Int_t n_glob_good;
  Int_t   multiplicity; // multiplicity of current event
  StPmdGeom* mPmdGeom;     //!
  StPmdMapUtil* maputil;   //!
  Int_t mRunNumber;
  Bool_t silHists;      // whether we're doing SVT,SSD hists
  Bool_t hitsAvail;     // whether TPC hits are available
  Float_t vertExists;   // prim vertex : none/questionable/good
  
  static const Int_t mNVPD = 19;
  static const Int_t mEastVpdTrayId = 121;
  static const Int_t mWestVpdTrayId = 122;
  Float_t mEastVpdCorr[mNVPD];
  Float_t mWestVpdCorr[mNVPD];

  Int_t PCThits(StTrackDetectorInfo* detInfo);
  
//------------------------------------------------------------------------
  
 public: 

  StEventQAMaker(const char *name="EventQA", const char *title="StEvent/QA");
  virtual       ~StEventQAMaker() {}
  virtual Int_t  Init();
  virtual Int_t  InitRun(int runnumber);
  virtual Int_t  Finish();
  virtual Int_t  Make();
  
  virtual void   MakeHistGlob();
  virtual void   MakeHistDE();
  virtual void   MakeHistPrim();
  virtual void   MakeHistPID();
  virtual void   MakeHistVertex();
  virtual void   MakeHistPoint();
  virtual void   MakeHistEMC();
  virtual void   MakeHistEval();
  virtual void   MakeHistBBC();
  virtual void   MakeHistFPD();
  virtual void   MakeHistPMD();
  virtual void   MakeHistTOF();
  
  virtual void   AllTriggers() { allTrigs = kTRUE; }

  //  virtual void   SetPntrToHistUtil(StHistUtil *m1);

  /// the following is a ROOT macro  that is needed in all ROOT code
  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StEventQAMaker.h,v 2.14 2011/02/19 02:20:46 genevb Exp $ built "__DATE__" "__TIME__ ; return cvs;}

  ClassDef(StEventQAMaker,0)   //StAF chain virtual base class for Makers
};
    
#endif

// $Id: StEventQAMaker.h,v 2.14 2011/02/19 02:20:46 genevb Exp $
// $Log: StEventQAMaker.h,v $
// Revision 2.14  2011/02/19 02:20:46  genevb
// Pile-up cuts
//
// Revision 2.13  2009/11/19 20:34:38  genevb
// Remove Event Summary (using defunct old software monitors)
//
// Revision 2.12  2009/02/21 00:42:52  genevb
// TOF updates (BTOF classes), and single primVtx pointer
//
// Revision 2.11  2007/11/30 05:38:50  genevb
// Changes for Run8: mostly silicon removal, TOF addition
//
// Revision 2.10  2004/12/13 15:52:36  genevb
// Numerous updates: PMD, primtrk, FPD, QAShift lists
//
// Revision 2.9  2003/02/19 06:38:28  genevb
// Rework trigger and mult/event class sections
//
// Revision 2.8  2003/01/17 15:15:47  genevb
// Add AllTriggers() function to ignore trigger words
//
// Revision 2.7  2002/04/23 01:59:56  genevb
// Addition of BBC/FPD histos
//
// Revision 2.6  2002/02/10 16:48:28  jeromel
// Attempt to prevent re-creation of mHitHist.
//
// Revision 2.5  2002/02/05 22:27:30  jeromel
// Modifications from David H. Int() -> InitRun().
//
// Revision 2.4  2001/05/25 16:31:21  lansdell
// more updates to qa shift histograms
//
// Revision 2.3  2001/05/23 00:14:52  lansdell
// more changes for qa_shift histograms
//
// Revision 2.2  2001/05/16 20:57:03  lansdell
// new histograms added for qa_shift printlist; some histogram ranges changed; StMcEvent now used in StEventQA
//
// Revision 2.1  2001/04/28 22:05:13  genevb
// Added EMC histograms
//
// Revision 2.0  2000/08/25 16:02:40  genevb
// New revision: new structure, multiplicity classes
//
//
