#include<iostream>
#include<cstdlib>
#include<mpi.h>

int compute_average = 1;
int send_average = 2;
int shutdown = 3;
int size, rank;

//  master()
void master(){
    //total average of all the averages from the nodes
    float total_avg = 0;
    float average = 0;

    // ask all three nodes to compute an average
    for (int i = 0; i < size; i++)
    {
        MPI_Send(&compute_average, 1, MPI_FLOAT, i, 0,MPI_COMM_WORLD);
        std::cout << "master (0) told all the slaves to compute" << std::endl;
    }
    
    // ask all three nodes to send their average
    for (int i = 0; i < size; i++)
    {
        MPI_Send(&send_average, 1, MPI_FLOAT, i, 0,MPI_COMM_WORLD);
        MPI_Recv(&average,1,MPI_FLOAT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        total_avg += average;
    }
   
    
    // tell all the nodes to shutdown
    for (size_t i = 0; i < size; i++)
    {
        MPI_Send(&shutdown, 1, MPI_INT, i, 0,MPI_COMM_WORLD);
    }

    std::cout << "Master (0): shutting down all slaves" << std::endl; 
     // display the total of averages
     std::cout << "master (0) average result from all the slaves " << total_avg / (size - 1) << std::endl;
    

}
// slave()
void slave(){
    int message_type = 0;
    float average = 0;
    //messages received
    MPI_Recv(&message_type,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
        while (message_type != shutdown)
        {
            if(message_type == compute_average){
                srand(rank);
                int sum =0;
                int req_num = 10;
                int rand_num = 0;
                std::cout << "Slave (" << rank << "): calculating average" << std::endl;

                for (int i = 0; i < req_num; i++)
                {
                    // + 1 so, we avoid number zero
                    rand_num= (rand() % 10 + 1);
                    //std::cout << "Slave (" << rank << "): random num: "<< rand_num << std::endl;
                    sum += rand_num;
                }
                //calculate the average 
                average = sum / req_num;
                //print results from slave
                std::cout << "Slave (" << rank << "): sum of '"<< req_num<<"' random ints: "<< sum << std::endl;
                std::cout << "Slave (" << rank << "): average of '"<< req_num << "' ints: " << average << std::endl;
                std::cout << "Slave (" << rank << "): Average Calculated" << std::endl;
                
            }else if (message_type == send_average){
                MPI_Send(&average, 1, MPI_FLOAT, 0,0,MPI_COMM_WORLD);
                std::cout << "Slave (" << rank << "): average sent" << std::endl;
            }
            //ask about this line of code
            MPI_Recv(&message_type, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // print shutdown message
        std::cout << "Slave (" << rank << "): shutting down" << std::endl;

}

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if(rank == 0){
        master();
    }else{
        slave();
    }
    
    
    MPI_Finalize();
}