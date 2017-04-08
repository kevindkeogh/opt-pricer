#include <math.h>
#include <stdlib.h>

double normalcdf(double z)
{
	/* https://www.johndcook.com/blog/cpp_phi/ */
	double a1, a2, a3, a4, a5, p, t, y;
	int sign;

	a1 =  0.254829592;
	a2 = -0.284496736;
	a3 =  1.421413741;
	a4 = -1.453152027;
	a5 =  1.061405429;
	p  =  0.3275911;

	sign = 1;
	if (z < 0)
		sign = -1;
	z = fabs(z) / pow(2, 0.5);
	t = 1 / (1 + p * z);
	y = 1 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-z * z);
	return 0.5 * (1 + sign * y);
}

double gaussrand()
{
	/* Marsaglia and Bray, ``A Convenient Method for Generating Normal Variables'' */
	static double V1, V2, S;
	static int phase = 0;
	double X;

	if (phase == 0) {
		do {
			double U1 = (double)rand() / RAND_MAX;
			double U2 = (double)rand() / RAND_MAX;

			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
		} while (S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);
	} else {
		X = V2 * sqrt(-2 * log(S) / S);
	}

	phase = 1 - phase;

	return X;
}
