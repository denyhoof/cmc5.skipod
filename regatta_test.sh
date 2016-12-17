rm *.out
rm *.err
mpicc -std=c99 jac_2d.c
nthreads=( 1 )
kruns=10
SUBMIT=mpisubmit
sizes=( 18 34 66 130 258 514 1026 2050 )
for nthread in ${nthreads[@]}
do
    for size in ${sizes[@]}
    do
        echo $size $nthread
        $SUBMIT -n $nthread -stdout $nthread+$size.out -w 00:15:00 a.out -- $kruns $size
    done
done
llq -u edu-cmc-pod16-010
