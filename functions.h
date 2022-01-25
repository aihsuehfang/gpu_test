
#include<math.h>
#include<stdbool.h>
#include<time.h>
#include<omp.h>
#include<cuda.h>

#define MIN(x,y) (x > y ? y : x)
#define MAX(x,y) (x > y ? x : y)

#define pi acos(-1.0)
#define MAX_MOD 65535
#define MIN_PRECITION 1E-20

#define SQ(x) ((x)*(x))

void sobol_f_(double *, int *, int *, double *, int *, int *,  int *,  int *, int *, int *, int *);
double *baker_trans(int , double *);
void qmc_int0(int, int, int, int, int, int, double, double *, double *, double *, double *, char *);
void loglik_q(double *, int *, int *, int *, double *, double *, double *, char *, double *, double *, int *, int *, int *);
