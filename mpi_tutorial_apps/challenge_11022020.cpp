#include<iostream>
#include<mpi.h>
int main(int argc, char **argv){
    //initialize mpi
    MPI_Init(&argc, &argv);
    int size, rank;
    double value;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0 )
    {
        MPI_Recv(&value,1,MPI_DOUBLE,2,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        std::cout << "Rank: " << rank <<" - data received from rank 2: "<< value << std::endl;
    }else if (rank == 2)
    {
        value = 3.1415; 
        MPI_Send(&value,1,MPI_DOUBLE,0,2,MPI_COMM_WORLD);
        std::cout << "Alejandro Rivera" << std::endl;

    }
     
    //finalize mpi
    MPI_Finalize();
    


}
