// Time and Charge Tree
// chao@bnl.gov
// "/Event/TQ/PMT{\d}""

#ifndef _TQTREE_
#define _TQTREE_

#include <vector>
#include <map>
#include "TString.h"

class TFile;
class TTree;
class DAQEvent;

class TQTree {
public:
    TQTree(const char* in="", const char* out="tqtree.root", int limit=-1);
    virtual ~TQTree();

    int fLimitEvents;
    TString fInputFileName;
    TString fOutputFileName;
    TFile* fOutputFile;
    TString treeName;
    TString treeDirName;
    TTree* fTQTree;
    DAQEvent* event;

    void InitOutput(int pmtId);
    void Generate(int pmtId);
    void Reset();
    void Write();

    int eventNo;
    float baseline;
    int nPulse;
    float maxCharge;
    float secondCharge;
    float maxPeak;
    float totalCharge;
    float firstTdc; // thresh tdc of the first pulse
    float peakTdc;  // thresh tdc of the max charge pulse
    float secondPeakTdc;  // thresh tdc of the second largest charge pulse

    std::vector<float>* charges_integral;
    std::vector<float>* charges_peak;
    std::vector<float>* tdcs_start;
    std::vector<float>* tdcs_end;
    std::vector<float>* tdcs_thresh;
    std::vector<float>* tdcs_peak;
    std::vector<float>* tdcs_prepeak_low;
    std::vector<float>* tdcs_prepeak_high;
    std::vector<float>* tdcs_postpeak_low;
    std::vector<float>* tdcs_postpeak_high;
    std::vector<unsigned short>* ordered_index;

};

#endif 
