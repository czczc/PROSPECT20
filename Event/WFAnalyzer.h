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
    
    enum SETTING {
        PULSE_THRESH = 5, // threshold for register a pulse and for tdc start 
        LOW_PEAK_PERCENT = 15,  // for PSD,  percentage of the peak adc
        HIGH_PEAK_PERCENT = 85,  // for PSD, percentage of the peak adc
    };

    std::vector<unsigned short>* fWf;

    double baseline;
    int nPulse;
    std::vector<double> charges_integral;
    std::vector<double> charges_peak;
    std::vector<double> tdcs_start;
    std::vector<double> tdcs_end;
    std::vector<double> tdcs_thresh;
    std::vector<double> tdcs_peak;
    std::vector<double> tdcs_prepeak_low;
    std::vector<double> tdcs_prepeak_high;
    std::vector<double> tdcs_postpeak_low;
    std::vector<double> tdcs_postpeak_high;

    // convenient methods for calculated pulse shape variables, return values all in tdc.
    double prepeakWidth(int i) { return tdcs_prepeak_high[i] - tdcs_prepeak_low[i]; }
    double postpeakWidth(int i) { return tdcs_postpeak_low[i] - tdcs_postpeak_high[i]; }
    double peakWidth(int i) { return tdcs_postpeak_high[i] - tdcs_prepeak_high[i]; }
    double headWidth(int i) { return tdcs_prepeak_low[i] - tdcs_start[i]; }
    double tailWidth(int i) { return tdcs_end[i] - tdcs_postpeak_low[i]; }

    double maxCharge;
    double riseTime;
    double totalCharge;

    int wf_size;

    // interpolating between two adjacent array values
    double xArrayInterpLinear(int index, std::vector<double>& xArray, double thresh) {
        if (fabs(xArray[index]-xArray[index+1])<0.1) return index;
        else return (thresh-xArray[index]) / (xArray[index+1]-xArray[index]) + index;
    }

};

#endif