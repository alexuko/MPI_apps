#include <iostream>
#include <mpi.h>
#include <math.h>
//variables to determine size of the world, rank and root.
int world_size, world_rank, root = 1;
//results variables
double *result, *totalResults;

//Declaring the methods we are going to need
//method that returns the amount of prime numbers in a given integer n.
int primeCounter(int n);

//method that returns the result of how long each node takes to run the primeCounter()
double performOperation();

//method will perform the performOperation(), gather the result from the other nodes and print the results out. 
void coordinator();

//method will perform the performOperation() and send its result to the coordinator. 
void participant();

//main() will be executed by each node
int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    //determine which node is the coordinator
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
    //print the name and student number and processor model
    std::cout << "Name: Roberto Alejandro Rivera Mejia" << std::endl;
    std::cout << "No: 3019536" << std::endl;
    std::cout << "uP: Intel(R) Core(TM) i5-3230M CPU @ 2.60GHz" << std::endl;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ " << std::endl;
    std::cout << "Rank " << world_rank << " printing ";
    //array of size one where we will store the node result
    result = new double[1];
    //array of size world_size where we will store all the results
    totalResults = new double[world_size];
    //save the result of the operation
    result[0] = performOperation();
    
    //gather the results from all the nodes including itself
    MPI_Gather(result, 1, MPI_DOUBLE, totalResults, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
    
    //printing results contained in array totalResults 
    std::cout << "results: " << std::endl;
    for (unsigned int i = 0; i < world_size; i++)
    {
        double rank_time = totalResults[i];
        printf("Runtime = %f\n", rank_time);
    }
    std::cout << std::endl;
    //deallocate memory
    delete totalResults, result;
}

void participant()
{
    //array of size one where we will store the node result
    result = new double[1];
    //save the result of the operation    
    result[0] = performOperation();
    // take part in the gather, sending its result to the root
    MPI_Gather(result, 1, MPI_DOUBLE, NULL, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
    //deallocate memory
    delete result;
}

double performOperation()
{   
    //variables to store initial time, ending time and the time it took to perform operation
    double start, end, elapsed_time;
    //save starting time
    start = MPI_Wtime();
    //perform the method primeCounter()
    primeCounter(pow(25, world_rank + 2));
    //save the final time 
    end = MPI_Wtime();
    //calculate the elapsed time 
    elapsed_time = end - start;
    //return the elapsed time it took to the rank to complete its operation 
    return elapsed_time;
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
    return count;
}

/*
compile
mpic++ Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part1.cpp -o Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part1
run with 4 nodes
mpiexec -n 4 Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part1
*/
