
#include "lednicky.h"

#include <TH1D.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TApplication.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
  //Plot the primary-primary correlation function (graphPrimaryCF) as calculated via the Lednicky and Lyoboshits parameterization.  The graph is scaled by the relevant lambda parameters.
  cout<<"This will draw a correlation function calculated from Lednicky eqn."
      <<endl;

  TApplication* theApp = new TApplication("App", &argc, argv);
  //The TApplication object allows the execution of the code to pause.
  //This allows the user a chance to look at and manipulate a TBrowser before
  //the program ends and closes everything


  TGraph *graphPrimaryCF = 0x0;
  graphPrimaryCF = GetLednickyEqn(identical);
  for (int xBin=0; xBin<totalBins; xBin++) {
    graphPrimaryCF->GetY()[xBin] = 1.0 + (graphPrimaryCF->GetY()[xBin]-1.0)*lamPrimary;
    graphPrimaryCF->GetY()[xBin] /= normalization; //scale so graphs look right
  }
  //Draw finished correlation function as a "connect-the-dots" line
  graphPrimaryCF->SetLineWidth(3);

  //Setup a canvas
  TCanvas c1;
  //Draw a blank histogram with the correction dimensions on the canvas
  TH1D *templateHist = new TH1D("h1","",totalBins, 0., maxKstar);
  templateHist->SetAxisRange(0.8,1.1,"Y");
  templateHist->SetYTitle("C(#it{k}*)");
  templateHist->SetXTitle("#it{k}* (GeV/#it{c})");
  templateHist->Draw();
  //Draw the correlation function on the canvas
  graphPrimaryCF->Draw("L");
  theApp->Run(kTRUE); //Run the TApp to pause the code.
  // Select "Exit ROOT" from Canvas "File" menu to exit
  // and execute the next statements.
  return 0;
}
