#include<iostream>
#include<mpi.h>


void printArray(const int theArray[], int size){
    for (int i = 0; i < size; i++)
    {        
        std::cout << "Value at position: " << i+1 << " -> " << theArray[i] << std::endl;
    }
    

}

int main(int argc, char **argv){
    
    MPI_Init(&argc, &argv);
    int world_size, world_rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


    int arr_size = 30;
    //int intsArray[arr_size];
        int intsArray[arr_size]={0};
        //int *intsArray = new int[arr_size];
        for (int i = 0; i < arr_size; i++){  
            // srand(world_rank);          
            unsigned int value = rand() % 10;
            intsArray[i] = value;            
            i++;
        }

    if(world_rank == 0){
        std::cout << "creating array of integers" << std::endl;
        MPI_Bcast(&intsArray,arr_size,MPI_INT,0,MPI_COMM_WORLD);
        std::cout << "rank 0 broadcasting array" << std::endl;


    }else{
        // set array and initialize it
        int intsArray[arr_size] = {0};

        MPI_Bcast(&intsArray,arr_size,MPI_INT,0,MPI_COMM_WORLD);
        std::cout << "rank: " << world_rank << " received array" << std::endl;

        if(world_rank == 2){
            std::cout << "rank: " << world_rank << " printing array" << std::endl;
            printArray(intsArray,arr_size);
            
        }
    }
  
    //deallocate memory
    MPI_Finalize();

}
