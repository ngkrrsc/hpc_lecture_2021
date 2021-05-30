final homework of hpc 2021
<br>
test:original
<br>
1_blocking.cpp:add cache blocking
<br>
<br>
compile 
> module load gcc  
> module load intel-mpi  
> mpicxx example.cpp -fopenmp -march=native -O3  
> mpirun -np 4 ./a.out
