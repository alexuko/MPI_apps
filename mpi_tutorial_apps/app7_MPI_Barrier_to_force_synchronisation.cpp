#include<iostream>
#include<mpi.h>
#include<cstdlib>
#include <unistd.h>

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);


    std::cout << "rank: " << world_rank << " world size: " << world_size << std::endl;
    srand(world_rank);
    int sleep_time = rand() % 10;
    std::cout << "Sleeping time: " << sleep_time <<  std::endl;
    sleep(sleep_time);    
    std::cout << "node " << world_rank << " exiting sleep and synchronising" << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "node " << world_rank << " synchronised" << std::endl;
    

    

    MPI_Finalize();
    return 0;
}
        