#define _XOPEN_SOURCE

#include "black_scholes.h"
#include "utils.h"
#include <math.h>
#include <time.h>


void bsm(struct Option *opt)
{
	double d1, d2, tte, price;
	time_t value_date, expiry_date;

	expiry_date = mktime(opt->expiry_date);
	value_date = mktime(opt->value_date);
	tte = difftime(expiry_date, value_date) / (60 * 60 * 24 * 365);

	d1 = (log(opt->spot / opt->strike) + tte * (opt->rfr + pow(opt->vol, 2) / 2))
		/ (opt->vol * pow(tte, 0.5));
	d2 = d1 - (opt->vol * pow(tte, 0.5));

	price = opt->spot * normalcdf(d1 * opt->type) * opt->type - opt->strike *
		exp(-opt->rfr * tte) * normalcdf(d2 * opt->type) * opt->type;
	opt->fv = price;
	opt->delta = (opt->type == 1) ? normalcdf(d1) : normalcdf(d1) - 1;
	opt->gamma = 1 / (opt->spot * opt->vol * pow(tte, 0.5)) * normalcdf(d1);
	opt->vega = opt->spot / 100 * pow(tte, 0.5) * normalpdf(d1);
	opt->rho = opt->type * tte * opt->strike * exp(-opt->rfr * tte) * normalcdf(opt->type * d2) / 100;
	opt->theta = -opt->type * (opt->rfr * opt->strike * exp(-opt->rfr * tte) * normalcdf(opt->type * d2)) - (opt->vol / (2 * pow(tte, 0.5)) * opt->spot * normalpdf(opt->type * d1));
}
