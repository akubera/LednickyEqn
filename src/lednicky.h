

#include <TGraph.h>

extern bool identical;  //Are the two particles identical?  This turns on/off quantum interference
extern int totalBins; //How many bins will the histograms have?  maxKstar/totalBins will be the bin width.
extern double normalization;  //Simple normalization factor
extern double lamPrimary; // lambda parameter for primary pairs
extern double maxKstar;

TGraph* GetLednickyEqn(bool identicalParticles);
