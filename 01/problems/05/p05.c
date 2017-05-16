/*
 * p05.c
 */

/*
 * usage:
 *   ./a.out
 *
 * Intended behavior
 *   It reads the contents of this file and print it.
 */

#include <assert.h>
#include <stdio.h>
int main()
{
  FILE * fp;
  fp = fopen("p05.c", "r");

  if (fp == NULL) {
    printf("hoge\n");
  }

  char buf[100];
  while (1) {
    int n = fread(buf, 1, 100, fp);
    if (n == 0) break;
    fwrite(buf, 1, n, stdout);
  }
  return 0;
}
