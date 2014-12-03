

#include <TGraph.h>

extern bool identical;  //Are the two particles identical?  This turns on/off quantum interference
extern int totalBins; //How many bins will the histograms have?  maxKstar/totalBins will be the bin width.
extern double normalization;  //Simple normalization factor
extern double lamPrimary; // lambda parameter for primary pairs
extern double maxKstar;
extern double radius;
extern double d0; // Effective range of interaction (Should be >= 0)
extern double f0re;
extern double f0im;


TGraph* GetLednickyEqn(bool identicalParticles);

