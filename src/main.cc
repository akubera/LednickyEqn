
#include "lednicky.h"

#include <TH1D.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TApplication.h>
#include <iostream>

#include <cstdlib>

using namespace std;

std::string EXEC_NAME;

void usage();
void parse_args(const std::vector<std::string>& args);

int main(int argc, char **argv)
{
  EXEC_NAME = *argv;
  std::vector<std::string> args(argv+1, argv+argc);
  parse_args(args);

  //Plot the primary-primary correlation function (graphPrimaryCF) as calculated via the Lednicky and Lyoboshits parameterization.  The graph is scaled by the relevant lambda parameters.

  // The TApplication object allows the execution of the code to pause.
  // This allows the user a chance to look at and manipulate a TBrowser before
  // the program ends and closes everything
  TApplication* theApp = new TApplication("App", &argc, argv);


  TGraph *graphPrimaryCF = GetLednickyEqn(identical);
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
  // Select "Exit ROOT" from Canvas "File" menu to exit and execute the next statements.

  return EXIT_SUCCESS;
}

void
usage()
{
  char indent[] = "    ";
  cout << "This will generate and draw a correlation function calculated from Lednicky eqn.\n";
  cout << "Usage:\n";
  cout << indent << "-h, --help " << " Display this help" << '\n';
  cout << indent << "--nogui " << " Do not display the GUI." << '\n';
  cout << std::endl;
}

void
parse_args(const std::vector<std::string>& args)
{
  for (auto arg : args) {
    if (arg == "--nogui") {
      cout << "Running No-Gui\n";
    }
    else if (arg == "-h" or arg == "--help") {
      usage();
      exit(EXIT_SUCCESS);
    } else if (arg[0] == '-') {
      cerr << "Unknown option '" << arg << "'\n";
      usage();
      exit(EXIT_FAILURE);
    }
  }
}

