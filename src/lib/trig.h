/* 
 * This basic library implements some basic trigonometry functions required for
 * calculating the trajectory of an angry nerd.
 */
#ifndef TRIG_H
#define TRIG_H

/* Constants required for trig functions. */
#define CONST_PI  3.14159265
#define CONST_2PI 6.28318531
#define CONST_G 9.81
#define modd(x, y) ((x) - (int)((x) / (y)) * (y))

/* Parameters: x in radians. */
double sin(double x);
double cos(double x);
double tan(double x);
double acosf(double x);

/* Converts an angle in degrees to radians. */
double deg_to_rad(int degrees);

#endif /* TRIG_H */
