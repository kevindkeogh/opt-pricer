/* vim:filetype=c
 */
#ifndef GBM_MC
#define GBM_MC

#include <time.h>


struct Option;

double gbm_simulation(double spot, double rfr, double vol, double tte, double rand);

void *run_simulation(void *opt_ptr);

void gbm(struct Option *opt);

#endif
