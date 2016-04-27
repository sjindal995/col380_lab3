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
            if(world_rank == 0){
                array[j] = tmp;
                // cout << tmp << " ";
            }
        }
        // if(world_rank == 0) cout << endl;
        MPI_Scatter(array, SIZE/world_size, MPI_INT, recv + i*SIZE/world_size, SIZE/world_size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    mytime = MPI_Wtime() - mytime;
    if(world_rank == 0){
        cout << "timing: " << mytime << endl;
    }
    // MPI_Barrier(MPI_COMM_WORLD);
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
    // Finalize the MPI environment.
    MPI_Finalize();
}