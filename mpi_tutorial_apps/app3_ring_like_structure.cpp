#include<iostream>
#include<cstdlib>
#include<mpi.h>

int compute_avg = 1;
int world_size, world_rank;

void coordinator(void){
    int message;
    // tell the next node that we need to start computing
    MPI_Send(&compute_avg,1,MPI_INT,1,0,MPI_COMM_WORLD);
    // we will expect the last node to send the us the data, so we can compute
    // our average 
    // world size = 4 
    // world size = (4 - 1) = 3
    MPI_Recv(&message,1,MPI_INT, world_size - 1, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    // compute the average and print out the result
    srand(world_rank);
    int sum = 0;
    float average = 0;

    for (int i = 0; i < 100000; i++){
        sum += rand() % 10;
        average = sum / 100000.f;

    }
               

    std::cout << " coordinator sum is: " << sum << std::endl;
    std::cout << " coordinator average is: " << average << std::endl;

    // ask node 1 to compute its average by sending a single floating point value
    //this will add it in its average  and pass to node 2 etc.
    // when we get the result back we will have the full average
    //send average to node 1
    MPI_Send(&average,1, MPI_FLOAT, 1, 0 ,MPI_COMM_WORLD);
    //receive average from node 3
    MPI_Recv(&average,1, MPI_FLOAT, world_size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  
    //std::cout << "World size is: " << world_size << std::endl;
    std::cout << " coordinator "<< world_rank <<" total average is: " << average / world_size << std::endl;
}


void computeAverage(void){
    int message;
    float current_average;
    // wait till we ger the compute message after this tell the nexr node to start computing
    // an average before computing our average
    MPI_Recv(&message,1,MPI_INT,(world_rank + world_size - 1) % world_size,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&compute_avg,1,MPI_INT,(world_rank + 1) % world_size,0, MPI_COMM_WORLD);


    srand(world_rank);
    int sum = 0;
    float average = 0;

    for (int i = 0; i < 100000; i++){
        sum += rand() % 10;
        average = sum / 100000.f;

    }
    std::cout << " node "<< world_rank << " sum is: " << sum << std::endl;
    std::cout << " node "<< world_rank << " average is: " << average << std::endl;

    // ask node 1 to compute its average by sending a single floating point value
    //this will add it in its average  and pass to node 2 etc.
    // when we get the result back we will have the full average
    MPI_Recv(&current_average,1, MPI_FLOAT, (world_rank + world_size - 1) % world_size, 0 ,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    current_average += average;
    std::cout << "node" << world_rank << " current average is: " << current_average << std::endl;
 
    MPI_Send(&current_average,1, MPI_FLOAT, (world_rank + 1) % world_size , 0, MPI_COMM_WORLD);


}

int main(int argc,char** argv){
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    if(world_rank == 0){
        coordinator();
    }else{
        computeAverage();
    }

    MPI_Finalize();

}