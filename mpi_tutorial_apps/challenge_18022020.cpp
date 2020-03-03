#include<iostream>
#include<mpi.h>


void printArray(int *theArray, int size){
    for (int i = 0; i < size; i++){      
        int current =  theArray[i];  
        std::cout << "Value at position: " << i + 1 << ": " << current << std::endl;
    }
}


void coordinator(const int size){
    std::cout << "Coordinator " << " creating array " << std::endl;
    int *intsArray= new int[size];//init array
        
        for (unsigned int i = 0; i < size; i++)
        {
            intsArray[i] = rand() % 10;           

        }

        std::cout << "Broadcasting array " << std::endl;
        MPI_Bcast(intsArray, size, MPI_INT, 0, MPI_COMM_WORLD);

        delete intsArray;

}

void worker(int rank, const int size){
    std::cout << "Rank "<< rank << " received array" << std::endl;
    int *intsArray= new int[size];//init array
    MPI_Bcast(intsArray, size, MPI_INT, 0, MPI_COMM_WORLD);
    
        if(rank == 2){
            printArray(intsArray,size);
        }
    delete intsArray;
}

int main(int argc, char **argv){
    
    MPI_Init(&argc, &argv);
    int world_size, world_rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


    int arr_size = 30;
    srand(world_rank);


    if(world_rank == 0){
        coordinator(arr_size);

    }else{
        // set array and initialize it
        worker(world_rank, arr_size);
        
    }
  
    //deallocate memory
    MPI_Finalize();
    return 0;
}
