 /**************************************************************************
 * Class      : St_spa_maker.cxx
 **************************************************************************
 * $Id: St_spa_Maker.cxx,v 1.9 2006/10/16 16:36:08 bouchet Exp $
 *
 * $Log: St_spa_Maker.cxx,v $
 * Revision 1.9  2006/10/16 16:36:08  bouchet
 * Unify classes : Remove StSlsStrip, StSlsPoint, StSpaStrip, StSpaNoise by the same classes used in StSsdPointMaker (StSsdStrip,StSsdPoint) ; The methods for these classes are in StSsdUtil
 *
 * Revision 1.8  2006/09/15 21:09:52  bouchet
 * read the noise and pedestal from ssdStripCalib
 *
 * Revision 1.7  2005/11/22 03:56:46  bouchet
 * id_mctrack is using for setIdTruth
 *
 * Revision 1.6  2005/05/13 08:39:33  lmartin
 * CVS tags added
 *
 * Revision 1.5  2003/10/08 03:46:34  suire
 * *** empty log message ***
 *
 * Revision 1.3  2002/03/25 20:06:44  suire
 * Doxygen documentation, cleaning
 *
 *
 **************************************************************************/
#include <Stiostream.h>
#include <stdlib.h>
#include "St_spa_Maker.h"
#include "StChain.h"
#include "TDataSetIter.h"
//#include "svt/St_spa_am_Module.h"
#include "TFile.h"
#include "StMessMgr.h"

#include "StSsdUtil/StSsdBarrel.hh"
#include "tables/St_spa_strip_Table.h"
#include "tables/St_sls_strip_Table.h"
#include "tables/St_ssdDimensions_Table.h"
#include "tables/St_sdm_calib_par_Table.h"
#include "tables/St_slsCtrl_Table.h"
#include "tables/St_sdm_calib_db_Table.h"
#include "tables/St_ssdStripCalib_Table.h"
#include "tables/St_ssdWafersPosition_Table.h"
#include "tables/St_ssdWafersPosition_Table.h"
#include "tables/St_ssdConfiguration_Table.h"
ClassImp(St_spa_Maker)
  
//_____________________________________________________________________________
St_spa_Maker::St_spa_Maker(const char *name):
StMaker(name),
m_cond_par(0),
m_geom_par(0),
m_cal_par(0),
m_noise(0),
m_condition(0),
m_ctrl(0),
m_config(0),
mySsd(0)
{
}
//_____________________________________________________________________________
St_spa_Maker::~St_spa_Maker(){
}
//_____________________________________________________________________________
Int_t St_spa_Maker::Init(){
  
  // 		Create tables
  TDataSet *ssdparams = GetInputDB("svt/ssd");
  TDataSetIter       local(ssdparams);
  
  m_cond_par  = (St_sdm_condition_par *)local("sdm_condition_par");
  m_cal_par   = (St_sdm_calib_par     *)local("sdm_calib_par");
  //m_noise     = (St_sdm_calib_db      *)local("sdm_calib_db");
  m_condition = (St_sdm_condition_db  *)local("sdm_condition_db");
  
  if (!m_cond_par) {
    gMessMgr->Error() << "No  access to condition parameters" << endm;
  }   
  if (!m_cal_par) {
    gMessMgr->Error() << "No  access to calibration parameters" << endm;
  }   
  return StMaker::Init();
}
//_____________________________________________________________________________
Int_t St_spa_Maker::InitRun(Int_t runnumber){
  TDataSet *CalibDbConnector = GetDataBase("Calibrations/ssd");
  if (!CalibDbConnector) {
    gMessMgr->Error()<<"InitRun: Can not found the calibration db.."<<endm;
    return kStFATAL;
  }
  else{
    m_noise = (St_ssdStripCalib*) CalibDbConnector->Find("ssdStripCalib");
    if (! m_noise) return kStFATAL;
  }
  TDataSet *ssdparams = GetInputDB("Geometry/ssd");
  if (! ssdparams) {
    gMessMgr->Error() << "No  access to Geometry/ssd parameters" << endm;
    return kStErr;
  }
  TDataSetIter    local(ssdparams);
  m_ctrl        = (St_slsCtrl           *)local("slsCtrl");
  if (!m_ctrl) {
    gMessMgr->Error() << "No  access to control parameters" << endm;
    return kStFatal;
  } 
  m_geom_par    = (St_ssdDimensions     *)local("ssdDimensions");
  if (!m_geom_par) {
    gMessMgr->Error() << "No  access to geometry parameters" << endm;
    return kStFatal;
  }   
  St_ssdConfiguration* configTable = (St_ssdConfiguration*) local("ssdConfiguration");
  if (!configTable) {
    gMessMgr->Error() << "InitRun : No access to ssdConfiguration database" << endm;
    return kStFatal;
  }
  //mConfig = new StSsdConfig();
  m_config = (ssdConfiguration_st*) configTable->GetTable() ; 

  return kStOK;
}
//_____________________________________________________________________________
Int_t St_spa_Maker::Make()
{
  if (Debug())  gMessMgr->Debug() << "In St_spa_Maker::Make() ... "
                               << GetName() << endm;
  // 		Create output tables
  Int_t res = 0; 
  
  St_sls_strip *sls_strip = (St_sls_strip *)GetDataSet("sls_strip/.data/sls_strip");
  
  St_spa_strip *spa_strip = new St_spa_strip("spa_strip",40000);
  m_DataSet->Add(spa_strip);
  
  ssdDimensions_st  *geom_par =  m_geom_par->GetTable();
  slsCtrl_st      *ctrl     =  m_ctrl->GetTable(); 

  cout<<"#################################################"<<endl;
  cout<<"####    START OF SSD PEDESTAL ANNIHILATOR    ####"<<endl;
  cout<<"####        SSD BARREL INITIALIZATION        ####"<<endl;
  mySsd = new StSsdBarrel(geom_par, m_config);
  mySsd->readStripFromTable(sls_strip);
  cout<<"####        NUMBER OF SLS STRIPS "<<sls_strip->GetNRows()<<"       ####"<<endl;
  mySsd->readNoiseFromTable(m_noise);
  cout<<"####       NUMBER OF DB ENTRIES "<<m_noise->GetNRows()<<"       ####"<<endl;
  mySsd->readConditionDbFromTable(m_condition);
  cout<<"####             ADD SPA NOISE               ####"<<endl;
  mySsd->addNoiseToStrip(ctrl);
  cout<<"####           DO DAQ SIMULATION             ####"<<endl;
  mySsd->doDaqSimulation(ctrl);
  Int_t nSsdStrips = mySsd->writeStripToTable(spa_strip,sls_strip);
  //Int_t nSsdStrips = mySsd->writeStripToTable(spa_strip);
  spa_strip->Purge();
  cout<<"####       NUMBER OF SPA STRIP "<<nSsdStrips<<"          ####"<<endl;
  delete mySsd;
  cout<<"#################################################"<<endl;
  if (nSsdStrips)  res =  kStOK;

  if(res!=kStOK){
    gMessMgr->Warning("St_spa_Maker: no output");
    return kStWarn;
  }
  
  return kStOK;
}
//_____________________________________________________________________________
void St_spa_Maker::PrintInfo()
{
  if (Debug()) StMaker::PrintInfo();
}
//_____________________________________________________________________________
Int_t St_spa_Maker::Finish() {
  if (Debug()) gMessMgr->Debug() << "In St_spa_Maker::Finish() ... "
                               << GetName() << endm; 
  return kStOK;
}

