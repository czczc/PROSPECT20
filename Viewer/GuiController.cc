#include "GuiController.h"
#include "MainWindow.h"
#include "ViewWindow.h"
#include "InfoWindow.h"
#include "ControlWindow.h"
#include "DAQEvent.h"
#include "WFAnalyzer.h"

#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TApplication.h"
#include "TGMenu.h"
#include "TGFileDialog.h"

#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGraph.h"
#include "TTimeStamp.h"
#include "TAxis.h"
#include "TLatex.h"
#include "TH2F.h"
#include "TFile.h"
#include "TBox.h"
#include "TLine.h"

#include <exception>
#include <iostream>
#include <map>
using namespace std;

GuiController::GuiController(const TGWindow *p, int w, int h)
{

    baseDir = baseDir + gSystem->DirName(__FILE__) + "/..";
    event = 0;
    currentShowWFLines = false;

    mw = new MainWindow(p, w, h);

    vw = mw->fViewWindow;
    cw = mw->fControlWindow;
    iw = mw->fControlWindow->fInfoWindow;
    can = vw->can;

    isMuon = false;
    isMuonDraw = false;

    for (int i=0; i<4; i++) {
        gWF[i] = new TGraph(DAQEvent::MAX_SAMPLE);
        gWF[i]->SetLineWidth(2);
    }
    hNoMuon = new TH2F("hNoMuon", "Muon Paddles", 100, 0, 1200, 100, 0, 4100);
    text_noMuon = new TLatex(330, 2000, "No Muon");
    text_noMuon->SetTextSize(0.2);

    for (int i=0; i<3; i++) {
        can->GetPad(i+1)->SetGridx();
        can->GetPad(i+1)->SetGridy();
    }

    InitConnections();
    MenuOpen();
}

GuiController::~GuiController()
{

}

void GuiController::InitConnections()
{
    mw->fFileMenu->Connect("Activated(int)", "GuiController", this, "HandleFileMenu(int)");

    cw->fPrevButton->Connect("Clicked()", "GuiController", this, "Prev()");
    cw->fNextButton->Connect("Clicked()", "GuiController", this, "Next()");
    cw->fEventEntry->Connect("ReturnPressed()", "GuiController", this, "Jump()");
    cw->fNextCIButton->Connect("Clicked()", "GuiController", this, "FindNextCoincidence()");
    cw->fNextMuonButton->Connect("Clicked()", "GuiController", this, "FindNextMuon()");
    cw->showWFLinesButton->Connect("Clicked()", "GuiController", this, "UpdateShowWFLines()");

}


//-------------------------------------------------
void GuiController::Modified()
{   

    can->GetPad(1)->Modified();
    can->GetPad(2)->Modified();
    can->GetPad(3)->Modified();
    can->Update();
}


//-------------------------------------------------
void GuiController::Prev()
{
    if (currentEventEntry==0) {
        currentEventEntry = event->nEvents-1;
    }
    else {
        currentEventEntry--;
    }
    Reload();
}

//-------------------------------------------------
void GuiController::Next()
{
    if (currentEventEntry==event->nEvents-1) {
        currentEventEntry = 0;
    }
    else {
        currentEventEntry++;
    }
    Reload();
}

//-------------------------------------------------
void GuiController::Jump()
{
    currentEventEntry = int(cw->fEventEntry->GetNumber());
    if (currentEventEntry>=event->nEvents-1) {
        currentEventEntry = event->nEvents-1;
    }
    else if (currentEventEntry < 0) {
        currentEventEntry=0;
    }
    Reload();
}

//-------------------------------------------------
void GuiController::FindNextCoincidence()
{
    int counter = 0;
    Next();
    while ( (triggerTS[2].AsDouble()-triggerTS[1].AsDouble())*1e6 > 200 && counter < 200) {
        Next();
        counter++;
    }
    if (counter == 200 ) {
        cout << "no coincidence found after 200 events." << endl;
    }
}

//-------------------------------------------------
void GuiController::FindNextMuon()
{
    int counter = 0;
    Next();
    while ( !isMuon && counter < 200) {
        Next();
        counter++;
    }
    if (counter == 200 ) {
        cout << "no muon found after 200 events." << endl;
    }
}

//-------------------------------------------------
void GuiController::UpdateShowWFLines()
{
    currentShowWFLines = !currentShowWFLines;
    Reload();

    // if (cw->showWFLinesButton->IsDown()) {
    //     if (currentShowWFLines == true) return;
    //     currentShowWFLines = true;
    //     Reload();
    // }
    // else {
    //     if (currentShowMC == false) return;
    //     currentShowMC = false;
    //     HideTrack();
    // }

}
//-------------------------------------------------
void GuiController::Reload()
{    
    ClearListOfDrawables();

    cw->fEventEntry->SetNumber(currentEventEntry);

    if (currentEventEntry==0) triggerTS[0] = 0;
    else {
        event->GetEntry(currentEventEntry-1);
        triggerTS[0] = event->ts;
    }
    if (currentEventEntry==event->nEvents-1) triggerTS[2] = 0;
    else {
        event->GetEntry(currentEventEntry+1);
        triggerTS[2] = event->ts;
    }

    event->GetEntry(currentEventEntry);
    triggerTS[1] = event->ts;

    wfa[0].SetWF(event->channelData0);
    wfa[0].Process();
    wfa[1].SetWF(event->channelData1);
    wfa[1].Process();

    iw->DrawEventInfo(event, wfa[0], wfa[1]);

    isMuon = false;
    isMuonDraw = false;
    DrawWF(*(event->channelData0), 0, 1);
    DrawWF(*(event->channelData1), 1, 2);
    DrawWF(*(event->channelData6), 3, 3);
    DrawWF(*(event->channelData5), 2, 3);

    if (currentShowWFLines) {
        DrawPulses(0, 1);
        DrawPulses(1, 2);
    }

    // event->PrintInfo(1);
    event->PrintInfo();

    Modified();
}

//-------------------------------------------------
void GuiController::DrawWF(vector<unsigned short>& ch, int wfNo, int padNo)
{
    can->cd(padNo);

    int size = ch.size();
    if (size == 0) {
        for (int n=0; n<DAQEvent::MAX_SAMPLE; n++) {
            gWF[wfNo]->SetPoint(n, n*4, 0);
        }
    }
    else {
        for (int n=0; n<DAQEvent::MAX_SAMPLE; n++) {
            gWF[wfNo]->SetPoint(n, n*4, ch[n]);
        }
    }

    if (wfNo == 0) {
        gWF[wfNo]->SetTitle(Form("PMT1 (%s)", triggerTS[1].AsString()));
    }
    else if (wfNo == 1) {
        gWF[wfNo]->SetTitle(Form("PMT2 (prev: %+d #mus | next: %+d #mus)", 
            int((triggerTS[0].AsDouble()-triggerTS[1].AsDouble())*1e6),
            int((triggerTS[2].AsDouble()-triggerTS[1].AsDouble())*1e6)));
    }
    else if (wfNo == 2) {
        gWF[wfNo]->SetTitle("Muon Paddles");
        gWF[wfNo]->SetLineColor(kBlue-2);
    }
    else if (wfNo == 3) {  // muon paddle 3 gain is very high
        gWF[wfNo]->SetTitle("Muon Paddles");
        gWF[wfNo]->SetLineColor(kRed-2);
    }

    if (wfNo < 2) {
        gWF[wfNo]->Draw("ALP");
        gWF[wfNo]->GetXaxis()->SetTitle("ns");
        gWF[wfNo]->GetXaxis()->SetRangeUser(0, 1200);
    }
    else {
        if ((wfNo == 2 || wfNo == 3)  && ch.size()>0) {
            isMuon = true;
            if (wfNo == 3 && ch.size()>0) {
                gWF[wfNo]->Draw("ALP");
                gWF[wfNo]->GetXaxis()->SetTitle("ns");
                gWF[wfNo]->GetXaxis()->SetRangeUser(0, 1200);
                isMuonDraw = true;
                // cout << "I am here wf 3 " << ch.size() << endl;

            }

            else if (isMuonDraw) {
                // cout << "I am here wf 2 same " << ch.size() << endl;
                gWF[wfNo]->Draw("LPsame");
            }
            else {
                // cout << "I am here wf 2 " << ch.size() << endl;
                gWF[wfNo]->Draw("ALP");
                gWF[wfNo]->GetXaxis()->SetTitle("ns");
                gWF[wfNo]->GetXaxis()->SetRangeUser(0, 1200);
                isMuonDraw = true;
            }
        }

        if (!isMuonDraw) {
            hNoMuon->Draw();
            text_noMuon->Draw();
        }
    }

}

//---------------------------------------------------

void GuiController::DrawPulses(int wfNo, int padNo)
{
    can->cd(padNo);

    WFAnalyzer *w = &wfa[wfNo];

    // bool showLines = false;
    for (int i=0; i<w->nPulse; i++) {
        double start = w->tdcs_start[i]*4;
        double end = w->tdcs_end[i]*4;
        TBox *b = new TBox(start, w->baseline, end, w->baseline - w->charges_peak[i]);
        b->SetLineColor(kOrange+7);
        // b->SetFillColor(kCyan-9);
        b->SetFillStyle(0);
        b->Draw();
        listOfDrawables.push_back(b);
        TLine *l0 = new TLine(w->tdcs_peak[i]*4, w->baseline, w->tdcs_peak[i]*4, w->baseline - w->charges_peak[i]);
        l0->SetLineColor(kGreen-2);
        l0->Draw();
        listOfDrawables.push_back(l0);
        TLine *l1 = new TLine(w->tdcs_prepeak_low[i]*4, w->baseline, w->tdcs_prepeak_low[i]*4, w->baseline - w->charges_peak[i]);
        l1->SetLineColor(kBlue);
        l1->Draw();
        listOfDrawables.push_back(l1);
        TLine *l2 = new TLine(w->tdcs_postpeak_low[i]*4, w->baseline, w->tdcs_postpeak_low[i]*4, w->baseline - w->charges_peak[i]);
        l2->SetLineColor(kBlue);
        l2->Draw();
        listOfDrawables.push_back(l2);
        TLine *l3 = new TLine(w->tdcs_prepeak_high[i]*4, w->baseline, w->tdcs_prepeak_high[i]*4, w->baseline - w->charges_peak[i]);
        l3->SetLineColor(kMagenta-2);
        l3->Draw();
        listOfDrawables.push_back(l3);
        TLine *l4 = new TLine(w->tdcs_postpeak_high[i]*4, w->baseline, w->tdcs_postpeak_high[i]*4, w->baseline - w->charges_peak[i]);
        l4->SetLineColor(kMagenta-2);
        l4->Draw();
        listOfDrawables.push_back(l4);
        TLine *l5 = new TLine(w->tdcs_thresh[i]*4, w->baseline, w->tdcs_thresh[i]*4, w->baseline - w->charges_peak[i]);
        l5->SetLineColor(kGray);
        l5->Draw();
        listOfDrawables.push_back(l5);
    }

}

//---------------------------------------------------
void GuiController::InitEvent(const char* filename)
{
    if (event) delete event;
    event = new DAQEvent(filename);
    currentEventEntry = 0;

    mw->SetWindowName(Form("PROSPECT 20 Event Display: %s", gSystem->BaseName(filename)));

    Reload();
}

//---------------------------------------------------
void GuiController::MenuOpen()
{
    const char *filetypes[] = {"ROOT files", "*.root", 0, 0};
    TString dir(baseDir + "/data");
    TGFileInfo fi;
    fi.fFileTypes = filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(gClient->GetRoot(), mw, kFDOpen, &fi);
    cout << "opening file: " << fi.fFilename << endl;

    InitEvent(fi.fFilename);
}

//---------------------------------------------------
void GuiController::HandleFileMenu(int id)
{
    switch (id) {
        case M_FILE_OPEN:
            MenuOpen();
            break;

        case M_FILE_EXIT:
            gApplication->Terminate(0);
            break;
    }
}

//---------------------------------------------------
void GuiController::ClearListOfDrawables()
{
    size_t size = listOfDrawables.size();
    for (size_t i=0; i!=size; i++) {
        delete listOfDrawables[i];
    }
    listOfDrawables.clear();
}

