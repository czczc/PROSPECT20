// Time and Charge Statistics Info Tree
// chao@bnl.gov
// "/Event/TQStats"

#ifndef _TQSTATSTREE_
#define _TQSTATSTREE_

#define MAXPMT 2
#define MAXMUONPADDLE 1

#include <vector>
#include <map>
#include "TString.h"

class TFile;
class TTree;
class DAQEvent;

class TQStatsTree {
public:
    TQStatsTree(const char* in="", const char* out="tqstatstree.root", int limit=-1);
    virtual ~TQStatsTree();

    int fLimitEvents;
    TString fInputFileName;
    TString fOutputFileName;
    TFile* fOutputFile;
    TString treeName;
    TString treeDirName;
    TTree* fTQStatsTree;
    DAQEvent* event;

    void InitOutput();
    void Generate();
    void Reset();
    void Write();

    int eventNo;
    double trigTime; // The trigger time offset from start of run

    int nPMT;         // number of hit PMTs
    int nPulse;       // number of total pulses
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

    bool isMuonPaddleHit[MAXMUONPADDLE];
    bool isMuonPaddleAllHit;
    bool isMuon;
};

#endif 
