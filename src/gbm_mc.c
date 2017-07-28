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
	double tte, theta_tte, expiry_date, value_date, level, rand, df;
	double delta_shift = 0, vega_shift = 0, theta_shift = 0, rho_shift = 0;
	double gamma_shift = 0, base_gamma = 0, upper_gamma = 0, lower_gamma = 0;
	double price, delta, gamma, vega, theta, rho;
	double base = 0;
	int i;

	if (opt->sims < 1) opt->sims = 1;

	expiry_date = mktime(opt->expiry_date);
	value_date = mktime(opt->value_date);
	tte = difftime(expiry_date, value_date) / (60 * 60 * 24 * 365);
	theta_tte = tte - 1. / 365;

	for (i=0; i<opt->sims; i++) {
		rand = gaussrand();
		/* Base scenario */
		level = gbm_simulation(opt->spot, opt->rfr, opt->vol, tte, rand);
		base += max((level - opt->strike) * opt->type, 0);

		/* Delta scenario */
		level = gbm_simulation(opt->spot + 0.0001, opt->rfr, opt->vol, tte, rand);
		delta_shift += max((level - opt->strike) * opt->type, 0);

		/* Gamma scenario */
		upper_gamma = gbm_simulation(opt->spot + 0.00001, opt->rfr, opt->vol, tte, rand);
		base_gamma = gbm_simulation(opt->spot, opt->rfr, opt->vol, tte, rand);
		lower_gamma = gbm_simulation(opt->spot - 0.00001, opt->rfr, opt->vol, tte, rand);
		gamma_shift += (max((upper_gamma - opt->strike) * opt->type, 0) -
			2 * max((base_gamma - opt->strike) * opt->type, 0) +
			max((lower_gamma - opt->strike) * opt->type, 0)) /
			pow(0.00001, 2);

		/* Vega scenario */
		level = gbm_simulation(opt->spot, opt->rfr, opt->vol + 0.0001, tte, rand);
		vega_shift += max((level - opt->strike) * opt->type, 0);

		/* Theta scenario */
		level = gbm_simulation(opt->spot, opt->rfr, opt->vol, theta_tte, rand);
		theta_shift += max((level - opt->strike) * opt->type, 0);

		/* Rho scenario */
		level = gbm_simulation(opt->spot, opt->rfr + 0.01, opt->vol, tte, rand);
		rho_shift += max((level - opt->strike) * opt->type, 0);
	}

	df = 1 / pow((1 + opt->rfr), tte);
	price = base / opt->sims * df;
	delta = delta_shift / opt->sims * df;
	gamma = gamma_shift / opt->sims * df;
	vega = vega_shift / opt->sims * df;
	theta = theta_shift / opt->sims * 1 / pow((1 + opt->rfr), tte - 1./365.);
	rho = rho_shift / opt->sims * 1 / pow((1 + opt->rfr + 0.01), tte);

	opt->fv = price;
	opt->delta = (delta - price) / 0.0001;
	opt->gamma = gamma;
	opt->vega = (vega - price) / 0.01;
	opt->theta = (theta - price) / (tte - theta_tte);
	opt->rho = (rho - price);
}
