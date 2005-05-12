/***************************************************************************
 *
 * $Id: StMcFstHit.cc,v 2.2 2005/05/12 22:48:38 potekhin Exp $
 * $Log: StMcFstHit.cc,v $
 * Revision 2.2  2005/05/12 22:48:38  potekhin
 * Layer, plane and module accessors from Mirko
 *
 * Revision 2.1  2005/04/18 20:11:33  calderon
 * Addition of Fgt and Fst files.  Modified other files to accomodate changes.
 *
 *
 **************************************************************************/
#include "StThreeVectorF.hh"

#include "StMcFstHit.hh"
#include "tables/St_g2t_fst_hit_Table.h" 

static const char rcsid[] = "$Id: StMcFstHit.cc,v 2.2 2005/05/12 22:48:38 potekhin Exp $";

ClassImp(StMcFstHit)
    
StMemoryPool StMcFstHit::mPool(sizeof(StMcFstHit));

StMcFstHit::StMcFstHit(const StThreeVectorF& x,const StThreeVectorF& p,
			 const float de, const float ds, const long key,
			 const long id,
			 StMcTrack* parent)  : StMcHit(x, p, de, ds, key, id, parent)
{ /* noop */ }

StMcFstHit::StMcFstHit(g2t_fst_hit_st* pt)
: StMcHit(StThreeVectorF(pt->x[0], pt->x[1], pt->x[2]),
	  StThreeVectorF(pt->p[0], pt->p[1], pt->p[2]),
	  pt->de,
	  pt->ds,
	  pt->id,
	  pt->volume_id,
	  0)
{/* noop */ }

StMcFstHit::~StMcFstHit() {/* noop */ }

ostream&  operator<<(ostream& os, const StMcFstHit& h)
{
    os << "Position      : " << h.position() << endl; 
    os << "Local Momentum: " << h.localMomentum() << endl; 
    os << "Layer         : " << h.layer()    << endl;
    return os;
}

unsigned long
StMcFstHit::layer() const
{    
  //  unsigned long iModule = mVolumeId/1000000;
  unsigned long iLayer = 0;
  iLayer=(mVolumeId/100)%10;
 cout << "VolId=" << mVolumeId  << endl;
 cout << "iLayer=" << iLayer  << endl;
  return iLayer;
}
unsigned long
StMcFstHit::plane() const
{    
  //  unsigned long iModule = mVolumeId/1000000;
  unsigned long iPlane = 0;
  iPlane=(mVolumeId/100)%10;
 cout << "VolId=" << mVolumeId  << endl;
 cout << "iPlane=" << iPlane  << endl;
  return iPlane;
}

unsigned long
StMcFstHit::module() const
{    
  //  unsigned long iModule = mVolumeId/1000000;
  unsigned long imodule = 0;
  imodule=(mVolumeId/100)%10;
#if 0 
 if (0 < iModule && iModule <=11)
    {
      iLayer = 1;
    }
  else if (iModule <=30)
    {
      iLayer = 2;
    }
  else if (iModule <=57)
    {
      iLayer = 3;
    }
  else 
    {
      cout << "StMcFstHit::layer() -E- volumeId not known!" << endl;
      iLayer = 10000; 
    }
 
#endif 
 cout << "VolId=" << mVolumeId  << endl;
 cout << "imodule=" << imodule  << endl;
  return imodule;
}

unsigned long
StMcFstHit::ladder() const
{
  //  unsigned long iModule = mVolumeId/1000000;
  unsigned long iLadder = 0;
  iLadder=(mVolumeId/1000000)%10;
#if 0
  if (0 < iModule && iModule <=11)
    {
      iLadder = iModule;
    }
  else if (iModule <=30)
    {
      iLadder = iModule - 11;
    }
  else if (iModule <=57)
    {
      iLadder = iModule - 30;
    }
  else 
    {
      cout << "StMcFstHit::ladder() -E- volumeId not known!" << endl;
      iLadder = 10000; 
    }
#endif  
  cout << "VolId=" << mVolumeId  << endl;
 cout << "iLadder=" << iLadder  << endl;
  return iLadder;
}
