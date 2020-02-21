#include<iostream>
#include<mpi.h>


int main(int argc, char **argv){

    MPI_Init(&argc, &argv);
    int world_size, rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int req_num = 30;
    int arrInt[req_num];
    int randomNum;
    
    for (int i = 0; i < req_num; i++)
    {
        randomNum = rand() % 10;
        arrInt[i] = randomNum;
        i++;
    }
    
    if(rank == 0){
        int arrSend[req_num];
        
        MPI_Bcast(&arrSend,req_num,MPI_INT,0,MPI_COMM_WORLD);
        std::cout << "rank " << rank << " broadcasting array message" << std::endl;

    }else if(rank == 2){
        int arrRecv[req_num];
        
        MPI_Bcast(&arrRecv,req_num,MPI_INT,0,MPI_COMM_WORLD);
        std::cout << "rank " << rank << " printing array received" << std::endl;
        for (int i = 0; i < req_num; i++)
        {
            std::cout << i << ": " << arrRecv[i] << std::endl;
        }
        
        
 
    }else{
        std::cout << "rank " << rank << " array received" << std::endl;
    }
    
            

    

    MPI_Finalize();



}
