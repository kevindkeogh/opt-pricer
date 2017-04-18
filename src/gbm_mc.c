#define _XOPEN_SOURCE
#define max(X,Y) (((X) > (Y)) ? (X) : (Y))

#include "gbm_mc.h"
#include "utils.h"
#include <math.h>
#include <time.h>

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
	double tte, expiry_date, value_date, level, price, rand;
	double results = 0;
	int i;

	if (sims < 1) sims = 1;

	expiry_date = mktime(&expiry);
	value_date = mktime(&value);
	tte = difftime(expiry_date, value_date) / (60 * 60 * 24 * 365);

	for (i=0; i<sims; i++) {
		rand = gaussrand();
		level = gbm_simulation(spot, rfr, vol, tte, rand);
		results += max((level - strike) * type, 0);
	}

	price = results / sims * 1 / pow((1 + rfr), tte);

	return price;
}
