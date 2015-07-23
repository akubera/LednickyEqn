///
/// \file lednicky.h
/// \brief Main include file for LednickyEqn
///

#pragma once

#include <TGraph.h>
#include <complex>

typedef unsigned short ushort_t;
typedef struct LednickyEquation LednickyEquation_s;

/**
 * LednickyEquation
 * \brief Structure housing all parameters used by the Lednicky equations.
 *
 * This structure replaces the global vraiables used in the original code,
 * allowing multiple Lednicky equations to be analyzed at the same time.
 */
struct LednickyEquation {
  /// Are the two particles identical?  This turns on/off quantum interference
  bool identical;

  /// Number of bins to give created histograms
  ushort_t totalBins;

  /// Normalization Factor
  double normalization;

  /// Lambda Parameter for primary pairs
  double lamPrimary;

  /// Maximum k* on histograms
  double maxKstar;

  /// Femtoscopic Radius
  double radius;

  /// Effective range of interaction (should be >= 0)
  double d0;

  // The f0 parameter
  std::complex<double> f0;

  /// Real part of f0
  double f0re;

  /// Imaginary part of f0
  double f0im;
};

extern bool identical;  //
extern int totalBins; //How many bins will the histograms have?  maxKstar/totalBins will be the bin width.
extern double normalization;  //Simple normalization factor
extern double lamPrimary; // lambda parameter for primary pairs
extern double maxKstar;
extern double radius;
extern double d0; //
extern double f0re;
extern double f0im;


TGraph* GetLednickyEqn(bool identicalParticles);
