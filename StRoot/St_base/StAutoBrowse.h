#if 0 //Obsolete in Root6
#ifndef _STAUTOBROWSE_
#define _STAUTOBROWSE_ 2000
class TObject;

class StAutoBrowse  
{
protected:
 StAutoBrowse(){};
~StAutoBrowse(){};
public:
static Int_t Browse(TObject *obj, TBrowser *browser);
};
#endif //  _STAUTOBROWSE_
#endif //Root6
