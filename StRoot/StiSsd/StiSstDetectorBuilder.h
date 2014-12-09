#ifndef StiSstDetectorBuilder_H
#define StiSstDetectorBuilder_H

#include "Sti/StiDetectorBuilder.h"
#include "StDetectorDbMaker/StiHitErrorCalculator.h"
#include "StSsdDbMaker/StSstDbMaker.h"

class ssdWafersPosition_st;
class St_ssdWafersPosition;
class StiDetector;

class StiSstDetectorBuilder : public StiDetectorBuilder
{

public:

   StiSstDetectorBuilder(bool active, bool buildIdealGeom=true);
   virtual ~StiSstDetectorBuilder();
   virtual void buildDetectors(StMaker &source);
   virtual void useVMCGeometry();

protected:

   bool          mBuildIdealGeom;
   StSstDbMaker *mSstDb;

private:

   void buildInactiveVolumes();
   void segmentSFMOVolume(StiDetector* stiSFMO);
};
#endif
