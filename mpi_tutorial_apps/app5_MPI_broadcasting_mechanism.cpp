#include<iostream>
#include<mpi.h>
#include<string>

int main(int argc, char** argv){
    MPI_Init(&argc,&argv);
    int worldSize, worldRank;
    

    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    // print out the rank and size
    std::cout << "rank " << worldRank << " size " << worldSize << std::endl;

    if(worldRank == 0){
        // broadcast a message to all the other nodes
        unsigned int message = 5;
        MPI_Bcast(&message,1,MPI_INT,0,MPI_COMM_WORLD);
        std::cout << "rank 0 broadcasting " << message << std::endl;
    }else{
        // recieve a message from the root
        unsigned int message = 0;
        MPI_Bcast(&message, 1, MPI_INT, 0,MPI_COMM_WORLD);
        std::cout << "rank " << worldRank << " received broadcast of " << message << std::endl;       

    }
    
    MPI_Finalize();


}
/*
mpic++ app5_MPI_broadcasting_mechanism.cpp -o app5_MPI_broadcasting_mechanism
mpiexec -n 4 app5_MPI_broadcasting_mechanism

*/