final homework of hpc 2021
<br>
test:original
<br>
mpi:add OpenMP
<br>
<br>
compile 
> module load gcc
> module load intel-mpi  
> mpicxx 00_hello.cpp -fopenmp  
> mpirun -np 4 ./a.out
