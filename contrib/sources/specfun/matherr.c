
/*
Cephes Math Library Release 2.0:  April, 1987
Copyright 1984, 1987 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

// XXX FIXME THREAD 
int merror = 0;

/* Notice: the order of appearance of the following messages cannot be bound
 * to error codes defined in mconf.h or math.h or similar, as these files are
 * not available on every platform. Thus, enumerate them explicitly.
 */
#define MTHERR_DOMAIN  1
#define MTHERR_SING  2
#define MTHERR_OVERFLOW  3
#define MTHERR_UNDERFLOW 4
#define MTHERR_TLPREC  5
#define MTHERR_PLPREC  6

static const char *ermsg[7] = {
  "unknown",                  /* error code 0 */
  "domain",                   /* error code 1 */
  "singularity",              /* et seq.      */
  "overflow",
  "underflow",
  "total loss of precision",
  "partial loss of precision"
};

// XXX FIXME Error handling  

static int mtherr(char *name, int code)
{

  /* Display string passed by calling program,
   * which is supposed to be the name of the
   * function in which the error occurred:
   */
  printf("\n%s ", name);

  /* Set global error message word */
  merror = code;

  /* Display error message defined by the code argument.  */
  if ((code <= 0) || (code >= 7))
       code = 0;
 
  printf("%s error\n", ermsg[code]);

  /* Return to calling program */
  return (0);
}
