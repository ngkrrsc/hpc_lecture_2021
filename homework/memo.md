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
> mpicxx 00_hello.cpp -O3 -fopenmp  
> mpirun -np 4 ./a.out
