
#ifndef LEDNICKY_PLOT_H
#define LEDNICKY_PLOT_H

#include <TGraph.h>

class LednickyPlot {
public:
  LednickyPlot();
  virtual ~LednickyPlot();

protected:
  TGraph *_plot;
};

#endif /* LEDNICKY_PLOT_H */
