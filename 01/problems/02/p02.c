/*
 * p02.c
 */

/*
 * Usage:
 *
 *    ./a.out X Y
 *
 * Intended behavior
 *  It should print X * Y if both X and Y can be read as
 *  floating point numbers, or print 0 otherwise.
 *
 * Example
 *    ./a.out 1.2 2.0
 *    2.400000
 *    ./a.out a b
 *    0.000000
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
  double x = atof(argv[1]);
  double y = atof(argv[2]);
  printf("%f\n", x * y);
  return 0;
}
