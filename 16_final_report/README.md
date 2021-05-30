final report of hpc 2021
<br>
<br>
example.cpp:オリジナル
<br>
0_omp.cpp:ループの入れ替え, openmp化, 行列積を関数化
<br>
1_blocking.cpp:キャッシュブロッキング
<br>
2_simd:キャッシュブロッキング+simd ←最速
<br>
<br>
compile 
> qrsh -g tga-hpc-lecture -l s_gpu=8 -l h_rt=0:20:00  
> module load gcc  
> module load intel-mpi  
> mpicxx example.cpp -fopenmp -march=native -O3  
> mpirun -np 4 ./a.out
