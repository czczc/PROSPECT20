{
    TString include = ".include ";
    TString load = ".L ";
    TString pwd = gSystem->pwd();
    pwd = pwd + '/';

    TString prefix = "../DAQ";
    gROOT->ProcessLine( include + pwd + prefix );
    gROOT->ProcessLine( load + prefix + "/Unpacker.cc+" );

    prefix = "../Event";
    gROOT->ProcessLine( include + pwd + prefix );
    gROOT->ProcessLine( load + prefix + "/DAQEvent.cc+" );
    gROOT->ProcessLine( load + prefix + "/WFAnalyzer.cc+" );
    gROOT->ProcessLine( load + prefix + "/TQStatsEvent.cc+" );

    
    prefix = "../TreeMaker";
    gROOT->ProcessLine( include + pwd + prefix );
    gROOT->ProcessLine( load + prefix + "/TQTree.cc+" );
    gROOT->ProcessLine( load + prefix + "/TQStatsTree.cc+" );

    prefix = "../Viewer";
    gROOT->ProcessLine( include + pwd + prefix );
    gROOT->ProcessLine( load + prefix + "/ViewWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/InfoWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/ControlWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/MainWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/GuiController.cc+" );
}
