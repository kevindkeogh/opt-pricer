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
	double drift, stoch;
	drift = (rfr - pow(vol, 2) / 2) * tte;
	stoch = vol * pow(tte, 0.5) * rand;
	return spot * exp(drift + stoch);

}

double gbm(double spot, double rfr, double vol, double strike, struct tm expiry, struct tm value, int type, int sims)
{
	const gsl_rng_type *T;
	gsl_rng *r;
	double tte, expiry_date, value_date, level, price;
	double *rands, *results;
	int i;

	rands = (double *)malloc(sizeof(double) * sims);
	results = (double *)malloc(sizeof(double) * sims);

	expiry_date = mktime(&expiry);
	value_date = mktime(&value);
	tte = difftime(expiry_date, value_date) / (60 * 60 * 24 * 365);

	/* get array of normal random numbers */
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc(T);

	for (i=0; i<sims; i++) {
		rands[i] = gsl_ran_gaussian(r, 1);
	}

	for (i=0; i<sims; i++) {
		level = gbm_simulation(spot, rfr, vol, tte, rands[i]);
		results[i] = max((level - strike) * type, 0);
	}

	price = gsl_stats_mean(results, 1, sims) * 1 / pow((1 + rfr), tte);

	gsl_rng_free(r);
	free(rands);
	free(results);
	return price;
}
