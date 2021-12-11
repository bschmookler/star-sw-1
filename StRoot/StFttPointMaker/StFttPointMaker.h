/***************************************************************************
 *
 * StFttPointMaker.h
 *
 * Author: jdb 2021
 ***************************************************************************
 *
 * Description: StFttPointMaker - class to fill the points in StEvent
 *
 ***************************************************************************/
#ifndef STFTTPOINTMAKER_H
#define STFTTPOINTMAKER_H
#include "StMaker.h"
#include <vector>
#include <map>



class StFttDb;
class StEvent;
class StFttCollection;
class StFttPoint;

class StFttPointMaker: public StMaker {

public:
    StFttPointMaker( const char* name = "stgcPoint" );

    ~StFttPointMaker();


    Int_t  Init();
    Int_t  InitRun( Int_t );
    Int_t  FinishRun( Int_t );
    Int_t  Finish();
    Int_t  Make();

private:
    void InjectTestData();
    void MakeLocalPoints();
    void MakeGlobalPoints();
    
    StEvent*             mEvent;
    StFttCollection*     mFttCollection;
    Bool_t               mDebug;
    Bool_t               mUseTestData;
    StFttDb*             mFttDb;

    ClassDef( StFttPointMaker, 1 )
};

#endif // STFTTPOINTMAKER_H