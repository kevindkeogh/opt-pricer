#define _XOPEN_SOURCE

#include "black_scholes.h"
#include "gbm_mc.h"
#include "utils.h"

#ifdef __MINGW32__
#include "strptime.h"
#endif

#include <locale.h>
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


char* read_type(struct Option opt)
{
	return (opt.type == 1) ? "Call" : "Put";
}


int main(int argc, char *argv[])
{
	double spot = 0, strike = 0, rfr = 0, vol = 0, sims = 1000;
	char expiry_date[11], value_date[11];
	int opt, option_index = 0, type = 0;
	struct tm value, expiry;
	struct Option bs_opt, mc_opt;

	extern char *optarg;
	extern int getopt_long();

	memset(&expiry, 0, sizeof(expiry));
	memset(&value, 0, sizeof(value));

	srand(time(NULL));

	while ((opt = getopt_long(argc, argv, "s:k:r:v:d:e:N:cph", long_options, &option_index)) != -1) {
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

	bs_opt.spot        = mc_opt.spot        = spot;
	bs_opt.strike      = mc_opt.strike      = strike;
	bs_opt.expiry_date = mc_opt.expiry_date = &expiry;
	bs_opt.value_date  = mc_opt.value_date  = &value;
	bs_opt.rfr         = mc_opt.rfr         = rfr;
	bs_opt.vol         = mc_opt.vol         = vol;
	bs_opt.type        = mc_opt.type        = type;
	mc_opt.sims = sims;

	/*
	mc_opt.spot = spot;
	mc_opt.strike = strike;
	mc_opt.expiry_date = &expiry;
	mc_opt.value_date = &value;
	mc_opt.rfr = rfr;
	mc_opt.vol = vol;
	mc_opt.type = type;
	*/

	strftime(expiry_date, 11, "%Y-%m-%d", &expiry);
	strftime(value_date, 11, "%Y-%m-%d", &value);
	bsm(&bs_opt);
	gbm(&mc_opt);

	setlocale(LC_ALL,"");
	printf(
		"\nValuation date: %s\n\n"
		"	              | BS Analytic | BS Monte Carlo |\n"
		"	 ---------------------------------------------\n"
		"	 |Type:       | %10s  | %13s  |\n"
		"	 |Spot:       | %10.2f  | %13.2f  |\n"
		"	 |Expiry:     | %s  |    %s  |\n"
		"	 |Strike:     | %10.2f  | %13.2f  |\n"
		"	 |Risk-free:  | %10.2f%% | %13.2f%% |\n"
		"	 |Implied Vol:| %10.2f%% | %13.2f%% |\n"
		"	 ---------------------------------------------\n"
		"	 |Fair value: |   %8.4f  |   %11.4f  |\n"
		"	 |Delta:      |   %8.4f  |   %11.4f  |\n"
		"	 |Gamma:      |   %8.4f  |   %11.4f  |\n",
		value_date,
		read_type(bs_opt), read_type(mc_opt),
		bs_opt.spot, mc_opt.spot,
		expiry_date, expiry_date,
		bs_opt.strike, mc_opt.strike,
		bs_opt.rfr*100, mc_opt.rfr*100,
		bs_opt.vol*100, mc_opt.vol*100,
		bs_opt.fv, mc_opt.fv,
		bs_opt.delta, mc_opt.delta,
		bs_opt.gamma, mc_opt.gamma);
	printf(
		"	 |Vega:       |   %8.4f  |   %11.4f  |\n"
		"	 |Theta:      |   %8.4f  |   %11.4f  |\n"
		"	 |Rho:        |   %8.4f  |   %11.4f  |\n"
		"	 |Simulations:|             |   %'11ld  |\n"
		"	 ---------------------------------------------\n\n",
		bs_opt.vega, mc_opt.vega,
		bs_opt.theta, mc_opt.theta,
		bs_opt.rho, mc_opt.rho,
		            mc_opt.sims);

	return 0;
}
