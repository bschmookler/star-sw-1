/***************************************************************************
 *
 * $Id: StTrack.hh,v 1.1 1999/01/15 20:40:12 wenaus Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTrack.hh,v $
 * Revision 1.1  1999/01/15 20:40:12  wenaus
 * Commit Thomas' original code
 *
 * Revision 1.4  1999/02/12 02:01:20  wenaus
 * New track constructor to load helix params independently of table
 *
 * Revision 1.3  1999/01/30 23:03:16  wenaus
#include "StTrackPidTraits.hh"
#include "StTrackFitTraits.hh"
 * version with constructors for table-based loading
 *
 **************************************************************************/
#ifndef StTrack_hh
#define StTrack_hh 
#include "StPhysicalHelix.hh"
public:
    StTrack();
            StThreeVector<double>& origin);
    StTrack(dst_track_st* trk,
            double curvature,
            double dip,
            double phase,
            StThreeVector<double>& origin,
	    int h);
    // StTrack(const StTrack&);                     use default
    // const StTrack & operator=(const StTrack&);   use default
    
    StPhysicalHelix&  helix();
    StVertex*         startVertex();
    StVertex*         stopVertex();
    StTrackFitTraits& fitTraits();
    StTrackPidTraits& pidTraits();
    virtual StVertex*         stopVertex();
    void setHelix(const StPhysicalHelix&);
    void setStartVertex(StVertex*);
    void setStopVertex(StVertex*);
    virtual void setHelix(const StPhysicalHelix&);
    virtual void setStartVertex(StVertex*);
    virtual void setStopVertex(StVertex*);
    
protected:
    StPhysicalHelix  mHelix;
    StVertex*        mStartVertex;
    StVertex*        mStopVertex;
    StTrackFitTraits mFitTraits;
    StTrackPidTraits mPidTraits;
};

inline StPhysicalHelix& StTrack::helix() { return mHelix; }

inline StVertex* StTrack::startVertex() { return mStartVertex; }

inline StVertex* StTrack::stopVertex() { return mStopVertex; }

inline StTrackFitTraits& StTrack::fitTraits() { return mFitTraits; }

inline StTrackPidTraits& StTrack::pidTraits() { return mPidTraits; }

#endif
