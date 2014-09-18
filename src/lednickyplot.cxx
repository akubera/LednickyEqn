
#include "lednickyplot.h"

LednickyPlot::LednickyPlot():
  _plot(new TGraph("_plot", "LednickyEquation;#it{k}* (GeV/#it{c});C(#it{k}*)", 100, 0.0, 2.0))
{
}

LednickyPlot::~LednickyPlot()
{
  delete _plot;
}
