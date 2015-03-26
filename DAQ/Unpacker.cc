#include "Unpacker.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TString.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TTimeStamp.h"

using namespace std;

Unpacker::Unpacker(const char* in, const char* out, int limit)
{   
    fInputFileName = in;
    fOutputFileName = out;
    fLimitEvents = limit; // read all events

    fStartTime = 0;
    timeTagRollover = 0;
    oldTimeTag = 0;

    fChannelIds = new std::vector<int>;
    // fChannelTime = new std::vector<unsigned int>;

    fChannelData0 = new std::vector<unsigned short>;
    fChannelData1 = new std::vector<unsigned short>;
    fChannelData2 = new std::vector<unsigned short>;
    fChannelData3 = new std::vector<unsigned short>;
    fChannelData4 = new std::vector<unsigned short>;
    fChannelData5 = new std::vector<unsigned short>;
    fChannelData6 = new std::vector<unsigned short>;
    fChannelData7 = new std::vector<unsigned short>;

    // This map is a super stupid hack . . . must be a better way.
    fChannelData.clear();
    fChannelData[0] = fChannelData0;
    fChannelData[1] = fChannelData1;
    fChannelData[2] = fChannelData2;
    fChannelData[3] = fChannelData3;
    fChannelData[4] = fChannelData4;
    fChannelData[5] = fChannelData5;
    fChannelData[6] = fChannelData6;
    fChannelData[7] = fChannelData7;

    InitOutput();
}

Unpacker::~Unpacker()
{

}

void Unpacker::InitOutput()
{
    TDirectory* tmpDir = gDirectory;

    fOutputFile = new TFile(fOutputFileName.Data(), "recreate");
    if (!fOutputFile->IsOpen()) {
      cout << "Output file " << fOutputFileName << " cannot be opened." << endl;
      exit(1);
    }

    // init Detector Geometry TTree
    TDirectory* subDir = fOutputFile->mkdir("DAQ");
    subDir->cd();
    fHeaderTree = new TTree("Header", "Header Information");
    
    fHeaderTree->Branch("startTime", &fStartTime); // start time
 
    // init Event TTree
    TDirectory* subDir2 = fOutputFile->mkdir("Event");
    subDir2->cd();
    fOutputTree = new TTree("Raw", "Raw Data Information");
    fOutputTree->Branch("eventNo", &fEventNo, "eventNo/i");
    fOutputTree->Branch("boardId", &fBoardId, "boardId/i");
    fOutputTree->Branch("nChannels", &fNChannels, "nChannels/i");
    fOutputTree->Branch("channelMask", &fChannelMask, "channelMask/i");
    fOutputTree->Branch("channelIds", &fChannelIds);
    fOutputTree->Branch("channelSize", &fChannelSize, "channelSize/i");
    
    fOutputTree->Branch("channelData0", &fChannelData0);
    fOutputTree->Branch("channelData1", &fChannelData1);
    fOutputTree->Branch("channelData2", &fChannelData2);
    fOutputTree->Branch("channelData3", &fChannelData3);
    fOutputTree->Branch("channelData4", &fChannelData4);
    fOutputTree->Branch("channelData5", &fChannelData5);
    fOutputTree->Branch("channelData6", &fChannelData6);
    fOutputTree->Branch("channelData7", &fChannelData7);

    // fOutputTree->Branch("channelTime", "vector<unsigned int>", &fChannelTime);

    fOutputTree->Branch("trigTime", &fTrigTime, "trigTime/D");

    gDirectory = tmpDir;

}


void Unpacker::Reset()
{
    // Make sure vectors are empty initially.

    fChannelIds->clear();
    
    fChannelData0->clear();
    fChannelData1->clear();
    fChannelData2->clear();
    fChannelData3->clear();
    fChannelData4->clear();
    fChannelData5->clear();
    fChannelData6->clear();
    fChannelData7->clear();
    
    // fChannelTime->clear();
    
}


void Unpacker::Unpack()
{
    // Open the input file
    ifstream rawDataFile(fInputFileName.Data(), ios::binary);
    if (!rawDataFile.is_open()) {
      cout << "Input File " << fInputFileName << " cannot be opened." << endl;
      exit(1);
    }
    
    
    // Calculate the total number of bytes in a file
    streampos begin = rawDataFile.tellg();
    rawDataFile.seekg(0, ios::end);
    streampos end = rawDataFile.tellg();
    
    cout << "The data file contains " << (end-begin) << " bytes." << endl;
    
    // Return to beginning of file.
    rawDataFile.seekg(0, ios::beg);
    
    // read and write header info to file
    ReadHeader(rawDataFile);
    fHeaderTree->Fill();
    TDirectory* tmpDir = gDirectory;
    fOutputFile->cd("/DAQ");
    fHeaderTree->Write("", TObject::kOverwrite);
    gDirectory = tmpDir;

    // read each trigger
    int nEvent = 0;
    // Check that the end of the file has not been reached.
    while (rawDataFile.tellg() != end) {
        Reset();
        ReadNextTrigger(rawDataFile);
        fOutputTree->Fill();
        nEvent++;
        // Check if limited entries have been reached
        if (fLimitEvents > 0 && nEvent >= fLimitEvents) {
            break;
        }
      
    } // End of binary file
    
    // Close the binary file
    rawDataFile.close();

    Finalize();
}

void Unpacker::ReadHeader(ifstream& rawDataFile)
{
    // There are two DAQs, one of which outputs a lot of extra
    // header information at the beginning of a file.
    // For now, this information will not be saved.
    // In future, the ability to save this information should be added.
    
    // Header
    char * guiHeader = new char [4];
    rawDataFile.read(guiHeader, 4);
    
    // In the GUI daq, there will be a flag (0xb0) in byte 4.
    // In non-GUI daq, there will be a flag (0xa0) in byte 4.
    unsigned int guiHeaderFlag = (unsigned int) (unsigned char) guiHeader[3];
    // cout << guiHeaderFlag << endl;
    if (guiHeaderFlag == 0xb0) {
        cout << "GUI DAQ header present." << endl;
        cout << "Skipping File Header information.." << endl;
      
        // Extract event size from GUI header
        // bytes 2, 1, 0 gives the number of 32-bit "words" in the event.
        unsigned int HeaderSize = (unsigned int) (unsigned char) guiHeader[3] * pow(16,6)
        - 0xB0000000 // Removes the flag of header start.
        + (unsigned int) (unsigned char) guiHeader[2] * pow(16,4)
        + (unsigned int) (unsigned char) guiHeader[1] * pow(16,2)
        + (unsigned int) (unsigned char) guiHeader[0];
      
        // There are four bytes in every 32-bit "word"
        HeaderSize *= 4;
      
        rawDataFile.seekg(0, ios::beg);
        char * fullHeader = new char [HeaderSize];
        rawDataFile.read(fullHeader, HeaderSize);
      
        // At this point ios::cur should point to the beginning of the first event.
    }
    else {
        // For the non-GUI DAQ we need to return to the beginning of the file.
        cout << "Non-GUI DAQ used." << endl;
        rawDataFile.seekg(0, ios::beg);
        TString name = gSystem->BaseName(fInputFileName.Data());
        // cout << name << endl;
        name = name.ReplaceAll(".dat", "");
        name = name.ReplaceAll("P20_", "");
        TObjArray *tx = name.Tokenize("-");
        if (! tx->GetEntries() == 6) {
            cout << "warning: file name not in datetime format." << endl;
        }
        TTimeStamp ts(
            ((TObjString *)(tx->At(0)))->String().Atoi(), 
            ((TObjString *)(tx->At(1)))->String().Atoi(), 
            ((TObjString *)(tx->At(2)))->String().Atoi(), 
            ((TObjString *)(tx->At(3)))->String().Atoi(), 
            ((TObjString *)(tx->At(4)))->String().Atoi(), 
            ((TObjString *)(tx->At(5)))->String().Atoi());
        cout << "File Start: " << ts << endl;            

        fStartTime = ts.AsDouble();
    }

}

void Unpacker::ReadNextTrigger(ifstream& rawDataFile)
{
    // Unpack header information
    // There are four parts to the header:
    // Header 0 - event size information
    // Header 1 - Digitizer board id, active channels
    // Header 2 - Event Counter
    // Header 3 - Trigger time
    
    // Header 0
    rawDataFile.seekg(0, ios::cur);
    char * header0 = new char [4];
    rawDataFile.read(header0, 4);
    // Header 1
    rawDataFile.seekg(0, ios::cur);
    char * header1 = new char [4];
    rawDataFile.read(header1, 4);
    // Header 2
    rawDataFile.seekg(0, ios::cur);
    char * header2 = new char [4];
    rawDataFile.read(header2, 4);
    // Header 3
    rawDataFile.seekg(0, ios::cur);
    char * header3 = new char [4];
    rawDataFile.read(header3, 4);
    
    // Check that header0 has new event flag
    // Byte 4 must be 0xa0- it is a flag assigned by Caen
    unsigned int headerFlag = (unsigned int) (unsigned char) header0[3];
    if (headerFlag != 0xa0) {
      cout << "Header flag not present at start of new event." << endl;
      cout << "Check input file!" << endl;
      cout << headerFlag << endl;
      exit(1);
    }
    
    // Extract event size from header0
    // bytes 2, 1, 0 gives the number of 32-bit "words" in the event.
    fEventSize = (unsigned int) (unsigned char) header0[3] * pow(16,6)
      - 0xA0000000 // Removes the flag of event start.
      + (unsigned int) (unsigned char) header0[2] * pow(16,4)
      + (unsigned int) (unsigned char) header0[1] * pow(16,2)
      + (unsigned int) (unsigned char) header0[0];
    
    // Subtract the header 32-bit "words" to get the number of data words
    fEventSize -= 4;
    // Convert the number of data 32-bit words to bytes
    fEventSize *= 4;

    // Board ID
    // This is the last 5 bits of an 8 bit number.
    // 0xF8 = 11111000 to mask out the unneccesary bits.
    // Then the bits must be shifted to the right (11111000 -> 00011111).
    fBoardId =((unsigned int) (unsigned char) header0[3]
                           & 0xF800) >> 3;
    
    // Channel Mask (first byte in header1)
    // Extract the channel Ids used and place into a vector.
    // This is done by comparing the mask to the eight channel ids.
    fChannelMask = (unsigned int) (unsigned char) header1[0];
    
    fChannelIds->clear();
    for (int i = 0; i < 8; i++) {
      if (fChannelMask & (1 << i)) fChannelIds->push_back(i);
    }
    
    fNChannels = fChannelIds->size();
    
    // Extract event number from header2
    fEventNo = (unsigned int) (unsigned char) header2[2] * pow(16,4)
      + (unsigned int) (unsigned char) header2[1] * pow(16,2)
      + (unsigned int) (unsigned char) header2[0];
    
    if (fEventNo%50000 == 0)
      cout << "Working on Event " << fEventNo << " ... " << endl;
    
    // Extract trigger time from header3
    fTrigTime = (double) ((unsigned int) (unsigned char) header3[3] * pow(16,6)
      + (unsigned int) (unsigned char) header3[2] * pow(16,4)
      + (unsigned int) (unsigned char) header3[1] * pow(16,2)
      + (unsigned int) (unsigned char) header3[0]);
    
    delete[] header0;
    delete[] header1;
    delete[] header2;
    delete[] header3;

    // deltaTS = ((double)fTrigTime - oldTimeTag) * 8.e-9;
    if (fTrigTime < oldTimeTag) {
        timeTagRollover ++;
        // deltaTS += (2**31)*8.e-9;
    }
    oldTimeTag = fTrigTime;

    fTrigTime += timeTagRollover*pow(2,31);  // add roll over offset
    fTrigTime *= 8e-9;  // convert to sec.
    fTrigTime += fStartTime;  // convert to unix timestamp

    // Find the number of bytes saved per channel
    if (fEventSize%fNChannels != 0) {
      cout << "Number of bytes must be divisible by number of channels." << endl;
      exit(1);
    }
    
    fChannelSize = fEventSize / fNChannels;
    
    // Loop through channel and save event information.
    for (unsigned int chan = 0; chan < fNChannels; chan++) {
      // Get entire event channel into memory
      rawDataFile.seekg(0, ios::cur);
      char * channelArr = new char [fChannelSize];
      rawDataFile.read(channelArr, fChannelSize);
      
      // Get this channel id.
      int chanId = fChannelIds->at(chan);
      
      // Extract channel data, by byte
      for (unsigned int i = 0; i < fChannelSize; i+=2) {
        unsigned int dataPoint = (unsigned int) (unsigned char) channelArr[i+1] * pow(16,2)
        + (unsigned int) (unsigned char) channelArr[i];
        fChannelData[chanId]->push_back(dataPoint);
        
        // Fill vector with time (do only once)
        // A reading is done every 4 ns.
        // i/2 is the same as fChannelData[chanId]->size()
        // Hence i*2 would give 4 ns separation between dataPoints
        // if (chan == 0) fChannelTime->push_back(i*2);
      }
      
    } // End of loop through channels.
    

}

void Unpacker::Finalize() {
    cout << "Reached end of analysis with " << fOutputTree->GetEntries() << " events." << endl;

    // Write fEventTree to fEventTree
    TDirectory* tmpDir = gDirectory;
    fOutputFile->cd("/Event");
    fOutputTree->Write("", TObject::kOverwrite);
    gDirectory = tmpDir;

    fOutputFile->Close();
}
