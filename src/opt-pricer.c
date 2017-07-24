#define _XOPEN_SOURCE

#include "black_scholes.h"
#include "gbm_mc.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <getopt.h>


struct helptext {
	const char *opt;
	const char *desc;
};


static struct helptext options[] = {
	{"-s, --spot",
	 "Spot price"},
	{"-k, --strike",
	 "Strike price"},
	{"-r, --rfr",
	 "Risk-free rate"},
	{"-v, --implied-volatility",
	 "Implied volatility"},
	{"-d, --effective-date",
	 "Effective date [YYYY-MM-DD]"},
	{"-e, --expiry-date",
	 "Expiry date [YYYY-MM-DD]"},
	{"-N",
	 "Number of MC simulations"},
	{"-c, --call",
	 "Call flag"},
	{"-p, --put",
	 "Put or call flag"},
	{"-h, --help",
	 "This help text"},
	{ NULL , NULL }
};


static struct option long_options[] = {
	{"spot",		required_argument,	0,	's'},
	{"strike",		required_argument,	0,	'k'},
	{"rfr",			required_argument,	0,	'r'},
	{"implied-volatility",	required_argument,	0,	'v'},
	{"effective-date",	required_argument,	0,	'd'},
	{"expiry-date",		required_argument,	0,	'e'},
	{"call",		no_argument,		0,	'c'},
	{"put",			no_argument,		0,	'p'},
	{"help",		no_argument,		0,	'h'},
	{0, 0, 0, 0}
};


int print_help(void)
{
	int i;
	printf("Usage: opt-pricer [options...]\n");
	for(i=0; options[i].opt; i++) {
		printf(" %-25s %s\n", options[i].opt, options[i].desc);
	}
	return 0;
}


int main(int argc, char *argv[])
{
	double spot = 0, strike = 0, rfr = 0, vol = 0, sims = 1000;
	double bs_price, mc_price;
	char expiry_date[11], buffer[512];
	int opt, option_index = 0, type = 0;
	struct tm expiry, value;

    extern char *optarg;
    extern int getopt();

	memset(&expiry, 0, sizeof(expiry));
	memset(&value, 0, sizeof(value));

	while ((opt = getopt_long(argc, argv, "s:k:r:v:d:e:N:cp:h", long_options, &option_index)) != -1) {
		switch (opt) {
		case 's':
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
		case 'N': /* number of simulations */
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
		case 'h': /* print help*/
			print_help();
			return 0;
		}
	}
	if (spot == 0 || strike == 0 || rfr == 0 || vol == 0) {
		print_help();
		return 1;
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
