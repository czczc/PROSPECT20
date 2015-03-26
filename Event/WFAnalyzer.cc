#include "WFAnalyzer.h"
#include "DAQEvent.h"

#include "TMath.h"
#include "TH1F.h"

#include <vector>
#include <iostream>
using namespace std;

// ------------------------------------
WFAnalyzer::WFAnalyzer():
fWf(0)
{
}

// ------------------------------------
WFAnalyzer::WFAnalyzer(std::vector<unsigned short>* wf):
fWf(wf)
{
}

// ------------------------------------
WFAnalyzer::~WFAnalyzer()
{
    
}

// ------------------------------------
void WFAnalyzer::Process()
{
    Reset();
    ProcessBaseline();
    ProcessQT();
}

// ------------------------------------
void WFAnalyzer::Reset()
{
    baseline = 0;
    nPulse = 0;
    charges.clear();
    peaks.clear();
    tdcs_start.clear();
    tdcs_end.clear();

    maxCharge = 0;
    riseTime = 0;
    totalCharge = 0;
}

// ------------------------------------
void WFAnalyzer::ProcessBaseline()
{
    wf_size = fWf->size(); 

    if (wf_size == 0) {
        return;
    }

    TH1F* h = new TH1F("h", "h", 5000, 0, 5000);
    for (int i=0; i<wf_size; i++) {
        h->Fill((*fWf)[i]);
    }
    baseline = h->GetMaximumBin() - 1;  // for example, 1 will be filled into bin 2, therefore there is an offset of 1


    // h->Draw();
    delete h;
}

// ------------------------------------
void WFAnalyzer::ProcessQT()
{
    vector<int> cleanTrace(wf_size);
    for (int i=0; i<wf_size; i++) {
        cleanTrace[i] = baseline - (*fWf)[i];
        // cout << cleanTrace[i] << " ";
    }
    // cout << endl;

    // calculate pulse with pulse finding of continuous area


    double charge = 0;
    double peak = 0;
    double tdc = 0;
    bool foundPulse = false;
    const int THRESHOLD = 5; // threshold for register a pulse and for tdc start 
    for (int i=0; i<wf_size-1; i++) {
        if (cleanTrace[i]>0 && cleanTrace[i+1]>0) {
            foundPulse = true;
            charge += cleanTrace[i];
            if (peak < cleanTrace[i]) peak = cleanTrace[i];
            if(i>0 && tdc<0.1 && cleanTrace[i-1]<THRESHOLD && cleanTrace[i]>=THRESHOLD) tdc = i-1;
        }
        else {
            if(foundPulse && tdc>0.1) {
                charges.push_back(charge);
                peaks.push_back(peak);
                tdcs_start.push_back(tdc);
                tdcs_end.push_back(i);
            }
            charge = 0;
            peak=0;
            tdc = 0;
            foundPulse = false;
        }
    }
    nPulse = charges.size();
    cout << "nPulse: " << nPulse << endl;
    for (int i=0; i< nPulse; i++) {
        cout << charges[i] << " at (" << tdcs_start[i]*4 << ", " << tdcs_end[i]*4 << ")" << endl;
    }
    cout << endl;

    maxCharge = 0;
    riseTime = 0;
    totalCharge = 0;
    for (int i=0; i<nPulse; i++) {
        totalCharge += charges[i];
        if (charges[i] > maxCharge) {
            maxCharge = charges[i];
            riseTime = tdcs_start[i] * 4;  // 4 ns per sample
        }
    }
}

