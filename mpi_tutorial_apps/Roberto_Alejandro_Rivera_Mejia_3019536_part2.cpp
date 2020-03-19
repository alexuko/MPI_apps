#include <iostream>
#include <mpi.h>
#include <math.h>

//size of the world, rank and root.
int world_size, world_rank, size, root = 0;
//variables for the time
double start, end;

//Declaring the methods we are going to need 
void printArray(int *arr, int size);
int sum(int *arr, int size);
double get_mean(int *arr, int size);
double get_variance(double diff_sum, int size);
double sumDiferences(int *arr, int size, double overall_mean);
double get_standar_deviation(double diff_sum, int size);
void coordinator(int arr_size);
void participant();

//main() will be executed by each node 
int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    //place a barrier before collecting the start time (to make sure all nodes are ready to proceed)
    MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
    //start time
    start = MPI_Wtime();
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
    //place a barrier before collecting the end time (to make sure all nodes are done with their task)
    MPI_Barrier(MPI_COMM_WORLD); 
    //end time
    end = MPI_Wtime();
    // we are done with the MPI library so we must finalise it
    MPI_Finalize();
    ///use time on root node and print out result 
    if (world_rank == root) { 
    printf("Runtime = %f\n", end-start);
    }
    //end time of the proccess
    return 0;
}

//printArray() method that will print out an array to console in a single line. It should accept two parameters: a pointer to the array and the size of the array.
void printArray(int *arr, int size)
{
    std::cout << "Rank " << world_rank << ", "<< size << " Values in Array: " ;
    for (int i = 0; i < size; i++)
    {
        //print out each value of the array in a single line
        std::cout << arr[i] << ", ";
        // if (i % 10 == 9)std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "---------------------------------------- " << std::endl;
}

//sum() method that takes in a reference to an array and an array size, and returns the sum of all the values in that array.
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

//get_mean() method that takes in a reference to an array and an array size, and returns the mean of a set of values.
double get_mean(int *arr, int size)
{
    return (double)sum(arr, size) / size;
}

//get_variance() method that takes in a reference to an array and an array size, and returns the variance of a set of values.
double get_variance(double diff_sum, int size)
{
    //print out the variance to check it's correct
    std::cout << "Variance: " << diff_sum / (size - 1) << std::endl;
    // return --> variance = Divide the sum of squares by (n-1)
    return diff_sum / (size - 1);
}

//sumDiferences() method that takes in a reference to an array, an array size, and the overall mean of the dataset. 
//It should produce a sum of the square of differences between each value in the dataset and the mean and return this as the result.
double sumDiferences(int *arr, int size, double overall_mean)
{
    double sum = 0;
    for (unsigned int i = 0; i < size; i++)
    {
        //Square all of the numbers from each of the subtractions and add to the sum
        sum += (arr[i] - overall_mean) * (arr[i] - overall_mean);
    }
    //return the sum of the differences
    return sum;
}

//get_standar_deviation() method that takes in the differemces sum and an array size, gets the variance first so the standard deviation can be calculated by getting the sqrt of the variance. 
double get_standar_deviation(double diff_sum, int size)
{
    //declare variable and get variance
    double variance = get_variance(diff_sum, size);
    //print out the Standard Deviation to chek it is correct
    std::cout << "Standar deviation: " << sqrt(variance) << std::endl;
    //return Standard Deviation = the square root of the variance
    return sqrt(variance);
}


//coordinator() method will be executed if the world rank is the root.
//It will create an set of random values, get the right partition size, scatter data accordingly
//get the mean back from every node, sum it up and calculate the overall mean, this broadcast it 
//to all of the nodes, so every node returns the sum of differences from their dataset 
//Finally the coordinator will calculate the standard deviation and print out results including
//the entire dataset, and then deallocates memory. 
void coordinator(int array_size)
{
    //check that the size is equaly divisible by the world_size otherwise 
    //we will give back a default size = world_size * 10
    int arr_size = (array_size % world_size) == 0 ? array_size : world_size * 10;

    //print the name and student number
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Roberto Alejandro Rivera Mejia" << std::endl;
    std::cout << "Student number: 3019536" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "============    Coordinator, Rank " << world_rank << "    ============" << std::endl;

    //inittialize the array with the desired size
    int *arr_values = new int[arr_size];
    //seed the random number generator with the value of 1
    srand(1);
    //for loop to generate numbers in the array
    for (unsigned int i = 0; i < arr_size; i++)
    {
        //assign a random value for each of the positions in the array
        //zero excluded
        arr_values[i] = (rand() % 50) + 1;
        //zero included
        //arr_values[i] = (rand() % 50);
    }
    
    //divide the size of the array between the total number of nodes
    int partition_size = arr_size / world_size;

    //coordinator broadcast the partition size to the rest of nodes
    MPI_Bcast(&partition_size, 1, MPI_INT, root, MPI_COMM_WORLD);

    //inittialialize the partition with its correct size
    int *partition = new int[partition_size];

    //scatter values to the rest of the nodes including itself
    MPI_Scatter(arr_values, partition_size, MPI_INT, partition, partition_size, MPI_INT, root, MPI_COMM_WORLD);

    //Calculate the mean for this node.
    double mean = get_mean(partition, partition_size);
    
    double overall_average = 0;
    // Use a reduce operation to gather the overall average.
    MPI_Reduce(&mean, &overall_average, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);

    //Compute the overall average (coordinator only).
    overall_average /= world_size;

    //Broadcast the overall average to all nodes
    MPI_Bcast(&overall_average, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
    
    //Compute the sum of diferences
    double sum_dif = sumDiferences(partition, partition_size, overall_average);

    double overall_sum_dif = 0;
    // Reduce the overall sum of diferences from all the nodes
    MPI_Reduce(&sum_dif, &overall_sum_dif, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);

    //Calculate the standard deviation and print out the dataset, mean and standard deviation (coordinator only).
    printArray(arr_values, arr_size);
    std::cout << "World size is: " << world_size << std::endl;
    std::cout << "Array size : " << arr_size << std::endl;
    std::cout << "Partition size per node: " << partition_size << std::endl;
    std::cout << "Mean: " << overall_average << std::endl;
    double standar_deviation = get_standar_deviation(overall_sum_dif, arr_size);

    std::cout << "============================================================" << std::endl;

    //deallocate memory
    delete arr_values, partition;
}

//participant() method will be executed if the world rank is NOT the root.
//get a broadcasted partition size from the root, create it own partition, and receive the scattered data from root
//calculate its mean and with a reduce operation send it to the root.
//participant will receive the overall average from the root that the node will use to calculate its
//difference sum and using a reduce operation that will sum it up, send it back to the root
void participant()
{
    //variable to hold the partition size of this node
    int partition_size = 0;

    //Get the broadcasted partition size from root
    MPI_Bcast(&partition_size, 1, MPI_INT, root, MPI_COMM_WORLD);

    //Inittialialize the partition with its correct size
    int *partition = new int[partition_size];

    //Receive the scatter part that the root has sent
    MPI_Scatter(partition,partition_size,MPI_INT,partition,partition_size,MPI_INT,root,MPI_COMM_WORLD);
    
    //Calculate the mean for this node.
    double mean = get_mean(partition, partition_size);
    
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
mpiexec -n 4 Roberto_Alejandro_Rivera_Mejia_3019536_part2 1000
run with 8 nodes
mpiexec -n 8 --oversubscribe Roberto_Alejandro_Rivera_Mejia_3019536_part2 1000

*/





