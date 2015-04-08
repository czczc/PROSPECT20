#include "ControlWindow.h"
#include "InfoWindow.h"

#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
#include "TGTextEntry.h"

#include <iostream>
using namespace std;

ControlWindow::ControlWindow(const TGWindow *p, int w, int h)
    :TGVerticalFrame(p, w, h)
{    
    fInfoWindow = new InfoWindow(this, w, 300); 
    AddFrame(fInfoWindow, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    
    // Event Navigation
    TGGroupFrame *navGroupFrame = new TGGroupFrame(this, "Event Navigation", kVerticalFrame);
    AddFrame(navGroupFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 0, 0));

    TGHorizontalFrame *navigationFrame = new TGHorizontalFrame(navGroupFrame, w, 100, kFixedWidth);
    navGroupFrame->AddFrame(navigationFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    {
        fPrevButton = new TGTextButton(navigationFrame, "< Prev");
        navigationFrame->AddFrame(fPrevButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

        fEventEntry = new TGNumberEntryField(navigationFrame, -1, 0, TGNumberFormat::kNESInteger);
        fEventEntry->SetDefaultSize(80, 20);
        navigationFrame->AddFrame(fEventEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));

        fNextButton = new TGTextButton(navigationFrame, "Next >");
        navigationFrame->AddFrame(fNextButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
    }

    fNextCIButton = new TGTextButton(navGroupFrame, "Find Next Coincidence");
    navGroupFrame->AddFrame(fNextCIButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2)); 

    fNextMuonButton = new TGTextButton(navGroupFrame, "Find Next Muon");
    navGroupFrame->AddFrame(fNextMuonButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2)); 

    showWFLinesButton = new TGCheckButton(navGroupFrame, "Show Waveform Lines");
    navGroupFrame->AddFrame(showWFLinesButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    showWFLinesButton->SetState(kButtonUp);


    // List of Special Events
    TGGroupFrame *loEGroupFrame = new TGGroupFrame(this, "List of Special Events", kVerticalFrame);
    AddFrame(loEGroupFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 0, 0));

    TGHorizontalFrame *LoEInputFrame = new TGHorizontalFrame(loEGroupFrame, w, 100, kFixedWidth);
    loEGroupFrame->AddFrame(LoEInputFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    {
        fLoEFileNameEntry = new TGTextEntry(LoEInputFrame, " Open an event list ...");
        LoEInputFrame->AddFrame(fLoEFileNameEntry, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
        // fLoEFileNameEntry->SetDefaultSize(w-40, 20);

        fLoEOpenFileButton = new TGPictureButton(LoEInputFrame, "fileopen.xpm");
        LoEInputFrame->AddFrame(fLoEOpenFileButton, new TGLayoutHints(kLHintsTop | kFixedWidth, 3, 2, 2, 2));
        fLoEOpenFileButton->SetMaxWidth(20);
    }

    TGHorizontalFrame *LoEFrame = new TGHorizontalFrame(loEGroupFrame, w, 100, kFixedWidth);
    loEGroupFrame->AddFrame(LoEFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    {
        fLoEPrevButton = new TGTextButton(LoEFrame, "< Prev");
        LoEFrame->AddFrame(fLoEPrevButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

        fLoEEventEntry = new TGNumberEntryField(LoEFrame, -1, 0, TGNumberFormat::kNESInteger);
        fLoEEventEntry->SetDefaultSize(80, 20);
        LoEFrame->AddFrame(fLoEEventEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));

        fLoENextButton = new TGTextButton(LoEFrame, "Next >");
        LoEFrame->AddFrame(fLoENextButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
    }


}

ControlWindow::~ControlWindow()
{
}
