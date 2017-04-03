#define _XOPEN_SOURCE

#include "black_scholes.h"
#include "utils.h"
#include <cblas.h>
#include <math.h>
#include <time.h>

double bsm(double spot, double rfr, double vol, double strike,
		struct tm expiry, struct tm value, int type)
{
	double d1, d2, tte, price;
	time_t value_date, expiry_date;

	expiry_date = mktime(&expiry);
	value_date = mktime(&value);
	tte = difftime(expiry_date, value_date) / (60 * 60 * 24 * 365);
	d1 = log(spot / strike) + tte * (rfr + pow(vol, 2) / 2)
		/ (vol * pow(tte, 0.5));
	d2 = d1 - (vol * pow(tte, 0.5));

	price = spot * normalcdf(d1 * type) * type -
		strike * exp(-rfr * tte) * normalcdf(d2 * type) * type;
	return price;
}
