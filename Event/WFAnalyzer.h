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
    
    std::vector<unsigned short>* fWf;

    double baseline;
    int nPulse;
    vector<double> charges;
    vector<double> peaks;
    vector<double> tdcs_start;
    vector<double> tdcs_end;


    double maxCharge;
    double riseTime;
    double totalCharge;

    int wf_size;

};

#endif