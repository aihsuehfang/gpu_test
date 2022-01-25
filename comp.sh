# env
export OMP_DISPLAY_ENV=TRUE
export OMP_STACKSIZE_DEV='143614149592031232B'
export OMP_STACKSIZE_DEV_0='143614149592031232B'
export OMP_STACKSIZE_DEV_1='143614149592031232B'
export OMP_STACKSIZE_DEV_2='143614149592031232B'
export OMP_STACKSIZE_DEV_3='143614149592031232B'
export OMP_STACKSIZE_DEV_4='143614149592031232B'
export OMP_STACKSIZE_DEV_5='143614149592031232B'
export OMP_STACKSIZE_DEV_6='143614149592031232B'
export OMP_STACKSIZE_DEV_7='143614149592031232B'
export OMP_DISPLAY_AFFINITY='TRUE'

# compile
nvfortran -c LowDiscrepancy.f  -acc=gpu -gpu=cuda11.5,cc70 -stdpar=gpu -Minfo -Mnomain -pgf77libs  -cuda -cudalib=cublas -g
nvc -c functions.c LowDiscrepancy.o -mp=gpu  -lgomp -lm -Minfo=all -Mcuda -lgf90 -gpu=cuda11.5 -loffload -O3 -acc=gpu -target=gpu -g
nvc model_gpu.c functions.o LowDiscrepancy.o -mp=gpu  -lgomp -lm -Minfo=accel,mp  -Mcuda -pgf77libs -gpu=cuda11.5,cc70 -O3 -acc=gpu -target=gpu

# test
time ./a.out 4pq 0.0413691174705704, 0.0165145724819948, -2.49188143656703, 0.69794927434121 0.95 20 2000
