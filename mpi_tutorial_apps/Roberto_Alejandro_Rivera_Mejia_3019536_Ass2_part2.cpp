#include <iostream>
#include <mpi.h>
#include <math.h>
//variables to determine size of the world, rank and root.
int world_size, world_rank, partitionSize, root = 0;
//results variables
int *intsArray, *partition;
//Declaring the methods we are going to need
//int primeCounter(int *intArray, int size);
int primeCounter(int n);
int newPrimeCounter(int *intArr, int arrSize);
void printArray(int *primesArray, int size);
void master(int size);
void slave();

//main() will be executed by each node
int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int size = 4000;
    
    //start function
    if (world_rank == root)
    {
        master(size);
    }
    else
    {
        slave();
    }
    // we are done with the MPI library so we must finalise it
    MPI_Finalize();
    return 0;
}

int newPrimeCounter(int *intArr, int arrSize) {}
void master(int arrSize)
{
    //print the name and student number and processor model
    std::cout << "Name: Roberto Alejandro Rivera Mejia" << std::endl;
    std::cout << "No: 3019536" << std::endl;
    std::cout << "uP: Intel(R) Core(TM) i5-3230M CPU @ 2.60GHz" << std::endl;
    std::cout << "============== Rank " << root << " (root) =============== " << std::endl;

    //new size that we will need to initialize the array
    int newSize = (arrSize / 2) + 1;
    //initialize the array with the correct size we are going to need
    intsArray = new int[newSize];

    //test
    //for (unsigned int i = 0; i < newSize; i++) intsArray[i] = 0;

    //the following loop will ger rid of almost half of the non prime number before start the distribution of work
    //set an increment to 1, we'll need it for moving spaces backwards to place numbers in order.
    //as arrays start from Zero and we are starting from number 1, we increment 1 to our number to compare
    //if the number is equals to 2 which is the only pair prime or number is NOT equaly divisible then its more likely to be a prime number
    int increment = 1;
    for (unsigned int i = 0; i < arrSize; i++)
    {
        int number = i + 1;
        if (number == 2 || number % 2 != 0)
        {
            if (number > 3)
            {
                intsArray[i - increment] = number;
                increment++;
            }
            else
            {
                intsArray[i] = number;
            }
        }
    }
    //as the new array is not evenly divisible by the world size we will increase 1 to have enough space in eace partition
    partitionSize = ceil(newSize / world_size + 1);
    std::cout << "Rank " << world_rank << " partitionSize " << partitionSize << std::endl;
    partition = new int[partitionSize];
    //printArray(intsArray, newSize);
    //send the partition size to the rest of the nodes
    MPI_Bcast(&partitionSize, 1, MPI_INT, root, MPI_COMM_WORLD);
    //scatter data to all of the nodes
    MPI_Scatter(intsArray, partitionSize, MPI_INT, partition, partitionSize, MPI_INT, root, MPI_COMM_WORLD);
    printArray(partition, partitionSize);
}

void slave()
{
    //variable where we'll save the partition size
    partitionSize = 0;
    //take part in the broadcast and receive the partition size that the root sent
    MPI_Bcast(&partitionSize, 1, MPI_INT, root, MPI_COMM_WORLD);
    //std::cout << "Rank " << world_rank <<" partition received " << partitionSize << std::endl;
    //initializing array with the correct partition size
    partition = new int[partitionSize];
    //take part in the scatter operation and receive the partition of data from the main array contained in root
    MPI_Scatter(NULL, partitionSize, MPI_INT, partition, partitionSize, MPI_INT, root, MPI_COMM_WORLD);
    printArray(partition, partitionSize);
    // int count;

    // std::cout << "'\e[1m" << count << "\e[0m' Primes found in node " << world_rank << std::endl;
    // return count;
}

int primeCounter(int n)
{
    //initialise counter to 0
    int count = 0;
    //the first of the prime numbers is 2, so we set our a variable = 2
    for (int a = 2; a < n; a++)
    {
        //set the variable to true
        bool isPrime = true;
        //and then procced with the check to find out if its a prime number
        for (int c = 2; c * c <= a; c++)
        {
            //if operation is equally divisible, then its not a prime number
            if (a % c == 0)
            {
                isPrime = false;
                break;
            }
        }
        if (isPrime)
        {
            //if it is prime increase the counter by 1

            count++;
        }
    }
    //return the total count of prime numbers found
    std::cout << "Rank " << world_rank << " prime numbers found: " << count << std::endl;
    return count;
}

void printArray(int *printArray, int size)
{
    if (printArray && size > 0)
    {
        std::cout << "Rank " << world_rank << " Primes: ";
        for (unsigned int i = 0; i < size; i++)
        {
            std::cout << printArray[i] << ", ";
            if (i % 10 == 9)
                std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

/*
compile
mpic++ Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2.cpp -o Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
run with 4 nodes
mpiexec -n 1 Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
mpiexec -n 8 --oversubscribe Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
*/
