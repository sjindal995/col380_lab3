#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include "math.h"
using namespace std;

#define SIZE 100000


int main(int argc, char** argv){
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int *recv = (int*)malloc(10*(SIZE/world_size)*sizeof(int));
    double mytime = MPI_Wtime();
    for(int i=0;i <10; i++){
        int array[SIZE] = {0};
        for(int j=0; j<SIZE; j++){
            int tmp = 1 + rand()%100;
            // cout << "i,j : " << i << "," << j << endl;
            if(world_rank == 0){
                array[j] = tmp;
                // cout << tmp << endl;
            }
        }
        MPI_Scatter(array, SIZE/world_size, MPI_INT, recv + i*SIZE/world_size, SIZE/world_size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    // for(int i=0; i<world_size;i++){
    //     if(i == world_rank){
    //         cout << "w"<<i<<" : ";
    //         for(int i=0;i<10*SIZE/world_size;i++){
    //             cout << recv[i] << ", ";
    //         }
    //         cout << endl;
    //     }
    //     MPI_Barrier(MPI_COMM_WORLD);
    // }
    // MPI_Barrier(MPI_COMM_WORLD);
    sort(recv, recv+ (10*SIZE/world_size));
    for(int i=0;i < log2(world_size);i++){
        for(int j=i; j>=0; j--){
            int comp = (world_rank^(int(pow(2,j))));
            if(comp < world_rank){
                MPI_Request req_send;
                MPI_Request req_recv;
                MPI_Status st;
                MPI_Isend(recv, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_send);
                MPI_Irecv(recv, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_recv);
                MPI_Wait(&req_recv, &st);
            }
            else{
                int *recved = (int*)malloc(2*10*(SIZE/world_size)*sizeof(int));
                for(int k=0;k<10*SIZE/world_size;k++){
                    recved[k] = recv[k];
                }
                MPI_Request req_send;
                MPI_Request req_recv;
                MPI_Status st;
                MPI_Irecv(recved + 10*SIZE/world_size, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_recv);
                MPI_Wait(&req_recv, &st);
                int dirxn = (world_rank&(int(pow(2,i+1))));
                if(dirxn == 0) sort(recved, recved + (2*10*(SIZE/world_size)));
                else sort(recved, recved + (2*10*(SIZE/world_size)), greater<int>());
                MPI_Isend(recved + 10*SIZE/world_size, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_send);
                for(int k=0;k<10*SIZE/world_size;k++){
                    recv[k] = recved[k];
                }
                // free(recved);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    for(int i=log2(world_size)-1;i >=0 ;i--){
        int comp = (world_rank^(int(pow(2,i))));
        if(comp < world_rank){
            MPI_Request req_send;
            MPI_Request req_recv;
            MPI_Status st;
            MPI_Isend(recv, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_send);
            MPI_Irecv(recv, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_recv);
            MPI_Wait(&req_recv, &st);
        }
        else{
            int *recved = (int*)malloc(2*10*(SIZE/world_size)*sizeof(int));
            for(int k=0;k<10*SIZE/world_size;k++){
                recved[k] = recv[k];
            }
            MPI_Request req_send;
            MPI_Request req_recv;
            MPI_Status st;
            MPI_Irecv(recved + 10*SIZE/world_size, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_recv);
            MPI_Wait(&req_recv, &st);
            int dirxn = (world_rank&(int(pow(2,i+1))));
            sort(recved, recved + (2*10*(SIZE/world_size)));
            MPI_Isend(recved + 10*SIZE/world_size, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_send);
            for(int k=0;k<10*SIZE/world_size;k++){
                recv[k] = recved[k];
            }
        }
    }
    // for(int i=0; i<world_size;i++){
    //     if(i == world_rank){
    //         cout << "w"<<i<<" : ";
    //         for(int i=0;i<10*SIZE/world_size;i++){
    //             cout << recv[i] << ", ";
    //         }
    //         cout << endl;
    //     }
    //     MPI_Barrier(MPI_COMM_WORLD);
    // }
    mytime = MPI_Wtime() - mytime;
    if(world_rank == 0){
        cout << "timing: " << mytime << endl;
    }
    // Finalize the MPI environment.
    MPI_Finalize();
}