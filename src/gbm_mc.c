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

void gbm(struct Option *opt)
{
	double tte, expiry_date, value_date, level, rand;
	double delta_shift = 0, vega_shift = 0, theta_shift = 0, rho_shift = 0;
	double price, delta, vega, theta, rho;
	double base = 0;
	int i;

	if (opt->sims < 1) opt->sims = 1;

	expiry_date = mktime(opt->expiry_date);
	value_date = mktime(opt->value_date);
	tte = difftime(expiry_date, value_date) / (60 * 60 * 24 * 365);

	for (i=0; i<opt->sims; i++) {
		rand = gaussrand();
		level = gbm_simulation(opt->spot, opt->rfr, opt->vol, tte, rand);
		base += max((level - opt->strike) * opt->type, 0);
		level = gbm_simulation(opt->spot + 1, opt->rfr, opt->vol, tte, rand);
		delta_shift += max((level - opt->strike) * opt->type, 0);
		level = gbm_simulation(opt->spot, opt->rfr, opt->vol + 0.01, tte, rand);
		vega_shift += max((level - opt->strike) * opt->type, 0);
		level = gbm_simulation(opt->spot, opt->rfr, opt->vol, tte - 1./365., rand);
		theta_shift += max((level - opt->strike) * opt->type, 0);
		level = gbm_simulation(opt->spot, opt->rfr + 0.01, opt->vol, tte, rand);
		rho_shift += max((level - opt->strike) * opt->type, 0);
	}

	price = base / opt->sims * 1 / pow((1 + opt->rfr), tte);
	delta = delta_shift / opt->sims * 1 / pow((1 + opt->rfr), tte);
	vega = vega_shift / opt->sims * 1 / pow((1 + opt->rfr), tte);
	theta = theta_shift / opt->sims * 1 / pow((1 + opt->rfr), tte - 1./365.);
	rho = rho_shift / opt->sims * 1 / pow((1 + opt->rfr + 0.01), tte);

	opt->fv = price;
	opt->delta = (delta - price);
	opt->vega = (vega - price);
	opt->theta = (theta - price);
	opt->rho = (rho - price);
}
