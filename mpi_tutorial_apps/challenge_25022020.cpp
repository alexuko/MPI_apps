#include<iostream>
#include<mpi.h>

void coordinator(int rank){
    std::cout << "coordinator rank 0 starting " << std::endl;
    int final_sum = 0;
    srand(rank);
    int rand_num = rand() % 400;     
    std::cout << "Coordinator "<< rank << " random number " << rand_num << std::endl;
    
    MPI_Reduce(&rand_num,&final_sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    std::cout << "Sum " << final_sum << std::endl;

}

void participant(int rank){
    std::cout << "Participant: " << rank << std::endl;
    int final_sum;
    srand(rank);
    int rand_num = rand() % 400;     
    
    MPI_Reduce(&rand_num,&final_sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    std::cout << "participant "<< rank << " random number: " << rand_num << std::endl;
    

}


int main(int argc, char **argv){
    MPI_Init(&argc, &argv);
    int world_size, world_rank, rand_num = 0, total_sum = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if(world_rank == 0){
        coordinator(world_rank);
    }else{
        participant(world_rank);
    }

    MPI_Finalize();
}