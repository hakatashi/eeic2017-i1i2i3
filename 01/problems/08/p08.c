/*
 * p08.c a simple calculator
 */

/*


This is a simple calculator that supports arithmetic
involving '+', '-', '*', '/', and '(...)'.

---------------------

when compiled and run this program crashes with the following
input. please investigate the problem and fix the bug, until this
program outputs the correct result.

$ ./a.out
1+(-2+3)*4-5/6+8/9/10
---------------------

Intended Behavior of this program:

Example

('$' is a prompt, not your input)

  $ ./a.out
  :
  -34.384615

  $ ./a.out
  -2(3)
  syntax error:
  -2(3)
    ^

When run, it reads a line from its standard input,
checks if the line can be read as an arithmetic
expression consisting of
+, -, /, *, and parens '(', and ')'.

Spaces are NOT allowed.

All numbers must be treated as real numbers (not integers).

Examples:

1
1+2
1+2*3   (should print 7, not 9)
(1+2)*3 (should print 9, not 7)
-10


Formal syntax of an expression is 'E' below.

E ::= F (('+'|'-') F)*
F ::= G (('*'|'/') G)*
G ::= ('+'|'-') G | H
H ::= number | '(' E ')'

(...)* expresses an arbitrary number (including zero)
of repetition of '...'.

*/

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAXLINE 1001
char line[MAXLINE];		/* read input here */
char * p;			/* points to the next character in line array */

/*
                 p
                 |
                 |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
line: |2|3|+|3|*|6|7|-|(|3|+|4|)| | | |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

 */

void syntax_error()
{
  fprintf(stderr, "syntax error:\n");
  fprintf(stderr, "%s", line);
  char * q = line;
  while (q < p) {
    fprintf(stderr, " ");
    q++;
  }
  fprintf(stderr, "^\n");
  exit(1);
}

double number()
{
  /* if p points to the beginning of a number,
     return the number and advance p one char past the
     end of the number. otherwise signal syntax error.

before:
             p
      +-+-+-+-+-+-+-+-+-+-+-+-+
line: | | | |6|7|.|3|+|(|1|*|3 |
      +-+-+-+-+-+-+-+-+-+-+-+-+

after:
                     p
      +-+-+-+-+-+-+-+-+-+-+-+-+
line: | | | |6|7|.|3|+|(|1|*|3|
      +-+-+-+-+-+-+-+-+-+-+-+-+


*/
  errno = 0;
  char * endptr[1];
  double x = strtod(p, endptr);
  p = *endptr;
  if (errno) {
    perror("strtod:");
    syntax_error();
  }
  return x;
}

double E_expression();

double H_expression()
{
  /* if p points to the beginning of H expression
     in the definition

     H ::= number | '(' E ')'

     return its value. otherwise signal syntax error
  */

  switch (*p) {
  case '0' ... '9': {
    return number();
  }
  case '(': {
    p++;
    double x = E_expression();
    if (*p == ')') {
      p++;
      return x;
    } else {
      syntax_error();
    }
  }
  default:
    syntax_error();
  }

  return 0.0; // dummy
}

double G_expression()
{
  /* if p points to the beginning of G expression
     in the definition

     G ::= ('+'|'-') G | H

     return its value. otherwise signal syntax error
  */

  switch (*p) {
  case '-': {
    p++;
    return -G_expression();
  }
  case '+': {
    p++;
    return G_expression();
  }
  default: {
    return H_expression();
  }
  }
}

double F_expression()
{
  /* if p points to the beginning of G expression
     in the definition

     F ::= G (('*'|'/') G)*

     return its value. otherwise signal syntax error
  */

  double x = G_expression();
  while (1) {
    if (*p == '*') {
      p++;
      x *= G_expression();
    } else if (*p == '/') {
      p++;
      x /= G_expression();
    } else {
      return x;
    }
  }
}

double E_expression()
{
  /* if p points to the beginning of G expression
     in the definition

     E ::= F (('+'|'-') F)*

     return its value. otherwise signal syntax error
  */

  double x = F_expression();
  while (1) {
    if (*p == '+') {
      p++;
      x += F_expression();
    } else if (*p == '-') {
      p++;
      x -= F_expression();
    } else {
      return x;
    }
  }
}

int main()
{
  char * s = fgets(line, MAXLINE, stdin);
  int n;
  if (s == NULL) { perror("fgets"); exit(1); }
  n = strlen(s);
  if (s[n - 1] != '\n') {
    fprintf(stderr, "line too long (should be < %d chars)\n", MAXLINE - 1);
  }
  p = line;
  double answer = E_expression();
  if (*p != '\n') syntax_error();
  printf("%f\n", answer);
  return 0;
}
