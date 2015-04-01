void unpack(const char* in, const char* out, int nEvents=-1)
{
    gROOT->ProcessLine(".x loadClasses.C");
    Unpacker p(in, out, nEvents);
    p.Unpack();
}