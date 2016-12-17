rm *.out
rm *.err
mpixlc_r -qsmp=omp jac_2d.c
nthreads=( 2 4 8 16 32 64 128)
kruns=10
SUBMIT=mpisubmit.bg
sizes=( 18 34 66 130 258 514 1026 2050 4098 8198)
for nthread in ${nthreads[@]}
do
    for size in ${sizes[@]}
    do
        echo $size $nthread
        $SUBMIT -n $nthread  -e "OMP_NUM_THREADS=4" -stdout $nthread+$size.out -w 00:15:00 a.out -- $kruns $size
    done
done
llq -u edu-cmc-pod16-010
