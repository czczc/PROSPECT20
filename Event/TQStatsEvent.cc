#include "TQStatsEvent.h"

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TChain.h"

using namespace std;

TQStatsEvent::TQStatsEvent(const char* dataFileName)
{

    currentEventEntry = -1; // don't load event at initialization

    eventTree = new TChain("/Event/TQStats");
    eventTree->Add(dataFileName);

    if (!eventTree) {
        cout << "TTee /Event/TQStats does NOT exist! exiting ..." << endl;
        exit(1);
    }

    nEvents = eventTree->GetEntries();
    cout << dataFileName << "TQStats: total events: " << nEvents << endl;

    InitBranchAddress();
}

//----------------------------------------------------------------
TQStatsEvent::~TQStatsEvent()
{
    delete eventTree;
}

//----------------------------------------------------------------
void TQStatsEvent::InitBranchAddress()
{
    eventTree->SetBranchAddress("eventNo", &eventNo);
    eventTree->SetBranchAddress("trigTime", &trigTime);

    eventTree->SetBranchAddress("nPMT", &nPMT);
    eventTree->SetBranchAddress("nPulse", &nPulse);
    eventTree->SetBranchAddress("chargeSum", &chargeSum);
    eventTree->SetBranchAddress("chargeDiff", &chargeDiff);
    eventTree->SetBranchAddress("maxChargeSum", &maxChargeSum);
    eventTree->SetBranchAddress("maxChargeDiff", &maxChargeDiff);
    eventTree->SetBranchAddress("peakTdc", &peakTdc);
    eventTree->SetBranchAddress("tdcDiff", &tdcDiff);

    eventTree->SetBranchAddress("charge_prepeak", &charge_prepeak);
    eventTree->SetBranchAddress("charge_postpeak", &charge_postpeak);
    eventTree->SetBranchAddress("charge_tail", &charge_tail);
    eventTree->SetBranchAddress("charge_integral", &charge_integral);

    eventTree->SetBranchAddress("width_head", &width_head);
    eventTree->SetBranchAddress("width_prepeak", &width_prepeak);
    eventTree->SetBranchAddress("width_peak", &width_peak);
    eventTree->SetBranchAddress("width_postpeak", &width_postpeak);
    eventTree->SetBranchAddress("width_tail", &width_tail);

    eventTree->SetBranchAddress("isMuonPaddleHit", &isMuonPaddleHit);
    eventTree->SetBranchAddress("isMuonPaddleAllHit", &isMuonPaddleAllHit);
    eventTree->SetBranchAddress("isMuon", &isMuon);
}

//----------------------------------------------------------------
void TQStatsEvent::Reset()
{
    // for (int i=0; i<MAXPMT; i++) {
    //     charge_prepeak[i] = 0;
    //     charge_postpeak[i] = 0;
    //     charge_tail[i] = 0;
    //     charge_integral[i] = 0;

    //     width_head[i] = 0;
    //     width_prepeak[i] = 0;
    //     width_peak[i] = 0;
    //     width_postpeak[i] = 0;
    //     width_tail[i] = 0;
    // }
    // isMuonPaddleHit[0] = false;
}

//----------------------------------------------------------------
void TQStatsEvent::GetEntry(int entry)
{
    Reset();
    eventTree->GetEntry(entry);

    ts.SetSec(int(trigTime));
    ts.SetNanoSec( int((trigTime-ts.GetSec())*1e9) );

    currentEventEntry = entry;
}

//----------------------------------------------------------------
void TQStatsEvent::PrintInfo(int level)
{
    // cout << "event: (" << eventNo << "/" << nEvents << ")" << endl;
    cout << "event " << eventNo  
         << " @ " << ts 
         << endl;
    // print extra
    if (level>0) {
    }
    cout << "-----------" << endl;
}


