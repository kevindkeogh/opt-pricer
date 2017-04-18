#define _XOPEN_SOURCE

#include "black_scholes.h"
#include "gbm_mc.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
	double spot = 0, strike = 0, rfr = 0, vol = 0, sims = 1000;
	double bs_price, mc_price;
	char expiry_date[11], buffer[512];
	int opt, type = 0;
	struct tm expiry, value;

    extern char *optarg;
    extern int getopt();

	memset(&expiry, 0, sizeof(expiry));
	memset(&value, 0, sizeof(value));

	while ((opt = getopt(argc, argv, "s:k:r:v:d:e:n:cp")) != -1) {
		switch (opt) {
		case 's': /* spot */
			if (sscanf(optarg, "%lf", &spot) == EOF) {
				return 1;
			};
			break;
		case 'k': /* strike */
			if (sscanf(optarg, "%lf", &strike) == EOF) {
				return 1;
			};
			break;
		case 'r': /* risk-free rate */
			if (sscanf(optarg, "%lf.", &rfr) == EOF) {
				return 1;
			};
			break;
		case 'v': /* implied volatility */
			if (sscanf(optarg, "%lf", &vol) == EOF) {
				return 1;
			};
			break;
		case 'e': /* expiry date, must be YYYY-MM-DD */
			strptime(optarg, "%Y-%m-%d", &expiry);
			break;
		case 'd': /* valuation date, must be YYYY-MM-DD */
			strptime(optarg, "%Y-%m-%d", &value);
			break;
		case 'n': /* number of simulations */
			if (sscanf(optarg, "%lf", &sims) == EOF) {
				return 1;
			};
			break;
		case 'c': /* set as call */
			type = 1;
			break;
		case 'p': /* set as put */
			type = -1;
			break;
		}
	}

	strftime(expiry_date, 11, "%Y-%m-%d", &expiry);
	bs_price = bsm(spot, rfr, vol, strike, expiry, value, type);
	mc_price = gbm(spot, rfr, vol, strike, expiry, value, type, sims);
	sprintf(&buffer[0], "Arguments:\n"
			"spot: %f\n"
			"strike: %f\n"
			"rfr: %f\n"
			"vol: %f\n"
			"sims: %f\n"
			"exp: %s\n"
			"BS price: %f\n"
			"MC price: %f\n",
			spot, strike, rfr, vol, sims, expiry_date, bs_price, mc_price);
	printf("%s", buffer);
	return 0;
}
