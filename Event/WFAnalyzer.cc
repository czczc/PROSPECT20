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
    charges_integral.clear();
    charges_peak.clear();
    tdcs_start.clear();
    tdcs_end.clear();
    tdcs_thresh.clear();
    tdcs_peak.clear();
    tdcs_prepeak_low.clear();
    tdcs_prepeak_high.clear();
    tdcs_postpeak_low.clear();
    tdcs_postpeak_high.clear();

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
    vector<double> cleanTrace(wf_size);
    for (int i=0; i<wf_size; i++) {
        cleanTrace[i] = baseline - (*fWf)[i];
        // cout << cleanTrace[i] << " ";
    }
    // cout << endl;

    // calculate pulse with pulse finding of continuous area


    double charge = 0;
    double peak = 0;
    double tdc = 0;
    double tdc_start = 0;
    double tdc_end = 0;
    double tdc_peak = 0;
    // double tdc_prepeak_low = 0;
    // double tdc_prepeak_high = 0;
    // double tdc_postpeak_low = 0;
    // double tdc_postpeak_high = 0;
    bool foundPulse = false;
    for (int i=1; i<wf_size-1; i++) {
        if (cleanTrace[i]>0 && cleanTrace[i+1]>0) {
            foundPulse = true;
            if (tdc_start <0.1) {
                // tdc_start = i;
                tdc_start = xArrayInterpLinear(i-1, cleanTrace, 0);
            }
            charge += cleanTrace[i];
            if (peak < cleanTrace[i]) {
                peak = cleanTrace[i];
                tdc_peak = i;
            }
            if(i>0 && tdc<0.1 && cleanTrace[i-1]<PULSE_THRESH && cleanTrace[i]>=PULSE_THRESH) tdc = i-1;
        }
        else {
            if(foundPulse && tdc>0.1) {
                charges_integral.push_back(charge);
                charges_peak.push_back(peak);
                tdcs_thresh.push_back(tdc);
                tdcs_start.push_back(tdc_start);
                // tdcs_end.push_back(i);
                tdc_end = xArrayInterpLinear(i, cleanTrace, 0);
                tdcs_end.push_back(tdc_end);
                tdcs_peak.push_back(tdc_peak);

                double low_adc  = peak * LOW_PEAK_PERCENT / 100.;
                double high_adc = peak * HIGH_PEAK_PERCENT / 100.;

                int j = tdc_peak;
                while (cleanTrace[j] > high_adc && j>tdc_start) j--;
                // tdcs_prepeak_high.push_back(j);
                // if (j<=tdc_start) tdcs_prepeak_high.push_back(tdc_start);
                tdcs_prepeak_high.push_back( xArrayInterpLinear(j, cleanTrace, high_adc) );

                while (cleanTrace[j] > low_adc && j>tdc_start) j--;
                // tdcs_prepeak_low.push_back(j);
                // if (j<=tdc_start) tdcs_prepeak_low.push_back(tdc_start);
                tdcs_prepeak_low.push_back( xArrayInterpLinear(j, cleanTrace, low_adc) );

                j = tdc_peak;
                while (cleanTrace[j] > high_adc && j<tdc_end) j++;
                // tdcs_postpeak_high.push_back(j);
                // if (j>=tdc_end) tdcs_postpeak_high.push_back(tdc_end);
                tdcs_postpeak_high.push_back( xArrayInterpLinear(j-1, cleanTrace, high_adc) );

                while (cleanTrace[j] > low_adc && j<tdc_end) j++;
                // tdcs_postpeak_low.push_back(j);
                // if (j>=tdc_end) tdcs_postpeak_low.push_back(tdc_end);
                tdcs_postpeak_low.push_back( xArrayInterpLinear(j-1, cleanTrace, low_adc) );
            }
            charge = 0;
            peak=0;
            tdc = 0;
            tdc_start = 0;
            tdc_end = 0;
            tdc_peak = 0;
            foundPulse = false;
        }
    }
    nPulse = charges_integral.size();
    cout << "nPulse: " << nPulse << endl;
    for (int i=0; i< nPulse; i++) {
        cout << charges_integral[i] << " at (" << tdcs_start[i]*4 << ", " << tdcs_end[i]*4 << ") " 
        << headWidth(i) << ", "
        << prepeakWidth(i) << ", "
        << peakWidth(i) << ", "
        << postpeakWidth(i) << ", "
        << tailWidth(i) << ", "
        << endl;
    }
    cout << endl;

    maxCharge = 0;
    riseTime = 0;
    totalCharge = 0;
    for (int i=0; i<nPulse; i++) {
        totalCharge += charges_integral[i];
        if (charges_integral[i] > maxCharge) {
            maxCharge = charges_integral[i];
            riseTime = tdcs_start[i] * 4;  // 4 ns per sample
        }
    }
}

