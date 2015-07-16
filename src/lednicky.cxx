///
/// \file lednicky.cxx
/// \breif Implementation of LednickyEquation
///

//Comments:
// 1. Currently, the quantum interference terms are only set up for
// spin 1/2 fermions.  For other spins, would need to make the lednicky eqn more
// generalized.  See Lednicky paper for the proper prefactors.
// 2. This does not include any Coulomb interactions.
// 3. This does not include any residual correlation effects.

#include "lednicky.h"

#include <algorithm>
#include <complex>
#include <cmath>

#include <TH1D.h>
#include <TGraph.h>
#include <TMath.h>
#include <TF1.h>
#include <TH2D.h>
#include <iostream>
#include <TStyle.h>
#include <TApplication.h>
#include <TCanvas.h>
#include "faddeeva.h" //Fast numerical integration package which
// includes the Dawson function

typedef std::complex<double> complex_t;

using std::cout;
using std::endl;
using std::pow;

const double hbarc = 0.19732697;

//Manually set the fit parameters here.
double lamPrimary = 0.2; // lambda parameter for primary pairs
double radius = 3.0; // R_inv
double normalization = 1.0;  // Simple normalization factor
double f0re = -0.071; //Scattering length.  Can be positive or negative
double f0im = 0.0; //Should be zero for baryon-baryon. Nonzero for any baryon-antibaryon.  Not sure about baryon-meson.  For non-zero cases, is > 0 (I don't know of situations where this can be negative).
double d0 = 0.0; //Effective range of interaction.  Should be >= 0
bool identical = false;  //Are the two particles identical?  This turns on/off quantum interference
double maxKstar = 1.5; //Highest k* value of histograms.  Minimum is 0
int totalBins = 1000; //How many bins will the histograms have?  maxKstar/totalBins will be the bin width.

double
get_lednicky_f1 (double z)
{
  return Faddeeva::Dawson(z) / z;
}

double
scattering_amplitude_denominator(double x, const complex_t& f0, double d0)
{
  const double f0real = f0.real(),
               f0im = f0.imag(),
               hbarc_2 = hbarc*hbarc,
               x_2 = x*x;

  double denominatorScatterAmp =
    pow((1+f0im*x/hbarc), 2)          // (1+\frac{\imag{f0}x}{\hbar c})^2
    + pow(f0re*x/hbarc, 2)           // (\frac{\real{f0}x}{\hbar c})^2
    + pow(x_2*d0/(2.0 * hbarc_2)*std::abs(f0), 2) // (|f0| * \frac{x^2*d0}{2(\hbar c)^2})^2
    + x_2*f0re*d0/(hbarc_2); // \frac{x^2 \real{f0} d0}{(\hbar c)^2}

  return denominatorScatterAmp;
}

complex_t
scattering_amplitude_numerator(double x, const complex_t& f0, double d0)
{
  //numerator of real part of scattering amplitude

  double real_part = f0.real() + x*x*d0*std::norm(f0)/(2.*hbarc*hbarc),
         imag_part = f0.imag() + x * std::norm(f0) / hbarc;

  // return pow(real_part, 2) + pow(imag_part, 2);
  return std::complex<double>(real_part, imag_part);
}


void
generate_lednicky_equation(const LednikcyEquation_s& eq)
{
  TF1 f2("function_f2", "(1-exp(-[0]*[0]*x*x))/([0]*x)");
  f2.SetParameter(0, 2.0 * eq.radius / hbarc);
  f2.SetRange(0, eq.maxKstar);

  // Create a vector spaning bin the x axis
  short x_index = 0;
  std::vector<double> X(eq.totalBins);
  std::for_each(X.begin(), X.end(),
    [&x_index](double &n) {
      n = (x_index++ + 0.5) * maxKstar / totalBins;
    });

  // Create kstar and correlation function vectors (y-axis)
  std::vector<double> kstar(eq.totalBins), Cf(eq.totalBins);

  const double radius_squared = pow(eq.radius, 2);

  // for (double x : X) {
  for (unsigned int i = 0; i < X.size(); ++i) {

    const double &x = X[i],
          SQRT_PI = sqrt(M_PI);

    double &cf = Cf[i];


    const double denom = scattering_amplitude_denominator(x, eq.f0, eq.d0);
    const complex_t num = scattering_amplitude_numerator(x, eq.f0, eq.d0);
    const double amplitude = std::norm(num) / (denom * denom);


    cf = 0.5 * (amplitude/radius_squared)*(1. - eq.d0 / (2.0 * SQRT_PI * eq.radius));
    cf += 2*(num.real()/denom) / (SQRT_PI * eq.radius) * get_lednicky_f1(2.* x *radius/hbarc);
    //if(finished) cout<<xBin<<"\t"<<kstar[xBin]<<"\t"<<Cf[xBin]<<"\t"<<2*(realScatterAmp/denominatorScatterAmp)/(TMath::Sqrt(TMath::Pi())*radius) <<endl;
    cf -= (num.imag()/denom) * f2.Eval(x)/eq.radius;

    if (eq.identical) {
     cf *= 0.5; // identical spin 1/2 particles get suppressed by 1/2
     cf -= 0.5 * exp(-4 * pow(x*radius/hbarc, 2));
    }

    cf += 1.0;
  }

}

double
GetLednickyF1(double z)
{
  //
  double result = (1./z)*Faddeeva::Dawson(z);
  return result;
}

TGraph*
GetLednickyEqn(bool identicalParticles)
{
  TF1 funf2("funf2", "(1-exp(-[0]*[0]*x*x))/([0]*x)");
  funf2.SetParameter(0, 2.0 * radius / hbarc);
  funf2.SetRange(0.0, 2.0); //why this range?

  // create a vector with number of bins
  std::vector<double> kstar_v(totalBins);

  float x_index = 0;
  std::for_each(kstar_v.begin(), kstar_v.end(), [&x_index] (double &n) {
    n = (x_index++ + 0.5) * maxKstar / totalBins;
  });

  double *kstar = new double[totalBins];
  // double ykre[100];
  // double ykim[100];
  double *Cf = new double[totalBins];
  // double rad = radius/hbarc;
  double denominatorScatterAmp, realScatterAmp, imaginaryScatterAmp, magnitudeScatterAmp;
  for (int xBin=0; xBin<totalBins; xBin++) {
    //Set the positions of the kstar bins
    kstar[xBin] = (xBin+0.5); // This shifts the center of the bin
    kstar[xBin] *= maxKstar;
    kstar[xBin] /= 1.0*totalBins;
    //Now calculate the denominator of the scattering amplitude
    denominatorScatterAmp = (1+f0im*kstar[xBin]/hbarc)*(1+f0im*kstar[xBin]/hbarc)
      + f0re*f0re*kstar[xBin]*kstar[xBin]/(hbarc*hbarc)
      + kstar[xBin]*kstar[xBin]*kstar[xBin]*kstar[xBin]*d0*d0*(f0re*f0re+f0im*f0im)/(4.*hbarc*hbarc*hbarc*hbarc)
      + kstar[xBin]*kstar[xBin]*f0re*d0/(hbarc*hbarc);
    //Calculate the real and imaginary parts of the scattering amplitude (just the numerators)
    realScatterAmp = f0re + kstar[xBin]*kstar[xBin]*d0*(f0re*f0re+f0im*f0im)/(2.*hbarc*hbarc); //numerator of real part of scattering amplitude
    imaginaryScatterAmp = f0im + kstar[xBin]*(f0re*f0re + f0im*f0im)/(hbarc);
    //Take the magnitude of the scattering amplitude
    magnitudeScatterAmp = (realScatterAmp*realScatterAmp + imaginaryScatterAmp*imaginaryScatterAmp)/(denominatorScatterAmp*denominatorScatterAmp);
    //Cf is the Lednicky and Lyuboshits parameterization of the correlation function, as seen in ALICE K0s-K0s pp paper from 2012.
    Cf[xBin] = 0.5 * (magnitudeScatterAmp/(radius*radius))*(1. - d0/(2.*TMath::Sqrt(TMath::Pi())*radius));
    Cf[xBin] += 2*(realScatterAmp/denominatorScatterAmp)/(TMath::Sqrt(TMath::Pi())*radius)*GetLednickyF1(2.*kstar[xBin]*radius/hbarc);
    //if(finished) cout<<xBin<<"\t"<<kstar[xBin]<<"\t"<<Cf[xBin]<<"\t"<<2*(realScatterAmp/denominatorScatterAmp)/(TMath::Sqrt(TMath::Pi())*radius) <<endl;
    Cf[xBin] -= (imaginaryScatterAmp/denominatorScatterAmp)*funf2.Eval(kstar[xBin])/radius;
    if(identicalParticles){
      Cf[xBin] *= 0.5; // identical spin 1/2 particles get suppressed by 1/2
      Cf[xBin] -= 0.5*exp(-4*pow(kstar[xBin]*radius/hbarc,2));
    }
    Cf[xBin] += 1.0;
  }
  // Make a TGraph of the correlation function
  TGraph *cfGraph = new TGraph(totalBins, kstar, Cf);
  delete kstar;
  delete Cf;
  return cfGraph;
}
