#ifndef UTILS_H
#define UTILS_H

double normalcdf(double z);

double normalpdf(double z);

double gaussrand(void);

struct Option {
	/* option details */
	int type;
	double strike;
	struct tm *expiry_date;

	/* market data */
	struct tm *value_date;
	double spot;
	double rfr;
	double vol;
	long sims;
	double *randoms;

	/* fv and greeks */
	double fv;
	double delta;
	double vega;
	double rho;
	double gamma;
	double theta;
};

#endif
