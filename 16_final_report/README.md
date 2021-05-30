final report of hpc 2021
<br>
<br>
example.cpp:オリジナル
<br>
0_omp.cpp:ループの入れ替え, openmp化, 行列積を関数化
<br>
1_blocking.cpp:キャッシュブロッキング
<br>
2_simd:キャッシュブロッキング+simd
<br>
compile 
> module load gcc  
> module load intel-mpi  
> mpicxx 00_hello.cpp -O3 -fopenmp  
> mpirun -np 4 ./a.out
