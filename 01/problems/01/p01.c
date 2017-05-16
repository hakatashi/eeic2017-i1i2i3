/*
 * p01.c
 */

/*
 * Usage:
 *
 *  ./a.out
 *
 * Intended behavior
 *
 * It calculate cos^2(t) + sin^2(t) of various values
 * of t, and print them. Thus, it should prints 1.00000
 * on all lines.
 *
 */

#include <stdio.h>
#include <math.h>

double sin2(double);
double cos2(double);

int main()
{
  int i;
  for (i = 0; i < 100; i++) {
    double y = cos2(i) + sin2(i);
    printf("cos^2(%d)+sin^2(%d) = %f\n", i, i, y);
  }
  return 0;
}

double cos2(double x)
{
  double c = cos(x);
  return c * c;
}

double sin2(double x)
{
  double s = sin(x);
  return s * s;
}
