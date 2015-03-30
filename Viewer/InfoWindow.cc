#include "InfoWindow.h"
#include "DAQEvent.h"
#include "WFAnalyzer.h"

#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TLatex.h"

#include <iostream>
#include <vector>
using namespace std;


InfoWindow::InfoWindow(const TGWindow *p, int w,int h)
    :TRootEmbeddedCanvas("InfoWindowCanvas", p, w, h)
{
    can = GetCanvas();
}

InfoWindow::~InfoWindow()
{
}


//------------------------------------------------------
void InfoWindow::ClearCanvas()
{
    can->cd();    
    size_t size = listOfDrawables.size();
    for (size_t i=0; i!=size; i++) {
        delete listOfDrawables[i];
    }
    listOfDrawables.clear();
    can->Clear();
}

//------------------------------------------------------
void InfoWindow::DrawEventInfo(DAQEvent *event, WFAnalyzer& wfa1, WFAnalyzer& wfa2)
{
    ClearCanvas();
    vector<TString> lines;
    lines.push_back(Form("      Event: %i", event->eventNo));
    lines.push_back(Form("   baseline: %.0f, %.0f", wfa1.baseline, wfa2.baseline));
    lines.push_back(Form("    nPulse: %i, %i", wfa1.nPulse, wfa2.nPulse));
    lines.push_back(Form("  maxCharge: %.0f, %.0f", wfa1.maxCharge, wfa2.maxCharge));
    lines.push_back(Form("totalCharge: %.0f, %.0f", wfa1.totalCharge, wfa2.totalCharge));
    lines.push_back(Form("    maxPeak: %.0f, %.0f", wfa1.maxPeak, wfa2.maxPeak));

    float startx = 0.05;
    float starty = 0.9;
    size_t size = lines.size();
    for (size_t i=0; i<size; i++) {
        TLatex* tex = new TLatex(startx, starty-i*0.1, lines[i].Data());
        listOfDrawables.push_back(tex);
        tex->SetNDC();
        tex->SetTextFont(102);
        tex->SetTextSize(0.08);
        tex->Draw();
    }

    UpdateCanvas();
}


//------------------------------------------------------
void InfoWindow::UpdateCanvas()
{
    can->Modified();
    can->Update();
}
