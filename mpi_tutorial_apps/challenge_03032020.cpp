#include <iostream>
#include <mpi.h>

int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    int arr_size = 80;
    int *total_array = new int[arr_size];
    int *partition = new int[arr_size / world_size];
    int highest = 0;
    int *arrHighests = new int[world_size];

    srand(5);

    if(world_rank == 0){
        std::cout << "Alejandro, 3019536" << std::endl;
        std::cout << "rank " << world_rank << " generating array" << std::endl;
        for (unsigned int i = 0; i < arr_size; i++){
            total_array[i] = rand() % 79;
            std::cout << total_array[i] << ", ";
            if(i % 10 == 9) std::cout << " " << std::endl;
        }       
        std::cout << "total array end-------- " << std::endl;
    }

    
    MPI_Scatter(total_array,arr_size/world_size,MPI_INT,partition,arr_size/world_size,MPI_INT, 0, MPI_COMM_WORLD);
    
    
    std::cout << "Rank " << world_rank << " received:" << std::endl;    
    for (unsigned int i = 0; i < arr_size / world_size; i++){
        std::cout << partition[i] << ", ";
        if(i % 10 == 9) std::cout << std::endl;
        if(partition[i] > highest) highest = partition[i];        
    }
    std::cout << "Rank " << world_rank << " highest number: " << highest << std::endl;
    

    //now gather all of the highest number into the array and node 1 will print out the overall highest
    MPI_Gather(&highest,1,MPI_INT,arrHighests,1,MPI_INT,1,MPI_COMM_WORLD);
    if(world_rank == 1){
        int highestOverall = 0;        
        for (unsigned int i = 0; i < world_size; i++){
            if(arrHighests[i] > highestOverall) highestOverall = arrHighests[i];
        }
        std::cout << "Overall Highests: " << highestOverall << std::endl;
    } 


    //print once on the screen
    // we are done with the MPI library so we must finalise it
    MPI_Finalize();
    delete total_array, partition, arrHighests;

    return 0;
}
//compile
//mpic++ challenge_03032020.cpp -o challenge_03032020
//run with 4 nodes
//mpiexec -n 4 challenge_03032020
//run with 8 nodes
//mpiexec -n 8 --oversubscribe challenge_03032020
  