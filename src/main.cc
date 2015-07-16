
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
#include <algorithm>

using namespace std;

struct ProgramOptions {
  bool show_gui {true};
  TString exe_name;
  TString output;

  /// Femtoscopic Radius
  double radius {3.0};
};

bool SHOW_GUI = true;

std::string EXEC_NAME;
std::string OUTPUT;
TString title;

void usage();
ProgramOptions parse_args(const std::vector<std::string>& args);

int
main(int argc, char **argv)
{
  const std::vector<std::string> argvv(argv, argv+argc);
  const ProgramOptions args = parse_args(argvv);

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

  const char title_tmpl[] = "Lednicky Prediction \\$(f_0=%.3f, d_0=%.3f, R=%.3f)\\$";
  templateHist->SetTitle(TString::Format(title_tmpl, f0re, d0, radius));

  // Draw the correlation function on the canvas
  graphPrimaryCF->Draw("L");

  // pause and wait for user
  if (SHOW_GUI) {
    gSystem->ProcessEvents();
    theApp->Run(kTRUE); //Run the TApp to pause the code.
  // Select "Exit ROOT" from Canvas "File" menu to exit and execute the next statements.
  }


//   c1.Draw();

   TCanvas *c2 = new TCanvas;
   TH1F *h = new TH1F("gaus", "gaus", 100, -5, 5);
   h->FillRandom("gaus", 10000);
   h->Draw();

   ///gSystem->ProcessEvents();

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

ProgramOptions
parse_args(const std::vector<std::string>& args)
{
  ProgramOptions opts;
  auto arg_it = args.cbegin();
  opts.exe_name = *arg_it;

  auto show_help_and_exit = [] (int status) {
    usage();
    exit(status);
  };

  auto set_show_gui = [&opts] (bool yes_or_no) {
    opts.show_gui = yes_or_no;
    SHOW_GUI = yes_or_no;
  };

  for (arg_it++; arg_it != args.end(); arg_it++) {
    auto arg = *arg_it;

    // skip empty arguments
    if (arg.size() == 0) {
      continue;
    }

    auto key_start = std::find_if_not(arg.begin(), arg.end(), [](char c){ return c == '-';}),
           key_end = std::find(key_start, arg.end(), '=');

    auto value_start = (key_end != arg.end()) ? key_end + 1 : arg.end(),
           value_end = arg.end();

    int dash_count = std::distance(arg.begin(), key_start);


    std::string key(key_start, key_end),
                val(value_start, value_end);

    // treat each char in key as a single key
    if (dash_count == 1) {
      for (auto subkey : key) {
        switch (subkey){
        case 'h':
          show_help_and_exit(EXIT_SUCCESS);
        default:
          std::cerr << "Unknown option '" << subkey << "'. Aborting." << std::endl;
          exit(EXIT_FAILURE);
        }
      }
    } else if (dash_count == 2) {

      if (key == "help") {
          show_help_and_exit(EXIT_SUCCESS);
      }

      else if (key == "nogui" || key == "no-gui") {
        cout << "[Lednicky] Running No-Gui\n";
        set_show_gui(false);
      }

      else if (arg == "radius") {
        std::string radius_param = (val == "") ? *(++arg_it) : val;
        try {
          radius = std::stof(radius_param);
        } catch (std::invalid_argument err_ia) {
          cerr << "Unable to transform radius argument '" << radius_param << "' into a floating point number.\n";
          exit(EXIT_FAILURE);
        }
      }

    // cout << "dash_count "<<dash_count << "\n";
    // std::cout << "Found key '" << key << "'\n";
    // std::cout << "Found value '" << val << "'\n";

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
    else if (arg == "--range" || arg == "--d0") {
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
  return opts;
}
