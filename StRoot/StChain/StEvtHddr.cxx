#include "StEvtHddr.h"
#include <stdio.h>
#include <time.h>
ClassImp(StEvtHddr)
//_____________________________________________________________________________
StEvtHddr::StEvtHddr(St_DataSet *parent):St_DataSet("EvtHddr",parent)
{ 
  SetDateTime(20330101,0);
  SetEventType("NONE");
  memset(&mRunNumber,0,(char*)&mEventNumber-(char*)&mRunNumber); 
  mRunNumber=-1;mOldRunNumber=-1;mEventNumber=-1;
}  
//_____________________________________________________________________________
StEvtHddr &StEvtHddr::operator=(const StEvtHddr &hddr)
{
  SetParent(0);
  SetName(hddr.GetName());
  SetTitle(hddr.GetTitle());
  memcpy(&mRunNumber,&hddr.mRunNumber,(char*)&mEventNumber-(char*)&mRunNumber); 
  mEventTime = hddr.mEventTime;
  mProdTime  = hddr.mProdTime;
  mEventType = hddr.mEventType;
  return *this;
}
//_____________________________________________________________________________
  void StEvtHddr::SetGMTime(ULong_t ut)
{  
   struct tm *tp;
   tp            = (tm*)gmtime((time_t*)&ut);
   UInt_t year   = tp->tm_year;
   UInt_t month  = tp->tm_mon + 1;
   UInt_t day    = tp->tm_mday;
   UInt_t hour   = tp->tm_hour;
   UInt_t min    = tp->tm_min;
   UInt_t sec    = tp->tm_sec;
   mEventTime.Set(year,month,day,hour,min,sec);
}
//_____________________________________________________________________________
  void StEvtHddr::Print()
{
  printf("\n *********** Event Info **********************\n");  
  printf("\tRun: \t%5d  Event %5d  TimeStamp %8d.%6d  Bunch %lu\n"
         ,mRunNumber,mEventNumber,mEventTime.GetDate(),mEventTime.GetTime()
         ,mBunchCrossingNumber);  
  printf("\tInpTrig: %4lx OutTrig: %4lx  \n"
         ,mInputTriggerMask,mTriggerMask);
  printf("\n *********************************************\n\n");  
}  
