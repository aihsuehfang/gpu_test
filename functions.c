#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "./functions.h"

#pragma omp declare target 

void sobol_f_(double *, int *, int *, double *, int *, int *,  int *,  int *, int *, int *, int *);

void ytassign(double *y, double *dt_y, int m, int n, int mi,int idx, double *y_mat, double *dt_mat){
     int j;
     for(j=0;j<mi;j++){
        y[j]=y_mat[idx*m+j];
        dt_y[j]=dt_mat[idx*m+j];
     }
     return;
}


double *baker_trans(int n, double *x){
    int i;
    double *y=malloc(sizeof(double)*n);
    for(i=0;i<n;i++){
        y[i]=1.0-x[i];
    }
    return(y);
}

void qmc_int0(int idx, int iseed, int n1, int n2, int m, int n_p0, double q, double *y, double *dt,
             double *pars, double *ans, char *method){
    int j1,j2;
    int m1=m; // use ALD directly
    int maxbit=30;
    int trans=0;
    int sv[m1*maxbit];
    int count;
    int init=1;
    int n=1;
    int iflag=1;
    int ll;
    int cnt;
    int n_p;
    char cm=method[0];
    char cg=method[1];
    double *fval_list=malloc(sizeof(double)*n1);
    double *cnt_list=malloc(sizeof(double)*n1);
    double *quasi=malloc(sizeof(double)*m1);
    double *u1=malloc(sizeof(double)*m1);
    double *u2=malloc(sizeof(double)*m1);
    sobol_f_(u1,&n,&m1,quasi,&ll,&count, sv, &iflag, &iseed, &init, &trans);
    init=0;
    for(j1=0;j1<n1;j1++){
        for(j2=0;j2<n2;j2++){
            // obtain u2
            u2=baker_trans(m1,u1);
            sobol_f_(u1,&n,&m1,quasi,&ll,&count, sv, &iflag, &iseed, &init, &trans);
        }
    }
    ans[0]=1;
    ans[1]=0;
    ans[2]=0;
    return;
} 


#pragma omp end declare target 

void loglik_q(double *pars1, int *n_p0, int *n0, int *m0, double *q0, double *Y1, 
        double *t_mat, char *method, double *fval0, double *ans_mat, int *nn1, 
        int *nn2, int *mi_vec){
    int m=m0[0];
    int n=n0[0];
    int n_all=m*n;
    int n_p=n_p0[0];
   /* 
    int mi;
    int iseed;
    */
    int i,k;
    int n1=nn1[0];
    int n2=nn2[0];
    int n_omp=n;
/*
    int ires=0;
    char cg=method[1];
    char cm=method[0];
    char me=method[2];
*/
    //double ga=1.0;
    /*
    double y[m];
    double dt_y[m];
    double ans[3];
    */
    double q=q0[0];
    //int idx[m];
    double fval;
    //double seed;
    double *seed_set=malloc(sizeof(double)*n);
    /*
    dsfmt_t dsfmt;
    dsfmt_init_gen_rand(&dsfmt,time(NULL));
    */
    printf("[DEBUG][loglik]n_p=%d\n",n_p);
    for(k=0;k<n_p;k++)
        printf("[DEBUG][loglik] %d %lf \n",k,pars1[k]);

    n_omp=MIN(n,50);
    printf("ok\n");

    printf("n_omp=%d\n",n_omp);

    srand((unsigned) time(NULL));
    fval=0.0;
    for(i=0;i<n;i++){
      seed_set[i]=(double) rand()/RAND_MAX;
    }
    printf("[DEBUG] omp_get_num_teams()=%d\n",omp_get_num_teams());
    #pragma omp target data map(from:Y1[0:n_all],t_mat[0:n_all], method[0:3], \
        mi_vec[0:n], n1[0:1], n2[0:1], seed_set[0:n], pars1[0:n_p],q[0:1],n[0:1]) \
        map(to:fval[0:1],ans_mat[0:(3*n)])
    #pragma omp target teams num_teams(9) 
    {
    #pragma omp distribute parallel for 
    for(i=0;i<n;i++){
        printf("[DEBUG] %d omp_get_num_teams()=%d\n",i,omp_get_num_teams());
        int mi=mi_vec[i];
        int iseed;
        double y[m];
        double dt_y[m];
        double ans[3];

        iseed=(int)(seed_set[i] * (double) MAX_MOD);
        ytassign( y, dt_y,  m,  n,  mi, i, Y1, t_mat);
        
        qmc_int0(i,iseed, n1, n2, mi, n_p, q, y, dt_y, pars1, ans, method);
        printf("[LOG][loglik] i=%d fval=%lf,%lf (%lf) rate=%lf seed=%d\n", i,ans[0],fval,ans[1], ans[2],iseed); 
        ans_mat[i]=ans[0];
        ans_mat[n+i]=ans[1];
        ans_mat[i*n+2]=ans[2];
        fval+=ans[0];
   } // for 
   }
  
    *fval0=fval;
    printf("[RES] fval=%20.10lf\n",fval0[0]);
    return;
}
