// -*- mode: c++;-*-
// $Id: StjTPCTrackMaker.h,v 1.4.8.1 2016/05/23 18:33:15 jeromel Exp $
#ifndef STJTPCTRACKMAKER_H
#define STJTPCTRACKMAKER_H

#include "StMaker.h"
#include <Rtypes.h>

class StjTrackListWriter;

class TDirectory;
class TTree;

class StMuDstMaker;

class StjTPC;

class StjTPCTrackMaker : public StMaker {

public:

  StjTPCTrackMaker(const Char_t *name, TDirectory* file, StMuDstMaker* uDstMaker);
  virtual ~StjTPCTrackMaker() { }

  Int_t Init();
  Int_t Make();
  Int_t Finish();
    
  const char* GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StjTPCTrackMaker.h,v 1.4.8.1 2016/05/23 18:33:15 jeromel Exp $ built " __DATE__ " " __TIME__; return cvs;}

private:

  TDirectory* _file;

  StMuDstMaker* _uDstMaker;

  StjTPC*  _tpc;

  StjTrackListWriter* _writer;

  ClassDef(StjTPCTrackMaker, 0)

};

#endif // STJTPCTRACKMAKER_H
