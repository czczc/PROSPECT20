#ifndef CONTROL_WINDOW_H
#define CONTROL_WINDOW_H

#include "TGFrame.h"

class InfoWindow;
class TGTextButton;
class TGNumberEntryField;
class TGCheckButton;
class TGTextEntry;
class TGPictureButton;

class ControlWindow: public TGVerticalFrame
{
public:
    ControlWindow(const TGWindow *p, int w, int h);
    virtual ~ControlWindow();

    InfoWindow         *fInfoWindow;

    TGTextButton       *fPrevButton, *fNextButton;
    TGNumberEntryField *fEventEntry; 

    TGTextButton       *fNextCIButton;
    TGTextButton       *fNextMuonButton;
    
    TGCheckButton      *showWFLinesButton;

    // List of Events
    TGTextEntry        *fLoEFileNameEntry;
    TGPictureButton    *fLoEOpenFileButton;

    TGTextButton       *fLoEPrevButton, *fLoENextButton;  
    TGNumberEntryField *fLoEEventEntry;

    ClassDef(ControlWindow, 0)
};

#endif
