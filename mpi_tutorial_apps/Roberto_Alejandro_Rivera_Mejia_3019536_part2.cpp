#include <iostream>
#include <mpi.h>
#include <math.h>

//variables that we are going to need for the MPI program
int world_size, world_rank, size, root = 0;

//Print out the values in an array
void printArray(int *arr, int size);
int sum(int *arr, int size);
double get_mean(int *arr, int size);
double get_variance(double diff_sum, int size);
double sumDiferences(int *arr, int size, double overall_mean);
double get_standar_deviation(double diff_sum, int size);
void coordinator(int arr_size);
void participant();

int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    //default size
    size  = world_size;
    //size of the array through command line
    //if more than 1 then a size has been assigned
    if (world_rank == 0 && argc > 1)
    {
        size = atoi(argv[1]);
    }

    if (world_rank == root)
    {
        coordinator(size);
    }
    else
    {
        participant();
    }

    // we are done with the MPI library so we must finalise it
    MPI_Finalize();

    //end time of the proccess
    return 0;
}

//printArray() will print out the values in an array
void printArray(int *arr, int size)
{
    std::cout << size << " Values in Array:" << std::endl;
    for (int i = 0; i < size; i++)
    {
        //print out each value of the array
        std::cout << arr[i] << ", ";
        if (i % 10 == 9)
            std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "---------------------------------------- " << std::endl;
}

//sum()
int sum(int *arr, int size)
{
    int sum = 0;
    for (unsigned int i = 0; i < size; i++)
    {
        //sum all of the values from the array
        sum += arr[i];
    }
    return sum;
}

double get_mean(int *arr, int size)
{
    return (double)sum(arr, size) / size;
}

double get_variance(double diff_sum, int size)
{
    //print out the variance to che it is correct
    std::cout << "Variance: " << diff_sum / (size - 1) << std::endl;
    // return --> variance = Divide the sum of squares by (n-1)
    return diff_sum / (size - 1);
}

double sumDiferences(int *arr, int size, double overall_mean)
{
    //variable to hold the sum
    double sum = 0;
    for (unsigned int i = 0; i < size; i++)
    {
        //Square all of the numbers from each of the subtractions and add to the sum
        sum += (arr[i] - overall_mean) * (arr[i] - overall_mean);
    }
    //std::cout << "Rank " << world_rank << " Diference_sum: " << sum << std::endl;
    //return the sum of the differences
    return sum;
}

double get_standar_deviation(double diff_sum, int size)
{
    //declare variable and get variance
    double variance = get_variance(diff_sum, size);
    //print out the Standard Deviation to chek it is correct
    std::cout << "Standar deviation: " << sqrt(variance) << std::endl;
    //Standard Deviation = the square root of the variance
    return sqrt(variance);
}

void coordinator(int array_size)
{
    //check that the size is equaly divisible by the world_size otherwise size = world_size *10
    int arr_size = (array_size % world_size) == 0 ? array_size : world_size * 10;

    //print the name and student number
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Roberto Alejandro Rivera Mejia" << std::endl;
    std::cout << "Student number: 3019536" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Rank " << world_rank << std::endl;

    //inittialize the array with the desired size
    int *arr_values = new int[arr_size];
    //seed the random number generator with the value of 1
    srand(1);
    //for loop to generate numbers in the array
    for (unsigned int i = 0; i < arr_size; i++)
    {
        //assign a random value for each of the positions in the array
        //zero included
        arr_values[i] = (rand() % 50);
        //zero excluded
        //arr_values[i] = (rand() % 50) + 1;
    }
    //printArray(arr_values,arr_size);

    //divide evenly the total size of the array between the total number of nodes
    int partition_size = arr_size / world_size;

    //coordinator broadcast the partition size to the rest of nodes
    MPI_Bcast(&partition_size, 1, MPI_INT, root, MPI_COMM_WORLD);
    std::cout << "partition_size " << partition_size << std::endl;

    //inittialialize the partition with its correct size
    int *partition = new int[partition_size];

    //scatter values to the rest of the nodes including itself
    for (unsigned int i = 0; i < world_size; i++)
    {
        MPI_Send(arr_values + partition_size * i, partition_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        //MPI_Send(arr_values + (i * partition_size), partition_size, MPI_INT, i, 0, MPI_COMM_WORLD);
    }

    //root receive itself the part of the array
    MPI_Recv(partition, partition_size, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printArray(partition, partition_size);
    //printArray(partition,partition_size);

    //Calculate the mean for this node.
    double mean = get_mean(partition, partition_size);
    std::cout << "Rank: " << world_rank << " Mean " << mean << std::endl;

    double overall_average = 0;
    // Use a reduce operation to gather the overall average.
    MPI_Reduce(&mean, &overall_average, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);

    //Compute the overall average (coordinator only).
    overall_average /= world_size;

    //Broadcast the overall average to all nodes and then compute the sum of diferences
    MPI_Bcast(&overall_average, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);

    double sum_dif = sumDiferences(partition, partition_size, overall_average);

    double overall_sum_dif = 0;
    // Reduce the overall sum of diferences
    MPI_Reduce(&sum_dif, &overall_sum_dif, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);

    std::cout << "=======    Coordinator, Rank " << world_rank << "    =======" << std::endl;
    //Calculate the standard deviation and print out the dataset, mean and standard deviation (coordinator only).
    printArray(arr_values, arr_size);
    std::cout << "Mean: " << overall_average << std::endl;
    double standar_deviation = get_standar_deviation(overall_sum_dif, arr_size);

    std::cout << "============================================================" << std::endl;

    //deallocate memory
    delete arr_values, partition;
}

void participant()
{
    //variable to hold the partition size of this node
    int partition_size = 0;
    //Get the broadcasted partition size from root
    MPI_Bcast(&partition_size, 1, MPI_INT, root, MPI_COMM_WORLD);

    //Inittialialize the partition with its correct size
    int *partition = new int[partition_size];

    //Receive the scatter part that the root has sent
    MPI_Recv(partition, partition_size, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //Calculate the mean for this node.
    double mean = get_mean(partition, partition_size);
    std::cout << "Rank: " << world_rank << " Mean " << mean << std::endl;

    double overall_average = 0;
    // Use a reduce operation to gather the overall average.
    MPI_Reduce(&mean, &overall_average, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);

    //Broadcast received from the root with the overall average
    MPI_Bcast(&overall_average, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);

    //compute the sum of diferences
    double sum_dif = sumDiferences(partition, partition_size, overall_average);

    double overall_sum_dif = 0;
    // Reduce the overall sum of diferences
    MPI_Reduce(&sum_dif, &overall_sum_dif, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);

    //deallocarte memory
    delete partition;
}

/*

compile
mpic++ Roberto_Alejandro_Rivera_Mejia_3019536_part2.cpp -o Roberto_Alejandro_Rivera_Mejia_3019536_part2
run with 4 nodes
mpiexec -n 4 Roberto_Alejandro_Rivera_Mejia_3019536_part2
run with 8 nodes
mpiexec -n 8 --oversubscribe Roberto_Alejandro_Rivera_Mejia_3019536_part2

*/