// $Id: StScmPoint.hh,v 1.3 2005/12/19 10:52:13 kisiel Exp $
//
// $Log: StScmPoint.hh,v $
// Revision 1.3  2005/12/19 10:52:13  kisiel
// Properly encode Cluster Size and Mean strip into the hardware information for the SSDHit
//
// Revision 1.2  2005/05/17 14:16:36  lmartin
// CVS tags added
//
#ifndef STSCMPOINT_HH
#define STSCMPOINT_HH
#include <Stiostream.h>

class StScmPoint
{
 public:
  StScmPoint(int rNPoint, int rNWafer, int rNumPackage, int rKindPackage);
  StScmPoint(const StScmPoint & originalPoint);
  ~StScmPoint();

  StScmPoint& operator=(const StScmPoint originalPoint);

  void        setFlag(int rFlag);
  void        setNPoint(int rNPoint);
  void        setNCluster(int rNCluster);
  void        setNMatched(int rNMatched);
  void        setNMchit(int rNMchit, int iR);
  void        setNWafer(int rNWafer);
  void        setEnergyLoss(float adcP, float adcN);
  void        setDe(float rEnergyLoss, int iR);
  void        setPositionU(float rPositionU, int iR);
  void        setXg(float rXg, int iR);
  void        setXl(float rXl, int iR);
  void        setIdClusterP(int iIdClusterP);
  void        setIdClusterN(int iIdClusterN);

  void        setPrevPoint(StScmPoint *rPrevPoint);
  void        setNextPoint(StScmPoint *rNextPoint);

  int         getFlag();
  int         getNPoint();
  int         getNCluster();
  int         getNMatched();
  int         getNMchit(int iR);
  int         getNWafer();
  float       getDe(int iR);
  float       getPositionU(int iR);
  float       getXg(int iR);
  float       getXl(int iR);
  int         getIdClusterP();
  int         getIdClusterN();

  StScmPoint* getPrevPoint();
  StScmPoint* getNextPoint();  

  StScmPoint* giveCopy();
  
 private:

  int         mFlag;
  int         mNPoint;
  int         mNCluster;
  int         mNMatched;
  int        *mNMchit;
  int         mNWafer;
  float      *mDe;
  float      *mPositionU;
  float      *mXg;
  float      *mXl;
  int         mIdClusterP;
  int         mIdClusterN;

  StScmPoint *mPrevPoint;
  StScmPoint *mNextPoint;
};

#endif
