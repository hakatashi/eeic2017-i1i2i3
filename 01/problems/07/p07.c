/*
 * p07.c
 *
 * usage:
 *
 *   ./a.out filename
 *
 * Intended behavior
 *
 * It first reads the contents of the file given
 * in the command line, and print characters in it
 * in the reverse order.
 * Since the size of the file is unknown, it allocates
 * memory dynamically and grows it when necessary
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

int main(int argc, char ** argv)
{
  /* try to read the whole contents of
     the file into s */
  FILE * fp = fopen(argv[1], "rb");
  /* s is initially small (10 bytes) */
  int begin = 0;
  int end = 10;
  char * s = (char *)malloc(end * 1000);
  while (1) {
    /* read as many bytes as s can store ((end - begin) bytes) */
    int r = fread(s + begin, 1, end - begin, fp);
    if (r < end - begin) {
      /* reached end of file */
      end = begin + r;
      break;
    }
    /* s is full and we have not reached end of file.
       we extend s by 10 bytes */
    begin = end;
    end += 10;
  }
  /* print s from end to beginning */
  int i;
  for (i = end - 1; i >= 0; i--) {
    putchar(s[i]);
  }
  free(s);
  return 0;
}
