#ifndef WFANALYZER_H
#define WFANALYZER_H

#include <vector>

class WFAnalyzer {
public:
    WFAnalyzer();
    WFAnalyzer(std::vector<unsigned short>* wf);
    virtual ~WFAnalyzer();
    
    void SetWF(std::vector<unsigned short>* wf) {fWf = wf;}
    void Reset();
    void Process();
    void ProcessBaseline();
    void ProcessQT();
    void Summarize();
    void PrintInfo();

    enum SETTING {
        PULSE_THRESH = 5, // threshold for register a pulse and for tdc start 
        LOW_PEAK_PERCENT = 15,  // for PSD,  percentage of the peak adc
        HIGH_PEAK_PERCENT = 85,  // for PSD, percentage of the peak adc
    };

    std::vector<unsigned short>* fWf;

    float baseline;
    int nPulse;
    std::vector<float> charges_integral;
    std::vector<float> charges_peak;
    std::vector<float> tdcs_start;
    std::vector<float> tdcs_end;
    std::vector<float> tdcs_thresh;
    std::vector<float> tdcs_peak;
    std::vector<float> tdcs_prepeak_low;
    std::vector<float> tdcs_prepeak_high;
    std::vector<float> tdcs_postpeak_low;
    std::vector<float> tdcs_postpeak_high;

    int *ordered_index; // index of the sorted charges_integral, from high to low

    // convenient methods for calculated pulse shape variables, return values all in tdc.
    float prepeakWidth(int i) { return tdcs_prepeak_high[i] - tdcs_prepeak_low[i]; }
    float postpeakWidth(int i) { return tdcs_postpeak_low[i] - tdcs_postpeak_high[i]; }
    float peakWidth(int i) { return tdcs_postpeak_high[i] - tdcs_prepeak_high[i]; }
    float headWidth(int i) { return tdcs_prepeak_low[i] - tdcs_start[i]; }
    float tailWidth(int i) { return tdcs_end[i] - tdcs_postpeak_low[i]; }

    // summarized info for this waveform (all pulses)
    float maxCharge;
    float secondCharge;
    float maxPeak;
    float totalCharge;
    float firstTdc; // thresh tdc of the first pulse
    float peakTdc;  // thresh tdc of the max charge pulse
    float secondPeakTdc;  // thresh tdc of the second largest charge pulse

    int wf_size;

    // interpolating between two adjacent array values
    float xArrayInterpLinear(int index, std::vector<float>& xArray, float thresh) {
        if (fabs(xArray[index]-xArray[index+1])<0.1) return index;
        else return (thresh-xArray[index]) / (xArray[index+1]-xArray[index]) + index;
    }

};

#endif