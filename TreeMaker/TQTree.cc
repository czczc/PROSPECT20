#include "TQTree.h"
#include "DAQEvent.h"
#include "WFAnalyzer.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TString.h"

using namespace std;

TQTree::TQTree(const char* in, const char* out, int limit)
{   
    fInputFileName = in;
    fOutputFileName = out;
    fLimitEvents = limit; // read all events
    event = new DAQEvent(in);

    charges_integral   = new vector<float>;
    adcs_peak          = new vector<float>;
    charges_prepeak    = new vector<float>;
    charges_postpeak   = new vector<float>;
    charges_tail       = new vector<float>; 
    tdcs_start         = new vector<float>;
    tdcs_end           = new vector<float>;
    tdcs_thresh        = new vector<float>;
    tdcs_peak          = new vector<float>;
    tdcs_prepeak_low   = new vector<float>;
    tdcs_prepeak_high  = new vector<float>;
    tdcs_postpeak_low  = new vector<float>;
    tdcs_postpeak_high = new vector<float>;
    ordered_index      = new vector<unsigned short>;


    TDirectory* tmpDir = gDirectory;
    fOutputFile = new TFile(fOutputFileName.Data(), "recreate");
    if (!fOutputFile->IsOpen()) {
      cout << "Output file " << fOutputFileName << " cannot be opened." << endl;
      exit(1);
    }
    treeDirName = Form("Event/TQ");
    fOutputFile->mkdir(treeDirName.Data());
    gDirectory = tmpDir;

}

TQTree::~TQTree()
{

}

void TQTree::InitOutput(int pmtId)
{

    if (pmtId<1 || pmtId>2) { cout << "illegal pmt id " << pmtId << endl; exit(1); }

    TDirectory* tmpDir = gDirectory;

    treeName = Form("PMT%d", pmtId);
    fOutputFile->cd(treeDirName.Data());
    fTQTree = new TTree(treeName.Data(), "TQ Information");

    fTQTree->Branch("eventNo", &eventNo, "eventNo/i");
    fTQTree->Branch("nPulse", &nPulse, "nPulse/i");
    fTQTree->Branch("baseline", &baseline, "baseline/F");
    fTQTree->Branch("maxAdc", &maxAdc, "maxAdc/F");
    fTQTree->Branch("secondCharge", &secondCharge, "secondCharge/F");
    fTQTree->Branch("totalCharge", &totalCharge, "totalCharge/F");
    fTQTree->Branch("firstTdc", &firstTdc, "firstTdc/F");
    fTQTree->Branch("peakTdc", &peakTdc, "peakTdc/F");
    fTQTree->Branch("secondPeakTdc", &secondPeakTdc, "secondPeakTdc/F");
    
    fTQTree->Branch("charges_integral", &charges_integral);
    fTQTree->Branch("adcs_peak", &adcs_peak);
    fTQTree->Branch("charges_prepeak", &charges_prepeak);
    fTQTree->Branch("charges_postpeak", &charges_postpeak);
    fTQTree->Branch("charges_tail", &charges_tail);
    fTQTree->Branch("tdcs_start", &tdcs_start);
    fTQTree->Branch("tdcs_end", &tdcs_end);
    fTQTree->Branch("tdcs_thresh", &tdcs_thresh);
    fTQTree->Branch("tdcs_peak", &tdcs_peak);
    fTQTree->Branch("tdcs_prepeak_low", &tdcs_prepeak_low);
    fTQTree->Branch("tdcs_prepeak_high", &tdcs_prepeak_high);
    fTQTree->Branch("tdcs_postpeak_low", &tdcs_postpeak_low);
    fTQTree->Branch("tdcs_postpeak_high", &tdcs_postpeak_high);
    fTQTree->Branch("ordered_index", &ordered_index);

    gDirectory = tmpDir;

}


void TQTree::Reset()
{
    // Make sure vectors are empty initially.

    charges_integral->clear();  
    adcs_peak->clear();  
    charges_prepeak->clear();
    charges_postpeak->clear();
    charges_tail->clear();    
    tdcs_start->clear();        
    tdcs_end->clear();          
    tdcs_thresh->clear();       
    tdcs_peak->clear();         
    tdcs_prepeak_low->clear();  
    tdcs_prepeak_high->clear(); 
    tdcs_postpeak_low->clear(); 
    tdcs_postpeak_high->clear();
    ordered_index->clear();
    
}


void TQTree::Generate(int pmtId)
{
    InitOutput(pmtId);

    WFAnalyzer wfa;
    for (int i=0; i<event->nEvents && i!=fLimitEvents; i++) {
        if (i>0  && i % 100000 == 0) cout << "Processing Event " << i << " ... " << endl;
        Reset();

        event->GetEntry(i);
        eventNo = event->eventNo;
        if (pmtId == 1) wfa.SetWF(event->channelData0);
        else if (pmtId == 2) wfa.SetWF(event->channelData1);
        else { cout << "illegal pmt id " << pmtId << endl; exit(1); }
        
        wfa.Process();

        nPulse = wfa.nPulse;
        baseline = wfa.baseline;
        maxAdc = wfa.maxAdc;
        secondCharge = wfa.secondCharge;
        totalCharge = wfa.totalCharge;
        firstTdc = wfa.firstTdc;
        peakTdc = wfa.peakTdc;
        secondPeakTdc = wfa.secondPeakTdc;

        for (int j=0; j<nPulse; j++) {
            charges_integral->push_back(wfa.charges_integral[j]);
            adcs_peak->push_back(wfa.adcs_peak[j]);
            
            charges_prepeak->push_back(wfa.charges_prepeak[j]);
            charges_postpeak->push_back(wfa.charges_postpeak[j]);
            charges_tail->push_back(wfa.charges_tail[j]);

            tdcs_start->push_back(wfa.tdcs_start[j]);
            tdcs_end->push_back(wfa.tdcs_end[j]);
            tdcs_thresh->push_back(wfa.tdcs_thresh[j]);
            tdcs_peak->push_back(wfa.tdcs_peak[j]);
            tdcs_prepeak_low->push_back(wfa.tdcs_prepeak_low[j]);
            tdcs_prepeak_high->push_back(wfa.tdcs_prepeak_high[j]);
            tdcs_postpeak_low->push_back(wfa.tdcs_postpeak_low[j]);
            tdcs_postpeak_high->push_back(wfa.tdcs_postpeak_high[j]);

            ordered_index->push_back(wfa.ordered_index[j]);
        } // loop over pulses

        fTQTree->Fill();

    } // loop over events;

    cout << "Finished processing " << fTQTree->GetEntries() << " events for PMT " << pmtId << endl;

    // Write fEventTree to fEventTree
    TDirectory* tmpDir = gDirectory;
    fOutputFile->cd(treeDirName.Data());
    fTQTree->Write("", TObject::kOverwrite);
    gDirectory = tmpDir;
    delete fTQTree;

}

void TQTree::Write()
{
    Generate(1);
    Generate(2);

    fOutputFile->Close();
}
