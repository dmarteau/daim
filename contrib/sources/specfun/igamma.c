
/* ** igamma.c
 *
 *   DESCRIBE  Approximate the incomplete gamma function P(a, x).
 *
 *                         1     /x  -t   (a-1)
 *             P(a, x) = -_--- * |  e  * t     dt      (a > 0)
 *                       |(a)   /0
 *
 *   CALL      p = igamma(a, x)
 *
 *             double    a    >  0
 *             double    x    >= 0
 *
 *   WARNING   none
 *
 *   RETURN    double    p    [0, 1]
 *                            -1.0 on error condition
 *
 *   XREF      lngamma()
 *
 *   BUGS      Values 0 <= x <= 1 may lead to inaccurate results.
 *
 *   REFERENCE ALGORITHM AS239  APPL. STATIST. (1988) VOL. 37, NO. 3
 *
 * Copyright (c) 1992 Jos van der Woude, jvdwoude@hut.nl
 *
 * Note: this function was translated from the Public Domain Fortran
 *       version available from http://lib.stat.cmu.edu/apstat/239
 *
 */

static double igamma(double a, double x)
{
  double arg;
  double aa;
  double an;
  double b;
  int i;

  /* Check that we have valid values for a and x */
  if (x < 0.0 || a <= 0.0)
    return -1.0;

  /* Deal with special cases */
  if (x == 0.0)
    return 0.0;
  if (x > XBIG)
    return 1.0;

  /* Check value of factor arg */
  arg = a * log(x) - x - GAMMA(a + 1.0);
  /* HBB 20031006: removed a spurious check here */
  arg = gp_exp(arg);

  /* Choose infinite series or continued fraction. */

  if ((x > 1.0) && (x >= a + 2.0)) 
  {
    /* Use a continued fraction expansion */
    double pn1, pn2, pn3, pn4, pn5, pn6;
    double rn;
    double rnold;

    aa = 1.0 - a;
    b = aa + x + 1.0;
    pn1 = 1.0;
    pn2 = x;
    pn3 = x + 1.0;
    pn4 = x * b;
    rnold = pn3 / pn4;

    for (i = 1; i <= ITMAX; i++)
    {
      aa++;
      b += 2.0;
      an = aa * (double) i;

      pn5 = b * pn3 - an * pn1;
      pn6 = b * pn4 - an * pn2;

      if (pn6 != 0.0) 
      {
        rn = pn5 / pn6;
        if (fabs(rnold - rn) <= GPMIN(MACHEPS, MACHEPS * rn))
          return 1.0 - arg * rn * a;

        rnold = rn;
      }
      pn1 = pn3;
      pn2 = pn4;
      pn3 = pn5;
      pn4 = pn6;

      /* Re-scale terms in continued fraction if terms are large */
      if (fabs(pn5) >= OFLOW) 
      {
        pn1 /= OFLOW;
        pn2 /= OFLOW;
        pn3 /= OFLOW;
        pn4 /= OFLOW;
      }
    }
  }
  else
  {
    /* Use Pearson's series expansion. */
    for (i = 0, aa = a, an = b = 1.0; i <= ITMAX; i++) 
    {
      aa++;
      an *= x / aa;
      b += an;
      if (an < b * MACHEPS)
        return arg * b;
    }
   }
   return -1.0;
}
