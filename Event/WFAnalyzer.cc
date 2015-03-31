#include "WFAnalyzer.h"
#include "DAQEvent.h"

#include "TMath.h"
#include "TH1F.h"

#include <vector>
#include <iostream>
using namespace std;

// ------------------------------------
WFAnalyzer::WFAnalyzer():
fWf(0),
ordered_index(0)
{
}

// ------------------------------------
WFAnalyzer::WFAnalyzer(std::vector<unsigned short>* wf):
fWf(wf),
ordered_index(0)
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
    Summarize();
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

    if (ordered_index) {
        delete ordered_index;
        ordered_index = 0;
    }

    maxCharge = 0;
    secondCharge = 0;
    maxPeak = 0;
    totalCharge = 0;
    firstTdc = 0; // thresh tdc of the first pulse
    peakTdc = 0;  // thresh tdc of the max charge pulse
    secondPeakTdc = 0;  // thresh tdc of the second largest charge pulse
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
    vector<float> cleanTrace(wf_size);
    for (int i=0; i<wf_size; i++) {
        cleanTrace[i] = baseline - (*fWf)[i];
        // cout << cleanTrace[i] << " ";
    }
    // cout << endl;

    // calculate pulse with pulse finding of continuous area


    float charge = 0;
    float peak = 0;
    float tdc = 0;
    float tdc_start = 0;
    float tdc_end = 0;
    float tdc_peak = 0;

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
            if(i>0 && tdc<0.1 && cleanTrace[i-1]<PULSE_THRESH && cleanTrace[i]>=PULSE_THRESH) {
                // tdc = i-1;
                tdc = xArrayInterpLinear(i-1, cleanTrace, PULSE_THRESH);
            }
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

                float low_adc  = peak * LOW_PEAK_PERCENT / 100.;
                float high_adc = peak * HIGH_PEAK_PERCENT / 100.;

                int j = tdc_peak;
                while (cleanTrace[j] > high_adc && j>tdc_start) j--;
                // tdcs_prepeak_high.push_back(j);
                tdcs_prepeak_high.push_back( xArrayInterpLinear(j, cleanTrace, high_adc) );

                while (cleanTrace[j] > low_adc && j>tdc_start) j--;
                // tdcs_prepeak_low.push_back(j);
                tdcs_prepeak_low.push_back( xArrayInterpLinear(j, cleanTrace, low_adc) );

                j = tdc_peak;
                while (cleanTrace[j] > high_adc && j<tdc_end) j++;
                // tdcs_postpeak_high.push_back(j);
                tdcs_postpeak_high.push_back( xArrayInterpLinear(j-1, cleanTrace, high_adc) );

                while (cleanTrace[j] > low_adc && j<tdc_end) j++;
                // tdcs_postpeak_low.push_back(j);
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

    ordered_index = new int[nPulse];
    TMath::Sort(nPulse, &(charges_integral[0]), ordered_index);

}

// ------------------------------------
void WFAnalyzer::PrintInfo()
{
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
    cout << "ordered: ";
    for (int i=0; i<nPulse; i++) {
        cout << ordered_index[i] << ", ";
    }
    cout << endl;

}

// ------------------------------------
void WFAnalyzer::Summarize()
{
    if (nPulse==0) return;
    maxCharge = charges_integral[ordered_index[0]];
    firstTdc = tdcs_thresh[0];
    peakTdc = tdcs_thresh[ordered_index[0]];
    if (nPulse>1) {
        secondCharge = charges_integral[ordered_index[1]];
        secondPeakTdc = tdcs_thresh[ordered_index[1]];
    }
    for (int i=0; i<nPulse; i++) {
        totalCharge += charges_integral[i];
        if (charges_peak[i] > maxPeak) {
            maxPeak = charges_peak[i];
        }
    }
}

