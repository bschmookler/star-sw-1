// $Id: St_emc_Maker.h,v 1.2 1998/12/15 22:39:52 akio Exp $
// $Log: St_emc_Maker.h,v $
// Revision 1.2  1998/12/15 22:39:52  akio
// Add emc_hit object and  adc_to_energy in here.
//
#ifndef STAR_St_emc_Maker
#define STAR_St_emc_Maker

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_emc_Maker class for <FONT COLOR="RED">EMc Calibration</FONT> dataset     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#endif
#include <TH2.h>
#include "St_ems_hits_Table.h"
#include "St_emc_pedestal_Table.h"
#include "St_emc_adcslope_Table.h"
#include "St_emc_calib_header_Table.h"
#include "St_emc_hit.h"
#include "St_emc_hits_Table.h"
#include "/afs/rhic/star/packages/SL98j/pams/emc/inc/emc_def.h"

class St_emc_Maker : public StMaker {
private:
  Bool_t drawinit; 
  Int_t  m_mode;          // mode=0/1 No/Create copy in emc_hits Table;
  void MakeHistograms();  // Filling QA Histograms
protected:
  TH2F *m_nhit;           //! 
  TH2F *m_etot;           //!
  TH2F *m_hits[MAXDET];   //!
  TH2F *m_energy[MAXDET]; //!
public: 
  St_emc_Maker(const char *name="emc_hit", const char *title="event/data/emc/hits");
  virtual ~St_emc_Maker();
  virtual Int_t Init();
  virtual Int_t Make();
  virtual void Set_mode (Int_t m = 0){m_mode = m;} // *MENU*  
  virtual void PrintInfo();
  ClassDef(St_emc_Maker, 1)   //Macro
};

#endif
