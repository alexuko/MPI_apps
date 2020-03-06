#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // arrays that we need for communicating data
    int arr_size = 40;
    int *total_array = new int[arr_size];
    int *partition = new int[arr_size / world_size];

    // all processes should generate a partition of data to be communicated to the node that is gathering and print it out to console
    srand(world_rank);
    std::cout << "rank: " << world_rank << " numbers: ";
    for (unsigned int i = 0; i < arr_size/world_size; i++)
    {
        partition[i] = rand() % 10;
        std::cout << partition[i] << ", ";
    }    
    std::cout << std::endl;
    
    MPI_Gather(partition,arr_size/world_size,MPI_INT, total_array,arr_size/world_size, MPI_INT,0,MPI_COMM_WORLD);
    if(world_rank == 0){
        std::cout <<"total data gathered from rank: "<< world_rank << std::endl;
        for (unsigned int i = 0; i < arr_size; i++)
        {
            std::cout << total_array[i] << ", ";
            if(i % 10 == 9) std::cout << std::endl;
        }
        std::cout << std::endl;
        

    }
       
    MPI_Finalize();
    delete total_array, partition;
    
    return 0;
}
