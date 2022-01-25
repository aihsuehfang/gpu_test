/*
 windows needs quadmath library
*/

#include "model_ig.h"

// laser data
/*
int n_unit=15;
int n_times=16;

double tp[17]={0,250,500,750,1000,1250,1500,1750,2000,2250,2500,2750,3000,3250,3500,3750,4000};
double dt[16]={250,250,250,250,250,250,250,250,250,250,250,250,250,250,250,250};
*/




// Gamma functions
double GF_linear(double x, double ga){
    return x;
}

double GF_power(double x, double ga){
    double fval;
    fval=exp(ga*log(x));
    return fval;
}

double GF_exp(double x, double ga){
    double fval;
    fval=exp(ga*x)-1.0;
    return fval;
}

double GF_ln(double x, double ga){
    double fval;
    fval=-1.0*log(1.0-ga*x);
    return fval;
}

void get_yt(int idx, int m, int n, int *mi, double *Y, double *tj, double *y,
        double *dt_y, char cg, double ga){
    
    int j,j1,j0;
    double (*GF_func)(double , double);
    
    switch (cg){
        case 'l':
            GF_func=GF_linear;
            break;
        case 'p':
            //printf("[DEBUG][get_yt] GF_power\n");
            GF_func=GF_power;
            break;
        case 'e':
            GF_func=GF_exp;
            break;
        case 'n':
            GF_func=GF_ln;
            break;
        default:
            printf("[DEBUG][get_yt]  no match function");
    }
    
    for(j=0;j<m;j++){
        dt_y[j]=0.0;
        y[j]=0.0;
        //printf("[DEBUG][get_yt] %d, Y=%lf, tj=%lf\n", j, Y[idx*m+j],tj[j]);
    }
    
    j1=1; // index of y.
    if(isfinite(Y[idx*m])==1){
      //printf("[DEBUG][get_yt] j0=0\n");
      y[0]=Y[idx*m];
      dt_y[0]=GF_func(tj[0],ga);
      //dt_y[0]=exp(log(tj[0])*ga);
      j0=0; // first or previous non nan position
    }else{
      for(j=1;j<m;j++){
        if(isfinite(Y[idx*m+j])==1){
          j0=j;
          y[0]=Y[idx*m+j];
          break;
        }
      }
    }
    //printf("[DEBUG][get_yt] y=%lf, dt=%lf\n",y[0],dt_y[0]);
    for(j=1+j0;j<m;j++){
        if(isfinite(Y[idx*m+j])==1){
            y[j1] = Y[idx*m+j] - Y[idx*m+j0]; // j0: previous non nan position
            dt_y[j1] = GF_func(tj[j],ga) - GF_func(tj[j0],ga);
            //printf("[DEBUG][get_yt] j1=%d, Y=%lf, Y0=%lf, y=%lf, dt=%lf\n",j1, Y[idx*m+j], Y[idx*m+j0],y[j1],dt_y[j1]);
            j1++;
            j0++;
        }
    }
    *mi=j1;
    return;
}

double *vec(FILE *in,int m){
     int id,j, idx;
     char a='\0';
     char a30[30]={'\0'};
     double *u=malloc(sizeof(double)*m);

     idx=0;
     id=0;
     //printf("m=%d\n",m);
     while(idx<m+1 && a!='\n'){
         a=fgetc(in);
         if(a!='\0'){
            if((a=='\t' || a=='\n' )|| a==' '){
               u[idx]=atof(a30);
               // printf("idx=%d, %lf\n",idx, u[idx]);
               idx++;
               id=0;
               for(j=0;j<30;j++) a30[j]='\0';
            }else{
               a30[id]=a;
               id++;
            }
         }
     }
     /*
     printf("idx=%d\n",idx);
     printf("a30=%s\n",a30);
     */
     return u;
}



void skiplines(FILE * in, int n_skip){
   int i_skip=0;
   char a;
   printf("skip %d rows\n",n_skip);
   while(i_skip<n_skip){
     a=getc(in);
     if(a=='\n') i_skip++;
   }
}



/* shape of data : m*(n+1) */
void readdata(double *Y, double *tj, char *fn, int m, int n, int n_skip){

   int i,j;
   //char a='\0';
   //char mod='r';
   //int set_m[n];
   double *u1=malloc(sizeof(double)*(n+1));
   double *yi=malloc(sizeof(double)*m);
   FILE *filein;

   //Y=malloc(sizeof(double)*m*n);
   //tj=malloc(sizeof(double)*m);

   printf("[LOG][readdata] data file (%s)\n",fn);
   printf("[LOG][readdata] m (%d)\n", m);
   printf("[LOG][readdata] n (%d)\n", n);
   filein=fopen(fn,"r");
   if(filein != NULL){
       skiplines(filein,n_skip);
       for(j=0;j<m;j++){
           u1=vec(filein,n+1);
           tj[j]=u1[0];

         for(i=0;i<n;i++){
             Y[i*m+j]=u1[i+1];
             //printf("[readdata] (%d,%d)%lf\n",i,j,Y[i*m+j]);
         }
       }
   }else{
        printf("[ERROR] no data file (%s)\n",fn);
   }
   fclose(filein);
   return;
}


void getcfg(char *filename, int *n_unit, int *n_times){
    FILE *file;
    char *name;
    char *value;
    //char md='r';
    char line[128];
    char search[2]="=\0";
    char ref1[3][15]={"filename","n_unit","n_times"};
    //char ref_target[15];
    char *fn_cfg="data.cfg";

    //strcpy(fn_cfg, "file.cfg");
    file=fopen(fn_cfg,"r");
    
    if ( file != NULL){
      while ( fgets ( line, sizeof(line), file ) != NULL ){ /* read a line */
         //printf("[getcfg][LINE]%s\n",line);

         name = strtok(line, search);
         value = strtok(NULL,search);         
         if(strcmp(ref1[0],name)==0){
           value=strtok(value,"\n");
           strcpy(filename,value);
         }         
         if(strcmp(ref1[1],name)==0){
           n_unit[0]=atoi(value);
         }
         if(strcmp(ref1[2],name)==0){
           n_times[0]=atoi(value);
         }
      }
    }else{
        printf("[ERROR] no %s", fn_cfg);
    }
    fclose (file);
       printf("[getcfg]n_times=%d\n",n_times[0]);
       printf("[getcfg]n_unit=%d\n",n_unit[0]);
       printf("[getcfg]filename=%s\n",filename);
    return;
}

double *para_trans(double *pars, int n, char *method){
    double *pars1=malloc(sizeof(double) * n);
    int j;
    char cm=method[0];
    char cg=method[1];
    
    printf("[DEBUG][para_trans]method=%s\n",method);
    printf("[DEBUG][para_trans]method=%c\n",cm);
    printf("[DEBUG][para_trans]method=%c\n",cg);
/*
    for(j=0;j<n;j++)
        printf("[DEBUG][para_trans]par[%d]=%lf\n",j,pars[j]);      
*/
    switch(cm){
        case '1':
            // Inverse Gaussian process with random effects (mu, lambda)
            // xi, sigma_mu, alpha, beta, sigma_e
            pars1[0]=fabs(pars[0]);
            pars1[1]=pow(10,pars[1]);
            pars1[2]=1.0/fabs(pars[2]);
            pars1[3]=fabs(pars[2]*pars[3]);
            pars1[4]=pow(10,pars[4]);

            break;
        case '2':
            // Inverse Gaussian process with random effect (lambda)
            // mu, alpha, beta, sigma_e
            pars1[0]=fabs(pars[0]);
            pars1[1]=1.0/fabs(pars[1]);
            pars1[2]=fabs(pars[1]*pars[2]);
            pars1[3]=pow(10,pars[3]);

            break;
        case '3':
            // Inverse Gaussian process with random effect (mu)
            // xi, sigma_mu, lambda, sigma_e
            pars1[0]=fabs(pars[0]);
            pars1[1]=pow(10,pars[1]);
            pars1[2]=fabs(pars[2]);
            pars1[3]=pow(10,pars[3]);
            break;
        case '4':
            // Inverse Gaussian process with fixed effects
            // mu, lambda, sigma_e
            pars1[0]=fabs(pars[0]);
            pars1[1]=fabs(pars[1]);
            pars1[2]=pow(10,pars[2]);
            break;            

        case 'g':
            // gamma process
            // for(j=0;j<n;j++)
            //     printf("par[%d]=%lf\n",j,pars[j]);
            pars1[0]=fabs(1.0/pars[0]);
            pars1[1]=fabs(pars[0]*pars[1]);
            pars1[2]=pow(10,pars[2]);
            break;
        case 'r':
            // gamma process with random effect
            // for(j=0;j<n;j++)
            //     printf("par[%d]=%lf\n",j,pars[j]);
            pars1[0]=fabs(1.0/pars[0]);
            pars1[1]=fabs(1.0/pars[1]);
            pars1[2]=fabs(pars[1]*pars[2]);
            pars1[3]=pow(10,pars[3]);

    }
    if(cg!='l'){
        pars1[n-1]=pars[n-1];
    }
    
    printf("[DEBUG] parameters:\n");
    for(j=0;j<n;j++)
        printf("[DEBUG] %lf\n",pars1[j]);  
    return(pars1);
}




double qtl=0.1;
int main(int *argc, char *argv[]){
    int i,j;
    int n_p=5;
    int n_unit;
    int n_times;
    int nn1,nn2;
    int mi;

    double *pars;
    double *pars1;
    double ga=1.0;
    double fval=0.0;

    char *method;
    char method_cm='1';
    char method_cg='l'; 
    //char method_me='q'; 
    char fn_name[50]={'\0'};
    
    getcfg(fn_name, &n_unit, &n_times);
    double *ans_mat=malloc(sizeof(double)*n_unit*3);
    double *Y1=malloc(sizeof(double)*n_unit*n_times);
    double *y_mat=malloc(sizeof(double)*n_unit*n_times);
    double *t_mat=malloc(sizeof(double)*n_unit*n_times);
    double *y=malloc(sizeof(double)*n_times);
    double *dt_y=malloc(sizeof(double)*n_times);
    double *tj1=malloc(sizeof(double)*n_times);
    int *mi_vec=malloc(sizeof(int)*n_unit);
    //readdata(Y1, tj1, "2024_T351_group1.txt", n_times, n_unit, 1);
    printf("[LOG] start to data reading\n");
    readdata(Y1, tj1, fn_name, n_times, n_unit, 1);
    printf("[LOG] end to data reading\n");

    method=argv[1];
    method_cm=argv[1][0];
    method_cg=argv[1][1];
    printf("[LOG][main] cg=%c\n",method_cg);
    printf("[LOG][main]method_cm=%c\n",method_cm);
    printf("[LOG][main]method_me=%c\n",method[2]);

    switch(method_cm){
        case '1':
            n_p=5;
            break;
        case '2':
            n_p=4;
            break;
        case '3':
            n_p=4;
            break;
        case '4':
            n_p=3;
            break;            
        case 'g':
            n_p=3;
            break;            
        case 'r':
            n_p=4;
            break;            
    }
    // non-linear
    if(method_cg!='l'){
        n_p++;
    }
    printf("[LOG][main] n_p=%d\n",n_p);

    pars=malloc(sizeof(double)*n_p);
    pars1=malloc(sizeof(double)*n_p);
    for(i=0;i<n_p;i++){
        pars[i]=atof(argv[i+2]);
        printf("[LOG][main] parameter %d:%lf\n", i+1, pars[i] );
    }
    pars1=para_trans(pars, n_p, method);
    ga=fabs(pars1[n_p-1]);
    qtl=atof(argv[n_p+2]);
    nn1=atoi(argv[n_p+3]);
    nn2=atoi(argv[n_p+4]);
    printf("[DEBUG][main] ga=%lf\n",ga);
    for(i=0;i<n_unit;i++){
        get_yt(i,n_times, n_unit, &mi, Y1, tj1, y, dt_y, method_cg, ga);
        for(j=0;j<mi;j++){
            y_mat[i*n_times+j]=y[j];
            t_mat[i*n_times+j]=dt_y[j];
        }
        mi_vec[i]=mi;
        //printf("[LOG][main] mi=%d\n",mi);
    }
    /*
    for(i=0;i<n_unit;i++){
        for(j=0;j<mi_vec[i];j++){
           printf("[DEBUG][main] (%d,%d) %6.2lf, %10.4lf \n",i,j,y_mat[i*n_times+j], t_mat[i*n_times+j]);
        }
    }
    */
    loglik_q(pars1, &n_p, &n_unit, &n_times, &qtl, y_mat, t_mat, method, &fval,ans_mat,&nn1,&nn2,mi_vec);
    return 0;
}

