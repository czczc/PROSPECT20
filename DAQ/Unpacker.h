#ifndef _UNPACKER_
#define _UNPACKER_

#include <vector>
#include <map>
#include "TString.h"

class TFile;
class TTree;

class Unpacker {
public:
    Unpacker(const char* in="", const char* out="test.root", int limit=-1);
    virtual ~Unpacker();

    void InitOutput();
    void Unpack();
    void Reset();
    void ReadHeader(ifstream& rawDataFile);
    void ReadNextTrigger(ifstream& rawDataFile);
    void Finalize();

    int fLimitEvents;
    TString fInputFileName;
    TString fOutputFileName;
    TFile* fOutputFile;
    TTree* fOutputTree;
    TTree* fHeaderTree;

    double fStartTime;

    // branches   
    unsigned int fBoardId; // The Id of the digitizer board
    unsigned int fEventNo; // The event number
    double fTrigTime; // trigger unix timestamp
    unsigned int fTrigTime_s;  // second part of the timestamp
    unsigned int fTrigTime_ns;  // nanosecond part of the timestamp

    // Eventually add a list of boards used...
    // Not needed for now (we have only one board).
    unsigned int fNChannels; // The number of channels used
    unsigned int fChannelMask; // The channel masking
    unsigned int fChannelSize; // The number of readouts per channel

    std::vector<int>* fChannelIds; // The channel ids (should be same size as fEventData).
    // std::vector<unsigned int>* fChannelTime; // A vector containing the timing of the readings.

    // The Individual Channel Vectors.
    std::vector<unsigned short>* fChannelData0;
    std::vector<unsigned short>* fChannelData1;
    std::vector<unsigned short>* fChannelData2;
    std::vector<unsigned short>* fChannelData3;
    std::vector<unsigned short>* fChannelData4;
    std::vector<unsigned short>* fChannelData5;
    std::vector<unsigned short>* fChannelData6;
    std::vector<unsigned short>* fChannelData7;

    // other helper variables
    unsigned int fEventSize;  // The event size
    std::map<int, std::vector<unsigned short>* > fChannelData;
    double oldTimeTag;
    // double deltaTS;
    int timeTagRollover;

};

#endif 
