#include<iostream>
#include<mpi.h>


int main(int argc, char **argv){
    MPI_Init(&argc, &argv);
    int world_size, world_rank, rand_num = 0, total_sum = 0;
    
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    
    srand(world_rank);
    rand_num = rand() % 400;     
    std::cout << "Rank: " << world_rank << ", Random number: "<< rand_num << std::endl;
    MPI_Reduce(&rand_num,&total_sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    
    if(world_rank == 0){
        std::cout << "Rank " << world_rank << " printing" << std::endl;
        std::cout << "Total sum: " << total_sum << std::endl;
    }

    MPI_Finalize();
}