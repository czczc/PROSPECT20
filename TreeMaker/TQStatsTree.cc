#include "TQStatsTree.h"
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

TQStatsTree::TQStatsTree(const char* in, const char* out, int limit)
{   
    fInputFileName = in;
    fOutputFileName = out;
    fLimitEvents = limit; // read all events
    event = new DAQEvent(in);

}

TQStatsTree::~TQStatsTree()
{

}

void TQStatsTree::InitOutput()
{
    TDirectory* tmpDir = gDirectory;
    fOutputFile = new TFile(fOutputFileName.Data(), "recreate");
    if (!fOutputFile->IsOpen()) {
      cout << "Output file " << fOutputFileName << " cannot be opened." << endl;
      exit(1);
    }
    treeDirName = Form("Event");
    fOutputFile->mkdir(treeDirName.Data());
    fOutputFile->cd(treeDirName.Data());

    treeName = Form("TQStats");
    fTQStatsTree = new TTree(treeName.Data(), "TQ Stats Information");

    fTQStatsTree->Branch("eventNo", &eventNo, "eventNo/i");
    fTQStatsTree->Branch("trigTime", &trigTime, "trigTime/D");
    fTQStatsTree->Branch("nPMT", &nPMT, "nPMT/i");
    fTQStatsTree->Branch("nPulse", &nPulse, "nPulse/i");
    fTQStatsTree->Branch("chargeSum", &chargeSum, "chargeSum/F");
    fTQStatsTree->Branch("chargeDiff", &chargeDiff, "chargeDiff/F");
    fTQStatsTree->Branch("peakSum", &peakSum, "peakSum/F");
    fTQStatsTree->Branch("peakDiff", &peakDiff, "peakDiff/F");
    fTQStatsTree->Branch("peakTdc", &peakTdc, "peakTdc/F");
    fTQStatsTree->Branch("tdcDiff", &tdcDiff, "tdcDiff/F");
    
    fTQStatsTree->Branch("width_head"    , &width_head, "width_head[2]/F");
    fTQStatsTree->Branch("width_prepeak" , &width_prepeak, "width_prepeak[2]/F");
    fTQStatsTree->Branch("width_peak"    , &width_peak, "width_peak[2]/F");
    fTQStatsTree->Branch("width_postpeak", &width_postpeak, "width_postpeak[2]/F");
    fTQStatsTree->Branch("width_tail"    , &width_tail, "width_tail[2]/F");

    fTQStatsTree->Branch("isMuonPaddleHit"   , &isMuonPaddleHit, "isMuonPaddleHit[1]/O");
    fTQStatsTree->Branch("isMuonPaddleAllHit", &isMuonPaddleAllHit, "isMuonPaddleAllHit/O");
    fTQStatsTree->Branch("isMuon", &isMuon, "isMuon/O");

    gDirectory = tmpDir;

}


void TQStatsTree::Reset()
{
    nPMT = 0;
    nPulse = 0;
    chargeSum = 0;
    chargeDiff = 0;
    peakSum = 0;
    peakDiff = 0;
    peakTdc = 0;
    tdcDiff = 0;

    // Make sure vectors are empty initially.
    for (int i=0; i<MAXPMT; i++) {
        width_head[i] = 0;
        width_prepeak[i] = 0;
        width_peak[i] = 0;
        width_postpeak[i] = 0;
        width_tail[i] = 0;
    }

    for (int i=0; i<MAXMUONPADDLE; i++) {
        isMuonPaddleHit[i] = false;
    }
    isMuonPaddleAllHit = false;
    isMuon = false;
}


void TQStatsTree::Generate()
{
    InitOutput();
    WFAnalyzer wfa[2];
    for (int i=0; i<event->nEvents && i!=fLimitEvents; i++) {
        if (i>0  && i % 100000 == 0) cout << "Processing Event " << i << " ... " << endl;
        Reset();

        event->GetEntry(i);
        eventNo = event->eventNo;
        trigTime = event->trigTime;

        isMuonPaddleHit[0] = (event->channelData5->size() > 0);
        isMuonPaddleAllHit = (event->channelData6->size() > 0);
        isMuon = (isMuonPaddleAllHit || isMuonPaddleHit[0]);

        wfa[0].SetWF(event->channelData0);
        wfa[1].SetWF(event->channelData1);
        
        for (int k=0; k<MAXPMT; k++) { wfa[k].Process(); }

        chargeDiff = wfa[0].totalCharge - wfa[1].totalCharge;
        peakDiff = wfa[0].maxCharge - wfa[1].maxCharge;
        tdcDiff = wfa[0].peakTdc - wfa[1].peakTdc;

        for (int k=0; k<MAXPMT; k++) {
            WFAnalyzer *w = &wfa[k];
            if (w->nPulse == 0) continue;

            int id = w->ordered_index[0];

            width_head[k] = w->tdcs_prepeak_low[id] - w->tdcs_start[id];
            width_prepeak[k] = w->tdcs_prepeak_high[id] - w->tdcs_prepeak_low[id];
            width_peak[k] = w->tdcs_postpeak_high[id] - w->tdcs_prepeak_high[id];
            width_postpeak[k] = w->tdcs_postpeak_low[id] - w->tdcs_postpeak_high[id];;
            width_tail[k] = w->tdcs_end[id] - w->tdcs_postpeak_low[id];

            nPMT++;
            nPulse += w->nPulse;
            chargeSum += w->totalCharge;
            peakSum += w->maxCharge;

            double tdc = w->peakTdc;
            if (peakTdc<0.1) { peakTdc = tdc; }
            else if (tdc < peakTdc) { peakTdc = tdc; }

        }

        fTQStatsTree->Fill();

    } // loop over events;

    cout << "Finished processing " << fTQStatsTree->GetEntries() << " events" << endl;

    // Write fEventTree to fEventTree
    TDirectory* tmpDir = gDirectory;
    fOutputFile->cd(treeDirName.Data());
    fTQStatsTree->Write("", TObject::kOverwrite);
    gDirectory = tmpDir;
    delete fTQStatsTree;

}

void TQStatsTree::Write()
{
    Generate();

    fOutputFile->Close();
}
