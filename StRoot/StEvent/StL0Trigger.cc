/***************************************************************************
 *
 * $Id: StL0Trigger.cc,v 1.1 1999/01/15 20:39:51 wenaus Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StL0Trigger.cc,v $
 * Revision 1.1  1999/01/15 20:39:51  wenaus
 * Commit Thomas' original code
 *
 **************************************************************************/
#include "StL0Trigger.hh"

StL0Trigger::StL0Trigger()
{
    mMwcCtbMultiplicity = 0;
    mMwcCtbDipole = 0;
    mMwcCtbTopology = 0;
    mMwcCtbMoment = 0;

}

StL0Trigger::~StL0Trigger() { /* noop */ }

void StL0Trigger::setMwcCtbMultiplicity(long val) { mMwcCtbMultiplicity = val; } 

void StL0Trigger::setMwcCtbDipole(long val) { mMwcCtbDipole = val; }            

void StL0Trigger::setMwcCtbTopology(long val) { mMwcCtbTopology = val; } 

void StL0Trigger::setMwcCtbMoment(long val) { mMwcCtbMoment = val; }
