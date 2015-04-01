void gen_tq(const char* in, const char* out, int nEvents=-1)
{
    gROOT->ProcessLine(".x loadClasses.C");

    TQTree t(in, out, nEvents);
    t.Write();
}