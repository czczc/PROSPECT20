#include "DAQEvent.h"

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"

using namespace std;

DAQEvent::DAQEvent(const char* dataFileName)
{
    channelIds = new std::vector<int>; 
    channelData0 = new vector<unsigned short>;
    channelData1 = new vector<unsigned short>;
    channelData2 = new vector<unsigned short>;
    channelData3 = new vector<unsigned short>;
    channelData4 = new vector<unsigned short>;
    channelData5 = new vector<unsigned short>;
    channelData6 = new vector<unsigned short>;
    channelData7 = new vector<unsigned short>;

    currentEventEntry = -1; // don't load event at initialization

    rootFile = new TFile(dataFileName);
    if (rootFile->IsZombie()) {
        cout << "Data file " << dataFileName << " Does NOT exist! exiting ..." << endl;
        exit(1);
    }
    headerTree = (TTree*)rootFile->Get("/DAQ/Header");
    if (!headerTree) {
        cout << "TTee /DAQ/Header does NOT exist! exiting ..." << endl;
        exit(1);
    }

    eventTree = (TTree*)rootFile->Get("/Event/Raw");
    if (!eventTree) {
        cout << "TTee /Event/Raw does NOT exist! exiting ..." << endl;
        exit(1);
    }

    nEvents = eventTree->GetEntries();
    cout << "total events: " << nEvents << endl;

    InitBranchAddress();
}

//----------------------------------------------------------------
DAQEvent::~DAQEvent()
{
    rootFile->Close();
    delete rootFile;
}

//----------------------------------------------------------------
void DAQEvent::InitBranchAddress()
{
    eventTree->SetBranchAddress("eventNo", &eventNo);
    eventTree->SetBranchAddress("boardId", &boardId);
    eventTree->SetBranchAddress("nChannels", &nChannels);
    eventTree->SetBranchAddress("channelMask", &channelMask);
    eventTree->SetBranchAddress("channelIds", &channelIds);
    eventTree->SetBranchAddress("channelSize", &channelSize);
    
    eventTree->SetBranchAddress("channelData0", &channelData0);
    eventTree->SetBranchAddress("channelData1", &channelData1);
    eventTree->SetBranchAddress("channelData2", &channelData2);
    eventTree->SetBranchAddress("channelData3", &channelData3);
    eventTree->SetBranchAddress("channelData4", &channelData4);
    eventTree->SetBranchAddress("channelData5", &channelData5);
    eventTree->SetBranchAddress("channelData6", &channelData6);
    eventTree->SetBranchAddress("channelData7", &channelData7);

    eventTree->SetBranchAddress("trigTime", &trigTime);
}

//----------------------------------------------------------------
void DAQEvent::Reset()
{
}

//----------------------------------------------------------------
void DAQEvent::GetEntry(int entry)
{
    Reset();
    eventTree->GetEntry(entry);

    ts.SetSec(int(trigTime));
    ts.SetNanoSec( int((trigTime-ts.GetSec())*1e9) );

    currentEventEntry = entry;
}

//----------------------------------------------------------------
void DAQEvent::PrintInfo(int level)
{
    // cout << "event: (" << eventNo << "/" << nEvents << ")" << endl;
    cout << "event " << eventNo  
         << " @ " << ts 
         << endl;
    // print waveform
    if (level>0) {
        int size = channelData0->size();
        cout << size << " samples: "; 
        for (int i=0; i<size; i++) {
            cout << (*channelData0)[i] << " ";
        }
        cout << endl;
    }
    cout << "-----------" << endl;
}


