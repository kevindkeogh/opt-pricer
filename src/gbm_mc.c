#define _XOPEN_SOURCE
#define max(X,Y) (((X) > (Y)) ? (X) : (Y))

#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
#include <time.h>
#include "gbm_mc.h"

double gbm_simulation(double spot, double rfr, double vol, double tte, double rand)
{
	/* S_T = S_0e^{[r-\frac{\sigma^2}{2}]t + \sigma\sqrt{t}\xi_i} */
	double drift, stoch;
	drift = (rfr - pow(vol, 2) / 2) * tte;
	stoch = vol * pow(tte, 0.5) * rand;
	return spot * exp(drift + stoch);

}

double gbm(double spot, double rfr, double vol, double strike, struct tm expiry,
		struct tm value, int type, int sims)
{
	const gsl_rng_type *T;
	double tte, expiry_date, value_date, level, price, rand;
	double *results;
	gsl_rng *r;
	int i;

	results = (double *)malloc(sizeof(double) * sims);

	expiry_date = mktime(&expiry);
	value_date = mktime(&value);
	tte = difftime(expiry_date, value_date) / (60 * 60 * 24 * 365);

	/* GSL RNG setup */
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc(T);

	for (i=0; i<sims; i++) {
		rand = gsl_ran_gaussian(r, 1);
		level = gbm_simulation(spot, rfr, vol, tte, rand);
		results[i] = max((level - strike) * type, 0);
	}

	price = gsl_stats_mean(results, 1, sims) * 1 / pow((1 + rfr), tte);

	gsl_rng_free(r);
	free(results);
	return price;
}
