// $Id: StSvtHitMaker.h,v 1.11 2002/06/10 21:13:17 caines Exp $
// $Log: StSvtHitMaker.h,v $
// Revision 1.11  2002/06/10 21:13:17  caines
// Add SvtHits and a hit collection if StEvent already exists - prepartation for ittf
//
// Revision 1.10  2002/02/22 18:43:43  caines
// Add SetFileNames function
//
// Revision 1.9  2002/02/16 22:05:06  jeromel
// Marcelo's recen changes to StSvtClusterMaker (needed to be in sync with
// StDbUtilities changes)
//
// Revision 1.8  2002/01/28 23:42:14  caines
// Move to SVT database with StSvtDbMaker
//
// Revision 1.7  2001/09/22 01:07:09  caines
// Fixes now that AddData() is cleared everyevent
//
// Revision 1.6  2001/08/07 20:52:16  caines
// Implement better packing of svt hardware and charge values
//
// Revision 1.5  2001/03/22 20:46:54  caines
// Comment out some of the QA histograms
//
// Revision 1.4  2000/11/30 20:42:08  caines
// Some more evaluation and use dataase
//
// Revision 1.3  2000/08/26 20:36:28  caines
// Adjustments for StSvtCoordinateTransform calls
//
// Revision 1.2  2000/08/24 04:26:56  caines
// Printout for debugging
//
// Revision 1.1  2000/08/21 13:03:40  caines
// First version of cluster->STAR hit maker
//
//
#ifndef STAR_StSvtHit
#define STAR_StSvtHit
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StSvtHit base class                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#endif

#include "tables/St_scs_spt_Table.h"

#include "TH2.h"

class TFile;
class TNtuple;

class St_scs_spt;
class StSvtGeometry;
class StSvtHybridCollection;
class StSvtAnalysedHybridClusters;
class StSvtData;
class StSvtGeantHits;
class StEvent;
class StSvtHitCollection;
 
class StSvtHitMaker : public StMaker
{
 public: 
  StSvtHitMaker(const char *name = "SvtHit");
  StSvtHitMaker(StSvtHitMaker& svthit);
  ~StSvtHitMaker();

  virtual Int_t Init();
  virtual Int_t Make();
  virtual Int_t Finish();
  Int_t FillHistograms();
  Int_t GetSvtRawData();
  Int_t GetSvtClusterData();
  Int_t GetSvtGeometry();
  Int_t GetSvtDriftVelocity();
  void TransformIntoSpacePoint();
  void SaveIntoTable(int numOfCluster, int index);
  void SaveIntoNtuple(int numOfCluster, int index);
  void SetWriteNtuple(int iwrite){iWrite = iwrite;};
  void SetFileNames(char* name1="/dev/null", char* name2="/dev/null");
  Int_t Eval();
  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StSvtHitMaker.h,v 1.11 2002/06/10 21:13:17 caines Exp $ built "__DATE__" "__TIME__ ; return cvs;}


 protected:

  int iWrite;

  StSvtGeometry *m_geom; //!
  StSvtHybridCollection *mSvtCluColl; //!
  StSvtHybridCollection *mSvtGeantHitColl; //!
  StSvtHybridCollection *m_driftVeloc; //!

  StSvtAnalysedHybridClusters  *mSvtBigHit;  //!
  
  StSvtData *mSvtData; //!
  StSvtGeantHits *mSvtGeantHit;  //!

  StEvent* mCurrentEvent; //!
  StSvtHitCollection* msvtHitColl; //!
  
  TH2F     *m_x_vs_y;  //! x vs y of Si points
  TH2F     **m_waf_no;  //! ladder no vs z of Si hit

  TNtuple *m_ClusTuple;                       //!
  TFile   *m_hfile;                           //!

  // Evaluation histos
  TH1F *mTimeHitResolution;  //!
  TH1F *mAnodeHitResolution;  //!
  TH1F *mXHitResolution;  //!
  TH1F *mYHitResolution;  //!
  TH1F *mZHitResolution;  //!
  TH2F *mHitResolution;   //!

  char* filenameN;
  char* filenameC;

  ClassDef(StSvtHitMaker,1)   //virtual base class for Makers

};


#endif
