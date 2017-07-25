#ifndef GBM_MC
#define GBM_MC

#include <time.h>

struct Option;
double gbm_simulation(double spot, double rfr, double vol, double tte, double rand);

void gbm(struct Option *opt);

#endif
