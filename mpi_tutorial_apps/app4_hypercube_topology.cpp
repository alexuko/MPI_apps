#include<iostream>
#include<cstdlib>
#include<mpi.h>

int world_size, world_rank;

int hypercubePower(int size){
    unsigned int power = 1, j = 0;
    for (; power < size; power = power << 1, j++)
        std::cout << j << std::endl;
    return j;  
}


// initialises an array with the given value and size
void initArray(int *to_init, int value, unsigned int size){
    for(unsigned int i = 0; i < size; i++)
        to_init[i] = value;
}

// function to calulate our most significant power
int mostSignificantPower(int rank){
    int power = 1;
    // to calculate our most significant power keep multiplying by two until we find a number
    // that is bigger than our rank then go one step back
    while (power < rank)
        power <<= 1;
    //
    power >>= 1;


    return power;
}

// function that determines the communication directions based on the given rank
void computeDirections(int *comms_offset, int rank, int hypercube_power){
    // go through each power if we have a 0 bit we must add, a 1 bit we must subtract
    for (int i = 1, j = 0 ; j < hypercube_power; i <<= 1, j++)
    {
        if( rank & i )
            comms_offset[j] = -i;
        else
            comms_offset[j] = i;
        
    }   

}

// function to calculate the sum of 100 random integers and return it
int randomSum(void){
    int sum = 0;
    for (unsigned int i = 0; i < 100; i++)
    {
        sum += rand() % 10;
    }
    
    return sum;
}

void coordinator(void){
    // we need to establish what power is used in the hypercube this tells us how many communication directions we have
    int power = hypercubePower(world_size);
    std::cout << power << std::endl;

    // an array that will house the various offsets that we need to connect with the appropriate nodes in each direction
    int comms_offsets[16];
    initArray(comms_offsets, 0, 16);

    // we need to determine our communication directions based on the bits in our rank
    computeDirections(comms_offsets, world_rank, power);
    for (unsigned int i = 0; i < power; i++)
        std::cout << "rank " << world_rank << " offset " << i << ": " << comms_offsets[i] << std::endl;
    // to start the computation we need to distribute a message for starting the
    // computation in this case we will send a
    // single integer that will represent the dimension that we need to communicate on.
    // this will state which channel we need to send on
    int message = 0; 
    for (unsigned int  i = 0; i < power; i++)
    {
        MPI_Send(&i, 1, MPI_INT, world_rank + comms_offsets[i], 0, MPI_COMM_WORLD);
        std::cout << "rank " << world_rank << "send message to rank " << world_rank + comms_offsets[i] << std::endl;
    }

    // calculate the average of 100 random numbers
    srand(world_rank);
    int sum = randomSum();
    float average = sum / 100.f;
    std::cout << "rank " << world_rank << " average is: " << average << std::endl;

    // send a message to all the other nodes to tell them to sum up their averages
    for (unsigned int i = 0; i < power; i++)
    {
        MPI_Send(&i, 1, MPI_INT, world_rank + comms_offsets[i], 0, MPI_COMM_WORLD);
        std::cout << "rank " << world_rank << "send total message to rank " << world_rank + comms_offsets[i] << std::endl;
    }
    
    // we need to get the averages from each direction
    float total_average = average;
    for (int message = power - 1; message >= 0; message--)
    {
        MPI_Recv(&average, 1, MPI_FLOAT, world_rank + comms_offsets[message], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "rank " << world_rank << " recieved total from " << world_rank + comms_offsets[message] << std::endl;
        total_average += average;
    }

    // print out the overall average
        std::cout << "rank " << world_rank << " total and average: " << total_average << " " << total_average / world_size << std::endl;
}

void participant(void){
    // we need to establish what power is used in the hypercube this tells us how many communication directions we have
    int power = hypercubePower(world_size);
    std::cout << power << std::endl;
    // an array that will house the various offsets that we need to connect with the appropriate nodes in each direction
    int comms_offsets[16];
    initArray(comms_offsets, 0, 16);
    // we need to determine our communication directions based on the bits in our rank

    computeDirections(comms_offsets, world_rank, power);
    for (unsigned int i = 0; i < power; i++)
        std::cout << "rank " << world_rank << " offset " << i << ": " << comms_offsets[i] << std::endl;

    // we need to recieve a message to start computation from another node and then propogate it on further
    int message = 0;
    MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for (message++; message < power; message++)
    {
        MPI_Send(&message, 1, MPI_INT, world_rank + comms_offsets[message], 0,
                 MPI_COMM_WORLD);
        std::cout << "rank " << world_rank << " send message to rank " << world_rank + comms_offsets[message] << std::endl;
    }
    // calculate the average of 100 random numbers
    srand(world_rank);
    int sum = randomSum();
    float average = sum / 100.f;
    std::cout << "rank " << world_rank << " average is: " << average << std::endl;
    // we need to get a message that will tell us to total our average get this message and retain it
    // send a message to all the other nodes to tell them to sum up their averages
    MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for (int i = message + 1; i < power; i++)
    {
        MPI_Send(&i, 1, MPI_INT, world_rank + comms_offsets[i], 0, MPI_COMM_WORLD);
        std::cout << "rank " << world_rank << " sent total message to rank " << world_rank + comms_offsets[i] << std::endl;
    }
    // we need to get the averages from each direction
    float total_average = average;
    for (int i = power - 1; i > message; i--)
    {
        MPI_Recv(&average, 1, MPI_FLOAT, world_rank + comms_offsets[i], 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "rank " << world_rank << " recieved total from " << world_rank + comms_offsets[i] << std::endl;
        total_average += average;
    }
    // print out the overall average
    std::cout << "rank " << world_rank << " total and average: " << total_average << " " << total_average / world_size << std::endl;
    // send our average onto the the next node
    std::cout << "rank " << world_rank << " " << comms_offsets[message] << std::endl;
    MPI_Send(&total_average, 1, MPI_FLOAT, world_rank + comms_offsets[message], 0, MPI_COMM_WORLD);
    std::cout << "rank " << world_rank << " sent total average to rank " << world_rank + comms_offsets[message] << std::endl;
}


int main(int argc, char** argv){
    //initialize mpi library
    MPI_Init(&argc, &argv);
        
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    if(world_rank == 0){
        coordinator();
    }else{
        participant();
    }
    
    //finalise everything so MPI can shutdown properly
    MPI_Finalize();

    
    // standard C/C++ thing to do at the end of main
    return 0;
}