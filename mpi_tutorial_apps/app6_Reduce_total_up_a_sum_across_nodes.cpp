#include<iostream>
#include<mpi.h>

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);


    //std::cout << "rank: " << world_rank << " world size: " << world_size << std::endl;
    int our_sum = 10;
    int total_sum = 0;
    MPI_Reduce(&our_sum, &total_sum, 1, MPI_INT,MPI_SUM, 0, MPI_COMM_WORLD);
    std::cout << "rank: " << world_rank << " our_sum: " << our_sum <<  "  total_sum:" << total_sum << std::endl;


    MPI_Finalize();
}
