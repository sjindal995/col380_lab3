#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include "math.h"
using namespace std;

#define SIZE 8

void merge(int* buf, int size, int first_dec, int second_dec, int final_dec){
    cout << "start: " << endl;
    for(int i=0;i < size;i++){
        cout << buf[i] << " ";
    }
    cout << endl;
    int new_arr[size];
    int first_i = (first_dec*((size/2)-1));
    int inc_first = 0;
    int inc_second = 0;
    if(first_dec == 1) inc_first = -1;
    else inc_first = 1;
    if(second_dec == 1) inc_second = -1;
    else inc_second = 1;
    int second_i = size/2 + (second_dec*((size/2)-1));
    int done_first = 0;
    int done_second = 0;

    int loop_i = (final_dec*(size-1));
    int loop_inc = 0;
    if(final_dec == 1) loop_inc = -1;
    else loop_inc = 1;
    for(; (loop_i < size && loop_i >= 0); loop_i += loop_inc){
        if(done_first < size/2 && done_second < size/2){
            if(buf[first_i] <= buf[second_i]){
                new_arr[loop_i] = buf[first_i];
                first_i += inc_first;
                done_first++;
            }
            else{
                new_arr[loop_i] = buf[second_i];
                second_i += inc_second;
                done_second++;
            }
        }
        else if(done_first == size/2){
            new_arr[loop_i] = buf[second_i];
            second_i += inc_second;
            done_second++;
        }
        else if(done_second == size/2){
            new_arr[loop_i] = buf[first_i];
            first_i += inc_first;
            done_first++;
        }
    }
    cout << "end: " << endl;
    for(int i=0;i<size;i++){
        buf[i] = new_arr[i];
        cout << buf[i] << " ";
    }
    cout << endl;
}


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
    if(world_rank%2 == 0) sort(recv, recv+ (10*SIZE/world_size));
    else sort(recv, recv+ (10*SIZE/world_size), greater<int>());
    for(int i=0;i < log2(world_size);i++){
        int first_dirxn = ((world_rank&(int(pow(2,i)))) > 0);
        int second_dirxn = (((world_rank^(int(pow(2,i))))&(int(pow(2,i)))) > 0);
        int dirxn = ((world_rank&(int(pow(2,i+1)))) > 0);
        for(int j=i; j>=0; j--){
            int comp = (world_rank^(int(pow(2,j))));
            if(comp < world_rank){
                MPI_Request req_send;
                MPI_Request req_recv;
                MPI_Status st;
                MPI_Isend(recv, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_send);
                MPI_Wait(&req_send, &st);
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
                // int dirxn = (world_rank&(int(pow(2,i+1))));
                // if(dirxn == 0) sort(recved, recved + (2*10*(SIZE/world_size)));
                // else sort(recved, recved + (2*10*(SIZE/world_size)), greater<int>());
                merge(recved, (2*10*(SIZE/world_size)), first_dirxn, second_dirxn, dirxn);
                MPI_Isend(recved + 10*SIZE/world_size, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_send);
                MPI_Wait(&req_send, &st);
                for(int k=0;k<10*SIZE/world_size;k++){
                    recv[k] = recved[k];
                }
                // free(recved);
            }
            first_dirxn = dirxn;
            second_dirxn = dirxn;
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }

    // for(int i=log2(world_size)-1;i >=0 ;i--){
    //     int comp = (world_rank^(int(pow(2,i))));
    //     if(comp < world_rank){
    //         MPI_Request req_send;
    //         MPI_Request req_recv;
    //         MPI_Status st;
    //         MPI_Isend(recv, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_send);
    //         MPI_Irecv(recv, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_recv);
    //         MPI_Wait(&req_recv, &st);
    //     }
    //     else{
    //         int *recved = (int*)malloc(2*10*(SIZE/world_size)*sizeof(int));
    //         for(int k=0;k<10*SIZE/world_size;k++){
    //             recved[k] = recv[k];
    //         }
    //         MPI_Request req_send;
    //         MPI_Request req_recv;
    //         MPI_Status st;
    //         MPI_Irecv(recved + 10*SIZE/world_size, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_recv);
    //         MPI_Wait(&req_recv, &st);
    //         int dirxn = (world_rank&(int(pow(2,i+1))));
    //         sort(recved, recved + (2*10*(SIZE/world_size)));
    //         MPI_Isend(recved + 10*SIZE/world_size, 10*SIZE/world_size, MPI_INT, comp, 0, MPI_COMM_WORLD, &req_send);
    //         for(int k=0;k<10*SIZE/world_size;k++){
    //             recv[k] = recved[k];
    //         }
    //     }
    // }
    for(int i=0; i<world_size;i++){
        if(i == world_rank){
            cout << "w"<<i<<" : ";
            for(int i=0;i<10*SIZE/world_size;i++){
                cout << recv[i] << ", ";
            }
            cout << endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    mytime = MPI_Wtime() - mytime;
    if(world_rank == 0){
        // cout << "timing: " << mytime << endl;
    }
    // Finalize the MPI environment.
    MPI_Finalize();
}