#ifndef GUI_CONTROLLER_H
#define GUI_CONTROLLER_H

#include "TGFrame.h"
#include "RQ_OBJECT.h"
#include "TString.h"
#include "TTimeStamp.h"
#include "WFAnalyzer.h"

#include <vector>

class MainWindow;
class ViewWindow;
class InfoWindow;
class ControlWindow;
class DAQEvent;
class TCanvas;
class TGraph;
class TLatex;
class TH2F;
class TObject;

class GuiController
{
    RQ_OBJECT("EVDGuiController")
public:
    GuiController(const TGWindow *p, int w, int h);
    virtual ~GuiController();

    void InitEvent(const char* filename);
    void Reload();
    void InitConnections();
    void Modified();
    void ClearListOfDrawables();

    // slots
    void HandleFileMenu(int id);
    void MenuOpen();
    void Prev();
    void Next();
    void Jump();
    void FindNextCoincidence();
    void FindNextMuon();
    void UpdateShowWFLines();
    void OpenLoEFile();
    void PrevSpecialEvent();
    void NextSpecialEvent();

    void DrawWF(std::vector<unsigned short>& ch, int wfNo, int padNo);
    void DrawPulses(int wfNo, int padNo);

    bool currentShowWFLines;

    // TGraph *gWF[3]; // previous, current, next waveform
    // TGraph *gMu[2][3]; // previous, current, next waveform of the two muon paddles
    TGraph *gWF[4]; // pmt 1, 2; muon paddle 1, 2;
    TH2F *hNoMuon;
    TLatex *text_noMuon;
    TTimeStamp triggerTS[3];  // previous, current, next waveform
    bool isMuon;
    bool isMuonDraw;

    // member variables
    MainWindow    *mw;
    ViewWindow    *vw;
    InfoWindow    *iw;
    ControlWindow *cw;
    DAQEvent       *event;
    WFAnalyzer     wfa[2];  // for the two PMTs.

    std::vector<int> listOfSpecialEvents;
    int currentSpecialEventIndex;

    TCanvas *can;
    TString baseDir;
    int currentEventEntry;
    std::vector<TObject*> listOfDrawables;

};

#endif
