#include <math.h>

/*
 * COPYRIGHT:        See COPYING in the top level directory
 * PROJECT:          ReactOS CRT
 * FILE:             lib/crt/math/cos.c
 * PURPOSE:          Generic C Implementation of cos
 * PROGRAMMER:       Timo Kreuzer (timo.kreuzer@reactos.org)
 */

#define PRECISION 9
#define M_PI 3.14
static double cos_off_tbl[] = {0.0, -M_PI/2., 0, -M_PI/2.};
static double cos_sign_tbl[] = {1,-1,-1,1};

static double sin_off_tbl[] = {0.0, -M_PI/2., 0, -M_PI/2.};
static double sin_sign_tbl[] = {1,-1,-1,1};

double sin(double x)
{
    int quadrant;
    double x2, result;

    /* Calculate the quadrant */
    quadrant = x * (2./M_PI);

    /* Get offset inside quadrant */
    x = x - quadrant * (M_PI/2.);

    /* Normalize quadrant to [0..3] */
    quadrant = (quadrant - 1) & 0x3;

    /* Fixup value for the generic function */
    x += sin_off_tbl[quadrant];

    /* Calculate the negative of the square of x */
    x2 = - (x * x);

    /* This is an unrolled taylor series using <PRECISION> iterations
     * Example with 4 iterations:
     * result = 1 - x^2/2! + x^4/4! - x^6/6! + x^8/8!
     * To save multiplications and to keep the precision high, it's performed
     * like this:
     * result = 1 - x^2 * (1/2! - x^2 * (1/4! - x^2 * (1/6! - x^2 * (1/8!))))
     */

    /* Start with 0, compiler will optimize this away */
    result = 0;

#if (PRECISION >= 10)
    result += 1./(1.*2*3*4*5*6*7*8*9*10*11*12*13*14*15*16*17*18*19*20);
    result *= x2;
#endif
#if (PRECISION >= 9)
    result += 1./(1.*2*3*4*5*6*7*8*9*10*11*12*13*14*15*16*17*18);
    result *= x2;
#endif
#if (PRECISION >= 8)
    result += 1./(1.*2*3*4*5*6*7*8*9*10*11*12*13*14*15*16);
    result *= x2;
#endif
#if (PRECISION >= 7)
    result += 1./(1.*2*3*4*5*6*7*8*9*10*11*12*13*14);
    result *= x2;
#endif
#if (PRECISION >= 6)
    result += 1./(1.*2*3*4*5*6*7*8*9*10*11*12);
    result *= x2;
#endif
#if (PRECISION >= 5)
    result += 1./(1.*2*3*4*5*6*7*8*9*10);
    result *= x2;
#endif
    result += 1./(1.*2*3*4*5*6*7*8);
    result *= x2;

    result += 1./(1.*2*3*4*5*6);
    result *= x2;

    result += 1./(1.*2*3*4);
    result *= x2;

    result += 1./(1.*2);
    result *= x2;

    result += 1;

    /* Apply correct sign */
    result *= sin_sign_tbl[quadrant];

    return result;
}

double cos(double x)
{
    int quadrant;
    double x2, result;

    /* Calculate the quadrant */
    quadrant = x * (2./M_PI);

    /* Get offset inside quadrant */
    x = x - quadrant * (M_PI/2.);

    /* Normalize quadrant to [0..3] */
    quadrant = quadrant & 0x3;

    /* Fixup value for the generic function */
    x += cos_off_tbl[quadrant];

    /* Calculate the negative of the square of x */
    x2 = - (x * x);

    /* This is an unrolled taylor series using <PRECISION> iterations
     * Example with 4 iterations:
     * result = 1 - x^2/2! + x^4/4! - x^6/6! + x^8/8!
     * To save multiplications and to keep the precision high, it's performed
     * like this:
     * result = 1 - x^2 * (1/2! - x^2 * (1/4! - x^2 * (1/6! - x^2 * (1/8!))))
     */

    /* Start with 0, compiler will optimize this away */
    result = 0;

#if (PRECISION >= 10)
    result += 1./(1.*2*3*4*5*6*7*8*9*10*11*12*13*14*15*16*17*18*19*20);
    result *= x2;
#endif
#if (PRECISION >= 9)
    result += 1./(1.*2*3*4*5*6*7*8*9*10*11*12*13*14*15*16*17*18);
    result *= x2;
#endif
#if (PRECISION >= 8)
    result += 1./(1.*2*3*4*5*6*7*8*9*10*11*12*13*14*15*16);
    result *= x2;
#endif
#if (PRECISION >= 7)
    result += 1./(1.*2*3*4*5*6*7*8*9*10*11*12*13*14);
    result *= x2;
#endif
#if (PRECISION >= 6)
    result += 1./(1.*2*3*4*5*6*7*8*9*10*11*12);
    result *= x2;
#endif
#if (PRECISION >= 5)
    result += 1./(1.*2*3*4*5*6*7*8*9*10);
    result *= x2;
#endif
    result += 1./(1.*2*3*4*5*6*7*8);
    result *= x2;

    result += 1./(1.*2*3*4*5*6);
    result *= x2;

    result += 1./(1.*2*3*4);
    result *= x2;

    result += 1./(1.*2);
    result *= x2;

    result += 1;

    /* Apply correct sign */
    result *= cos_sign_tbl[quadrant];

    return result;
}

static const int N = 100;

double coef(int n)
{
 	double t;

	if (n == 0) 
	{
		return 0;
	}

 	t = 1.0/n;

 	if (n%2 == 0) 
	{
		t = -t;
	}

 	return t;
}

double horner(double x)
{
	double u = coef(N);
 	int i;

 	for(i=N-1; i>=0; i--)
	{
  		u = u*x + coef(i);
	}

 	return u;
}

double sqrt(double b)
{
 	double x = 1;
	int step = 0;

 	while ((x*x-b<-0.000000000000001 || x*x-b>0.000000000000001) && step<50)
 	{
		x = (b/x+x)/2.0;
		step++;
	}
 	return x;
}

double ln(double x)
{
	int i;

 	if (x > 1.5)
 	{
  		for(i=0; x>1.25; i++)
		{
   			x = sqrt(x);
		}
  		return (1<<i)*horner(x-1);
 	}
 	else if (x<0.7 && x>0)
 	{
  		for(i=0; x<0.7; i++)
		{
   			x = sqrt(x);
		}
  		return (1<<i)*horner(x-1);
 	}
 	else if(x > 0)
	{
  		return horner(x-1);
	}
}

double exp(double x)
{
	double sum = 1;
 	int i;

 	for(i=N; i>0; i--)
 	{ 
	  	sum /= i;
	  	sum *= x;
	  	sum += 1;
 	}
 	return sum;
}

double pow(double m, double n)
{
	return exp(n*ln(m));
}

