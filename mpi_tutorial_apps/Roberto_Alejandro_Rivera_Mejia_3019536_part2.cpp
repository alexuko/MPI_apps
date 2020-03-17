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
    std::cout << "Rank " << world_rank << ", "<< size << " Values in Array:" << std::endl;
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
        //arr_values[i] = (rand() % 50);
        //zero excluded
        arr_values[i] = (rand() % 50) + 1;
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
    MPI_Scatter(arr_values, partition_size, MPI_INT, partition, partition_size, MPI_INT, root, MPI_COMM_WORLD);

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
    MPI_Scatter(partition,partition_size,MPI_INT,partition,partition_size,MPI_INT,root,MPI_COMM_WORLD);
    //MPI_Scatter(arr_values, partition_size, MPI_INT, partition, partition_size, MPI_INT, root, MPI_COMM_WORLD);
    printArray(partition,partition_size);

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
#include <iostream>
#include <mpi.h>
#include <math.h>

//variables that we are going to need for the MPI program
int world_size, world_rank, partition_size, *partition, *arr,arr_size, root, input;
float  mean, mean_sum, overall_mean, diference, diferences_sum;
   
//printArray() will print out the values in an array
void printArray(int *arr, int size)
{
    std::cout << "Values in Array:" << std::endl;
    for (int i = 0; i < size; i++)
    {
        //print out each value of the array
        std::cout << arr[i] << ", ";
        if (i % 10 == 9)
            std::cout << std::endl;
    }
    std::cout << "---------------------------------------- " << std::endl;
}

//sum()
int sum(int *arr, int size)
{
    float sum = 0;
    for (unsigned int i = 0; i < size; i++)
    {
        //sum all of the values from the array
        sum += arr[i];
    }
    return sum;
}

float get_mean(int *arr, int size)
{
    //print out to check results are correct
    //std::cout << "Rank " << world_rank << " Median: " << sum(arr, size) / size << std::endl;
    //return --> median = Divide the sum by how array size
    return sum(arr, size) / size;
}

float get_variance(float diff_sum, int size)
{
    //print out the variance to che it is correct
    std::cout << "Variance: " << diff_sum / (size - 1) << std::endl;
    // return --> variance = Divide the sum of squares by (n-1)
    return diff_sum / (size - 1);
}

float sumDiferences(int *arr, int size, float overall_mean)
{
    //variable to hold the sum
    float sum = 0;
    for (unsigned int i = 0; i < size; i++)
    {
        //Square all of the numbers from each of the subtractions and add to the sum
        sum += (arr[i] - overall_mean) * (arr[i] - overall_mean);
    }
    //std::cout << "Rank " << world_rank << " Diference_sum: " << sum << std::endl;
    //return the sum of the differences
    return sum;
}

float get_standar_deviation(float diff_sum, int size)
{
    //declare variable and get variance
    float variance = get_variance(diff_sum, size);
    //print out the Standard Deviation to chek it is correct
    std::cout << "Standar deviation: " << sqrt(variance) << std::endl;
    //Standard Deviation = the square root of the variance
    return sqrt(variance);
}



int *generate_array(int arr_size)
{
    //inittialize the array with the desired number of values
    int *arr = new int[arr_size];
    //seed the random number generator with the value of 1
    srand(1);    
    //for loop to generate numbers in the array
    for (unsigned int i = 0; i < arr_size; i++)
    {
        //assign a random value for each of the positions in the array
        arr[i] = rand() % 50;
    }
    //return the array
    return arr;
}

void coordinator()
{
    //print the name and student number
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Roberto Alejandro Rivera Mejia" << std::endl;
    std::cout << "Student number: 3019536" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    //generate the array with the desired size
    arr = generate_array(arr_size);
    //coordinator broadcast the partition size to the rest of nodes
    MPI_Bcast(partition, 1, MPI_INT, root, MPI_COMM_WORLD);
}


int main(int argc, char **argv)
{   
    //size of the array through command line
    //if more than 1 then a size has been assigned
    if (world_rank == 0 && argc > 1) {
        arr_size = atoi(argv[1]);
    }else{
        //if not assigned then a predefined size of 100 is assigned to the size
        arr_size = 100;
    }
    //variable that holds the time at the begining
    double start = MPI_Wtime();
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    //assign the root number
    root = 0;
    //inittialialize the main array 
    arr = new int[arr_size];
    //divide evenly the total size of the array between the total number of nodes
    partition_size = arr_size/world_size;
    //inittialialize the partition array for each node 
    partition = new int[partition_size];
    //set mean_sum variable to 0
    mean_sum = 0; 
    //check is the rank is the coordinator
    if (world_rank == root){
        coordinator();
    }

    
    
    //scatter the partition to all the nodes
    MPI_Scatter(arr, partition_size, MPI_INT, partition, partition_size, MPI_INT, root, MPI_COMM_WORLD);
    //print out at which node we are at
    std::cout << "Rank " << world_rank << " partition: ";
    //print out the partition values from each node
    for (unsigned int i = 0; i < partition_size; i++)
        std::cout << partition[i] << ", ";
    std::cout << std::endl;
    //get the mean from each node 
    mean = get_mean(partition, partition_size);
    //reduce operation to gather the overall average
    MPI_Reduce(&mean, &mean_sum, 1, MPI_FLOAT, MPI_SUM, root, MPI_COMM_WORLD);
       
    //Compute the overall average (coordinator only)
    if(world_rank == 0){
        overall_mean = mean_sum/world_size;
        //Broadcast the overall average to all nodes
        MPI_Bcast(&overall_mean, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }
    
    //compute the sum of diferences from each node
    diference = sumDiferences(partition,partition_size,overall_mean);

    //Reduce the overall sum of diferences
    MPI_Reduce(&diference,&diferences_sum,1,MPI_FLOAT,MPI_SUM,root,MPI_COMM_WORLD);

    //Calculate the standard deviation and print out the dataset, mean and standard deviation (coordinator only).
    if(world_rank == root){
        std::cout <<"=======    Coordinator, Rank " << world_rank <<"    =======" << std::endl;
        //print out the dataset
        printArray(arr,arr_size);
        //print out mean
        std::cout <<"Mean: " << overall_mean << std::endl;
        //get standar_deviation and print it out
        get_standar_deviation(diferences_sum,arr_size);
        std::cout <<"=========================================" << std::endl;
        //variable that holds the time at the end
        double end = MPI_Wtime();
        //print out the time it took to the proccess to complete    
        std::cout << "The process took " << end - start << " seconds to run." << std::endl;

    }        

    
    // we are done with the MPI library so we must finalise it
    MPI_Finalize();
    //deallocate memory
    delete arr, partition;
    //end time of the proccess
    return 0;
}

*/


/*

compile
mpic++ Roberto_Alejandro_Rivera_Mejia_3019536_part2.cpp -o Roberto_Alejandro_Rivera_Mejia_3019536_part2
run with 4 nodes
mpiexec -n 4 Roberto_Alejandro_Rivera_Mejia_3019536_part2
run with 8 nodes
mpiexec -n 8 --oversubscribe Roberto_Alejandro_Rivera_Mejia_3019536_part2

*/





