#include <mpi.h>
#include <cstdio>
#include <cmath>
#include <vector>
#include <chrono>
using namespace std;

#include <cstdlib>
#include <immintrin.h>
/*typedef vector<vector<float>> matrix;*/

/*
void matmult(matrix &A, matrix &B, matrix &C, int N) {
  const int m = N, n = N, k = N;
  const int kc = 512;
  const int nc = 64;
  const int mc = 256;
  const int nr = 64;
  const int mr = 32;
#pragma omp parallel for collapse(2)
  for (int jc=0; jc<n; jc+=nc) {
    for (int pc=0; pc<k; pc+=kc) {
      float Bc[kc*nc];
      for (int p=0; p<kc; p++) {
        for (int j=0; j<nc; j++) {
          Bc[p*nc+j] = B[p+pc][j+jc];
        }
      }
      for (int ic=0; ic<m; ic+=mc) {
	float Ac[mc*kc],Cc[mc*nc];
        for (int i=0; i<mc; i++) {
          for (int p=0; p<kc; p++) {
            Ac[i*kc+p] = A[i+ic][p+pc];
          }
          for (int j=0; j<nc; j++) {
            Cc[i*nc+j] = 0;
          }
        }
        for (int jr=0; jr<nc; jr+=nr) {
          for (int ir=0; ir<mc; ir+=mr) {
            for (int kr=0; kr<kc; kr++) {
              for (int i=ir; i<ir+mr; i++) {
                for (int j=jr; j<jr+nr; j++) { 
                  Cc[i*nc+j] += Ac[i*kc+kr] * Bc[kr*nc+j];
                }
              }
            }
          }
        }
        for (int i=0; i<mc; i++) {
          for (int j=0; j<nc; j++) {
            C[i+ic][j+jc] += Cc[i*nc+j];
          }
        }
      }
    }
  }
}
*/

int main(int argc, char** argv) {
  /*size:プロセス数, rank:プロセス番号*/
  int size, rank;
  /*MPI communicator の初期化*/
  MPI_Init(&argc, &argv);
  /*プロセス数取得*/
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  /*プロセス番号取得*/
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  const int N = 4096;
  /*vector:可変長配列*/
  vector<float> A(N*N);
  vector<float> B(N*N);
  vector<float> C(N*N, 0);
  vector<float> subA(N*N/size);
  vector<float> subB(N*N/size);
  vector<float> subC(N*N/size, 0);
  /*n*n行列作成, 並列化できそう*/
  for (int i=0; i<N; i++) {
    for (int j=0; j<N; j++) {
      /*drand48:区間 [0.0, 1.0) で 一様分布する非負の倍精度浮動小数点実数値*/
      /*A_ij*/
      A[N*i+j] = drand48();
      /*B_ij*/
      B[N*i+j] = drand48();
    }
  }
  /*intは切り捨て, offset:各プロセスの担当領域の先頭*/
  int offset = N/size*rank;
  /*N/size:1プロセスの担当する行, 列の数*/
  for (int i=0; i<N/size; i++)
    for (int j=0; j<N; j++)
      subA[N*i+j] = A[N*(i+offset)+j];
  for (int i=0; i<N; i++)
    for (int j=0; j<N/size; j++)
      subB[N/size*i+j] = B[N*i+j+offset];
  /*次のrankから受け取る*/
  int recv_from = (rank + 1) % size;
  /*前のrankに送る*/
  int send_to = (rank - 1 + size) % size;

  /*パラメータ設定*/
  const int m = N/size, n = N, k = N/size;
  const int kc = 512;
  const int nc = 64;
  const int mc = 256;
  const int nr = 64;
  const int mr = 32;

  double comp_time = 0, comm_time = 0;
  for(int irank=0; irank<size; irank++) {
    /*計測開始*/
    auto tic = chrono::steady_clock::now();
    offset = N/size*((rank+irank) % size);
    /*subA*subBをsubCに格納*/
    /*openmpで並列化したい、キャッシュブロッキング*/
    /*subB*subAを計算*/
#pragma omp parallel for collapse(2)
  for (int jc=0; jc<n; jc+=nc) {
    for (int pc=0; pc<k; pc+=kc) {
      float Bc[kc*nc];
      for (int p=0; p<kc; p++) {
        for (int j=0; j<nc; j++) {
          Bc[p*nc+j] = subB[(p+pc)*N/size+j+jc];
        }
      }
      for (int ic=0; ic<m; ic+=mc) {
	float Ac[mc*kc],Cc[mc*nc];
        for (int i=0; i<mc; i++) {
          for (int p=0; p<kc; p++) {
            Ac[i*kc+p] = subA[(i+ic)*N+p+pc];
          }
          for (int j=0; j<nc; j++) {
            Cc[i*nc+j] = 0;
          }
        }
        for (int jr=0; jr<nc; jr+=nr) {
          for (int ir=0; ir<mc; ir+=mr) {
            for (int kr=0; kr<kc; kr++) {
              for (int i=ir; i<ir+mr; i++) {
                for (int j=jr; j<jr+nr; j++) { 
                  Cc[i*nc+j] += Ac[i*kc+kr] * Bc[kr*nc+j];
                }
              }
            }
          }
        }
        for (int i=0; i<mc; i++) {
          for (int j=0; j<nc; j++) {
            subC[(i+ic)*N+j+jc+offset] += Cc[i*nc+j];
          }
        }
      }
    }
  }	  
	/*  
    for (int i=0; i<N/size; i++)
      for (int j=0; j<N/size; j++)
        for (int k=0; k<N; k++)
          subC[N*i+j+offset] += subA[N*i+k] * subB[N/size*k+j];
	  */
	  
    /*計算時間加算*/
    auto toc = chrono::steady_clock::now();
    comp_time += chrono::duration<double>(toc - tic).count();
    /*mpiにring型の通信はない*/
    /*subBを通信*/
    MPI_Request request[2];
    MPI_Isend(&subB[0], N*N/size, MPI_FLOAT, send_to, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(&subB[0], N*N/size, MPI_FLOAT, recv_from, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Waitall(2, request, MPI_STATUS_IGNORE);
    /*計算時間加算*/
    tic = chrono::steady_clock::now();
    comm_time += chrono::duration<double>(tic - toc).count();
  }
  MPI_Allgather(&subC[0], N*N/size, MPI_FLOAT, &C[0], N*N/size, MPI_FLOAT, MPI_COMM_WORLD);
  for (int i=0; i<N; i++)
    for (int j=0; j<N; j++)
      for (int k=0; k<N; k++)
        C[N*i+j] -= A[N*i+k] * B[N*k+j];
  double err = 0;
  for (int i=0; i<N; i++)
    for (int j=0; j<N; j++)
      err += fabs(C[N*i+j]);
  if(rank==0) {
    double time = comp_time+comm_time;
    printf("N    : %d\n",N);
    printf("comp : %lf s\n", comp_time);
    printf("comm : %lf s\n", comm_time);
    printf("total: %lf s (%lf GFlops)\n",time,2.*N*N*N/time/1e9);
    printf("error: %lf\n",err/N/N);
  }
  MPI_Finalize();
}
