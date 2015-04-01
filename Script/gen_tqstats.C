void gen_tqstats(const char* in, const char* out, int nEvents=-1)
{
    gROOT->ProcessLine(".x loadClasses.C");

    TQStatsTree t(in, out, nEvents);
    t.Write();
}