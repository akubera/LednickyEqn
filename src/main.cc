
#include "lednicky.h"

#include <TString.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TImage.h>
#include <TApplication.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <TSystem.h>

#include <cstdlib>

using namespace std;

bool SHOW_GUI = true;

std::string EXEC_NAME;
std::string OUTPUT;
TString title;

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
  for (int xBin=0; xBin < totalBins; xBin++) {
    graphPrimaryCF->GetY()[xBin] = 1.0 + (graphPrimaryCF->GetY()[xBin]-1.0)*lamPrimary;
    graphPrimaryCF->GetY()[xBin] /= normalization; //scale so graphs look right
  }

  //Draw finished correlation function as a "connect-the-dots" line
  graphPrimaryCF->SetLineWidth(3);

  //Setup a canvas
  TCanvas *c = new TCanvas;
  //Draw a blank histogram with the correction dimensions on the canvas
  TH1D *templateHist = new TH1D("h1","",totalBins, 0., maxKstar);
  templateHist->SetAxisRange(0.8,1.1,"Y");
  templateHist->SetYTitle("C(#it{k}*)");
  templateHist->SetXTitle("#it{k}* (GeV/#it{c})");
  templateHist->Draw();
  templateHist->SetStats(0);
  std::stringstream title_stream;
  title_stream << "Lednicky Prediction \\$("
          << "f_0= " << f0re << "," 
          << "R = " << radius<< ")\\$ ";
  templateHist->SetTitle(title_stream.str().c_str());

  //Draw the correlation function on the canvas
  graphPrimaryCF->Draw("L");

  if (SHOW_GUI) {
    theApp->Run(kTRUE); //Run the TApp to pause the code.
  // Select "Exit ROOT" from Canvas "File" menu to exit and execute the next statements.
  }

gSystem->ProcessEvents();

//   c1.Draw();

   TCanvas *c2 = new TCanvas;
   TH1F *h = new TH1F("gaus", "gaus", 100, -5, 5);
   h->FillRandom("gaus", 10000);
   h->Draw();

   gSystem->ProcessEvents();

   TImage *img = TImage::Create();

   //img->FromPad(c, 10, 10, 300, 200);
   img->FromPad(c2);

   img->WriteImage("canvas.png");

  if (OUTPUT.length()) {
   TImage *img = TImage::Create();
   img->FromPad(c);
   img->WriteImage(OUTPUT.c_str());
   delete img;
  }

  delete c;

  return EXIT_SUCCESS;
}

void
usage()
{
  char indent[] = "    ";
  cout << "This will generate and draw a correlation function calculated from Lednicky eqn.\n";
  cout << "Usage:\n\n";
  cout << "  " << EXEC_NAME << " <OPTIONS>\n\n";
  cout << "Options:\n";
  cout << indent << "-h, --help "  << '\t'<< '\t' << '\t' << " Display this help" << '\n';
  cout << indent << "--nogui "   << '\t'<< '\t'<< '\t' << " Do not display the GUI." << '\n';
  cout << indent << "--radius <radius (fm)> " << '\t' << " Use as source radius." << '\n';
  cout << indent << "--bin_count <integer> " << '\t' << " Number of bins in the correlation function plot." << '\n';
  cout << indent << "--max_kstar <k* (GeV/C)> " << '\t' << " Upper limit of the correlation function's domain." << '\n';
  cout << std::endl;
}

void
parse_args(const std::vector<std::string>& args)
{
  for (auto arg_it = args.begin();
        arg_it != args.end();
        arg_it++) {
    auto arg = *arg_it;
    if (arg == "--nogui" || arg == "--no-gui") {
      cout << "[Lednicky] Running No-Gui\n";
      SHOW_GUI = false;
    }
    else if (arg == "-h" or arg == "--help") {
      usage();
      exit(EXIT_SUCCESS);
    }
    else if (arg == "--radius") {
      std::string radius_param(*(++arg_it));
      try {
        radius = std::stof(radius_param);
      } catch (std::invalid_argument err_ia) {
        cerr << "Unable to transform radius argument '" << radius_param << "' into a floating point number.\n";
        exit(EXIT_FAILURE);
      }
    }
    else if (arg == "--nonidentical") {
      identical = false;
    }
    else if (arg == "--identical") {
      identical = true;
    }
    else if (arg == "--bin_count") {
      std::string bin_param(*(++arg_it));
      try {
        totalBins = std::stoi(bin_param);
      } catch (std::invalid_argument err_ia) {
        cerr << "Unable to transform bin_count argument '" << bin_param << "' into an integer.\n";
        exit(EXIT_FAILURE);
      }
    }
    else if (arg == "--max_kstar") {
      std::string kstar_param(*(++arg_it));
      try {
        maxKstar = std::stof(kstar_param);
      } catch (std::invalid_argument err_ia) {
        cerr << "Unable to transform max_kstar argument '" << kstar_param << "' into a floating point number.\n";
        exit(EXIT_FAILURE);
      }
    }
    else if (arg == "--range") {
      d0 = std::stod(*(++arg_it));
    }
    else if (arg == "--title") {
      title =*(++arg_it);
    }
    else if (arg[0] == '-') {
      cerr << "Unknown option '" << arg << "'\n";
      usage();
      exit(EXIT_FAILURE);
    }
    else {
      OUTPUT = arg;
    }
  }
}

