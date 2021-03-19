#include "trig.h"

/*
 * Simple approximation of cos(x) using a 6th-order Taylor polynomial.
 * Citation: https://web.eecs.utk.edu/~azh/blog/cosine.html
 */
double cos(double x)
{
    x = modd(x, CONST_2PI);
    char sign = 1;
    if (x > CONST_PI)
    {
        x -= CONST_PI;
        sign = -1;
    }
    double xx = x * x;

    return sign * (1 - ((xx) / (2)) + ((xx * xx) / (24)) - ((xx * xx * xx) / (720)) + ((xx * xx * xx * xx) / (40320)) - ((xx * xx * xx * xx * xx) / (3628800)) + ((xx * xx * xx * xx * xx * xx) / (479001600)));
}

/*
 * Use sinx = cos(x-pi/2).
 */
double sin(double x) 
{
    return cos(x - CONST_PI/2);
}

/*
 * Use tanx = sinx/cosx.
 */
double tan(double x) 
{
    return sin(x)/cos(x);
}

/*
 * Converts radians to degrees.
 */
double deg_to_rad(int degrees) 
{
    return (double)(((double)degrees) * CONST_PI / 180);
}

double acosf(double x) {
    double arcsin = x + (x*x*x) / (2*3) + (3 * x*x*x*x*x) / (2*4*5) + (3 * 5 * x*x*x*x*x*x*x) / (2*4*6*7) + (3 * 5 * 7 * x*x*x*x*x*x*x
    return CONST_PI/2 - arcsin;
}
