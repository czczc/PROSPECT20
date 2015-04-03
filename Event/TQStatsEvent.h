#ifndef TQSTATSEVENT_H
#define TQSTATSEVENT_H

#include <vector>
#include "TTimeStamp.h"

#define MAXPMT 2

class TTree;
class TFile;

class TQStatsEvent
{
public:
    TQStatsEvent(const char* dataFileName="");
    virtual ~TQStatsEvent();

    TTree* EventTree()   { return eventTree; }
    void InitBranchAddress();
    void GetEntry(int entry);
    void Reset();
    void PrintInfo(int level=0);

    TFile *rootFile;
    TTree *eventTree;

    unsigned int nEvents;
    int currentEventEntry;

    // eventTree branches   
    unsigned int eventNo;
    double trigTime; // The trigger time offset from start of run

    unsigned int nPMT;         // number of hit PMTs
    unsigned int nPulse;       // number of total pulses
    float chargeSum;  // charge1 + charge2
    float chargeDiff; // charge1 - charge2
    float maxChargeSum;    // maxCharge1 + maxCharge2
    float maxChargeDiff;   // maxCharge1 - maxCharge2
    float peakTdc;    // earliest tdc of the max pulse of the 2 PMT
    float tdcDiff;    // difference in tdc between the two max pulse

    // all following variables are referring to the largest pulse
    float charge_prepeak[MAXPMT];
    float charge_postpeak[MAXPMT];
    float charge_tail[MAXPMT];
    float charge_integral[MAXPMT];

    float width_head[MAXPMT];
    float width_prepeak[MAXPMT];
    float width_peak[MAXPMT];
    float width_postpeak[MAXPMT];
    float width_tail[MAXPMT];

    bool isMuonPaddleHit[1];
    bool isMuonPaddleAllHit;
    bool isMuon;

    TTimeStamp ts; 

};

#endif
