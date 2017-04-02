#ifndef BLACK_SCHOLES_H_
#define BLACK_SCHOLES_H_

struct tm;

double gbm(double spot, double rfr, double vol, double strike, struct tm expiry,
		struct tm value, int type, int sims);

double normalcdf(double value);

double bsm(double spot, double rfr, double vol, double strike, struct tm expiry,
		struct tm value, int type);

#endif
