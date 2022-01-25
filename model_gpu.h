#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<time.h>
#include<omp.h>
#include "./inverseGaussian.h"


#define MIN(x,y) (x > y ? y : x)
#define MAX(x,y) (x > y ? x : y)

#define pi acos(-1.0)
#define MAX_MOD 65535


void sobol_f_(double *, int *, int *, double *, int *, int *,  int *,  int *, int *, int *, int *);
void get_yt(int , int , int , int *, double *, double *, double *, double *, char , double );
double pnorm(double );
