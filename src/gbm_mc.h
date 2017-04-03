#ifndef GBM_MC
#define GBM_MC

#include <time.h>

double gbm_simulation(double spot, double rfr, double vol, double tte, double rand);

double gbm(double spot, double rfr, double vol, double strike, struct tm expiry, struct tm value, int type, int sims);

#endif
