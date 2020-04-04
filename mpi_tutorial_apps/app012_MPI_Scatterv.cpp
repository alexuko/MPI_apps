#include <iostream>
#include <mpi.h>
#include <math.h>
//variables to determine size of the world, rank and root.
int world_size, world_rank, root = 0;
void printArrayToConsole(int *to_print, int print_size);
void coordinator();
void participant();

//main() will be executed by each node
int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if (world_rank == root)
    {
        coordinator();
    }
    else
    {
        participant();
    }

    // we are done with the MPI library so we must finalise it
    MPI_Finalize();
    return 0;
}

void coordinator()
{
    // generate a group of 40 numbers randomly
    int *num_array = new int[40];
    for (unsigned int i = 0; i < 40; i++)
        num_array[i] = rand() % 10;
    printArrayToConsole(num_array, 40);
    // make a partition length array with even divisions then randomly offset it by 1
    int *length_array = new int[world_size];
    for (unsigned int i = 0; i < 4; i++)
    {
        length_array[i] = (40 / world_size);
        if (i % 2)
            length_array[i]--;
        else
            length_array[i]++;
    }
    printArrayToConsole(length_array, 4);
    // make an offsets array by using the displacements array
    int *offsets_array = new int[world_size];
    offsets_array[0] = 0;
    for (unsigned int i = 1; i < 4; i++)
        offsets_array[i] = offsets_array[i - 1] + length_array[i - 1];
    printArrayToConsole(offsets_array, 4);
    // send each node their individual pariition size and allocate memory for our own partition 
    int partition_size;
    MPI_Scatter(length_array, 1, MPI_INT, &partition_size, 1, MPI_INT, 0,
                MPI_COMM_WORLD);
    std::cout << "rank 0 psize: " << partition_size << std::endl;
    int *partition = new int[partition_size];
    // use the scatterv to send the data to each node
    MPI_Scatterv(num_array, length_array, offsets_array, MPI_INT, partition,
                 partition_size, MPI_INT, 0, MPI_COMM_WORLD);
    // print out the numbers that we have obtained in the scatter
    printArrayToConsole(partition, partition_size);
    // delete our partition as we are finished at it
    delete partition;
    delete num_array;
    delete length_array;
    delete offsets_array;
}

// task for the participant
void participant()
{
    // take part in the scatter to get our partition size
    int partition_size;
    MPI_Scatter(NULL, 0, MPI_INT, &partition_size, 1, MPI_INT, root, MPI_COMM_WORLD);
    std::cout << "rank " << world_rank << " psize: " << partition_size << std::endl;
    // allocate memory for our partition
    int *partition = new int[partition_size];
    // take part in the scatterv to get our data
    MPI_Scatterv(NULL, NULL, NULL, MPI_INT, partition, partition_size, MPI_INT, root, MPI_COMM_WORLD);
    // print out the numbers that we have obtained in the scatter
    printArrayToConsole(partition, partition_size);
    // delete our partition as we are finished with it
    delete partition;
}

// function that takes in an integer array and prints it out to console
void printArrayToConsole(int *to_print, int print_size)
{
    for (int i = 0; i < print_size; i++)
    {
        std::cout << to_print[i] << ", ";
        if (i % 10 == 9)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}

/*
compile
mpic++ app012_MPI_Scatterv.cpp -o app012_MPI_Scatterv
run with 4 nodes
mpiexec -n 4 app012_MPI_Scatterv

*/
