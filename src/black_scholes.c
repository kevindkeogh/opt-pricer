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
	opt->vega = opt->spot * exp(-opt->rfr * tte) * pow(tte, 0.5) * normalpdf(d1);
}
