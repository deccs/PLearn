#ifndef qld_INC
#define qld_INC

/**
 * Declaration of the Fortran QL0001 routine
 */

int ql0001_(int *m,      int *me,     int *mmax,
            int *n,      int *nmax,   int *mnn,
            double *c,   double *d,   double *a,
            double *b,   double *xl,  double *xu,
            double *x,   double *u,
            int *iout,   int *ifail,  int *iprint,
            double *war, int *lwar,
            int *iwar,   int *liwar,
            double *eps1);

#endif
