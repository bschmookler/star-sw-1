/***********************************************************************
 *
 *  StMuDebug.h,v 1.0 1999/09/07
 * Authors: Frank Laue, BNL, laue@bnl.gov
 *
 ***********************************************************************/

#ifndef StMuDebug_h
#define StMuDebug_h

#include "TObject.h"

#define FORCEDDEBUGMESSAGE(x)  cout << "##### " << __PRETTY_FUNCTION__ << "##### " << x << endl;
#define DEBUGMESSAGE(x)  if (StMuDebug::level()>0)    cout << "##### " << __PRETTY_FUNCTION__ << "##### " << x << endl;
#define DEBUGMESSAGE1(x) if (StMuDebug::level()>=1) cout << "##### " << __PRETTY_FUNCTION__ << "##### " << x << endl;
#define DEBUGMESSAGE2(x) if (StMuDebug::level()>=2) cout << "##### " << __PRETTY_FUNCTION__ << "##### " << x << endl;
#define DEBUGMESSAGE3(x) if (StMuDebug::level()>=3) cout << "##### " << __PRETTY_FUNCTION__ << "##### " << x << endl;
#define DEBUGVALUE(x)  if (StMuDebug::level()>0)    cout << "##### " << __PRETTY_FUNCTION__ << "##### " << (#x) << "=" << x << endl;
#define DEBUGVALUE1(x) if (StMuDebug::level()>=1) cout << "##### " << __PRETTY_FUNCTION__ << "##### " << (#x) << "=" << x << endl;
#define DEBUGVALUE2(x) if (StMuDebug::level()>=2) cout << "##### " << __PRETTY_FUNCTION__ << "##### " << (#x) << "=" << x << endl;
#define DEBUGVALUE3(x) if (StMuDebug::level()>=3) cout << "##### " << __PRETTY_FUNCTION__ << "##### " << (#x) << "=" << x << endl;
#define IFDEBUG(x)  if (StMuDebug::level()>0)    { DEBUGMESSAGE(""); (x);} 
#define IFDEBUG1(x) if (StMuDebug::level()>=1) { DEBUGMESSAGE(""); (x);} 
#define IFDEBUG2(x) if (StMuDebug::level()>=2) { DEBUGMESSAGE(""); (x);} 
#define IFDEBUG3(x) if (StMuDebug::level()>=3) { DEBUGMESSAGE(""); (x);} 


/** 
    @class StMuDebug 
    Helper class used to control the amount of output. 
    All datamembers and functions are static, so you can use them everywhere using 'StMuDebug::xxxxx()`
    So far I was using 3 debug levels:
    1 : typical used for io that is generated once, e.g. in the constructor or init function
    2 : typical used for io that is generated repeatedly, but in a moderate amount, e.g. printing the event number in an event loop
    3 : typical used for io that is generated very frequently, e.g. printing pT when looping over all tracks
    0 or negative debug levels means debug output switched off

    You can use the debug level as:
        if ( StMuDebug::level()>2 ) { ... } 
    However you will also find macros 'DEBUGMESSAGE(<string>)` or `IFDEBUG(<cmd>)` which I frequently use to automatically print the 
    name of actual scope

*/
class StMuDebug : public TObject{
 public:
  StMuDebug() {};
  ~StMuDebug() {};
  
  /// switch debug output on ( to last known level)
  static void on() { mDebug = absolut(mDebug); }
  /// sets the debug level
  static void setLevel(unsigned int level) { mDebug=level; }
  /// switch of debug level, but remember last 
  static void off() { mDebug = -1*absolut(mDebug); }
  /// returns debug level
  static int level() { return mDebug;}
 private:
  /// returns the absolute of the debug level
  static int absolut(int i) { return (i>0) ? i : -i; }
  /// debug level
  static int mDebug;
  ClassDef(StMuDebug,1)
};

#endif

/***********************************************************************
 *
 * $Log: StMuDebug.h,v $
 * Revision 1.3  2002/08/20 19:55:48  laue
 * Doxygen comments added
 *
 * Revision 1.2  2002/04/11 14:19:30  laue
 * - update for RH 7.2
 * - decrease default arrays sizes
 * - add data base readerfor number of events in a file
 *
 * Revision 1.1  2002/03/08 17:04:17  laue
 * initial revision
 *
 *
 ***********************************************************************/
