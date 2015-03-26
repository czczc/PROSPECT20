#ifndef MCEVENT_H
#define MCEVENT_H

#include <vector>
#include "TTimeStamp.h"

class TTree;
class TFile;

class DAQEvent
{
public:
    DAQEvent(const char* dataFileName="");
    virtual ~DAQEvent();

    TTree* EventTree()   { return eventTree; }
    TTree* HeaderTree() { return headerTree; }
    void InitBranchAddress();
    void GetEntry(int entry);
    void Reset();
    void PrintInfo(int level=0);

    TFile *rootFile;
    TTree *headerTree;
    TTree *eventTree;

    enum LIMITS {
        MAX_SAMPLE = 300,
    };

    int nEvents;
    int currentEventEntry;

    // eventTree branches   
    unsigned int eventNo; // The event no.
    unsigned int boardId; // The Id of the digitizer board
    unsigned int nChannels; // The number of channels used
    unsigned int channelMask; // The channel masking
    unsigned int channelSize; // The number of readouts per channel
    double trigTime; // The trigger time offset from start of run


    std::vector<int>* channelIds; // The channel ids (should be same size as fEventData).

    // The Individual Channel Vectors.
    std::vector<unsigned short>* channelData0;
    std::vector<unsigned short>* channelData1;
    std::vector<unsigned short>* channelData2;
    std::vector<unsigned short>* channelData3;
    std::vector<unsigned short>* channelData4;
    std::vector<unsigned short>* channelData5;
    std::vector<unsigned short>* channelData6;
    std::vector<unsigned short>* channelData7;

    TTimeStamp ts; 

};

#endif
