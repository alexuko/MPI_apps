#include <iostream>
#include <mpi.h>
#include <math.h>
//variables to determine size of the world, rank and root.
int world_size, world_rank, root = 1;
//result per node
double result;
//array that will hold results from each node in the world
double *totalResults;

//Declaring the methods we are going to need
/**
 * Takes in and int = n, as a parameter and will return the total of 
 * prime numbers encounter startin from 0 until n
*/
int primeCounter(int n);

/**
 * Returns the result of how long each node takes to run the primeCounter() method
 * It's going to initialize a timer when the operation starts and when it ends,
 * perform the calculation for the current node
 * once the operation has finished the calculate the diference between the starting time and the 
 * end time and we return it as our final result 
*/
double performOperation();

/**
 * this method is the core method of the MPI program and 
 * It will get the result of an operation perform in the method performOperation()
 * then the result will be Gathered by the root which is going to hold the results from all
 * of the ranks (nodes) in the world.  
 * And finally print them out to the console by the root (only).
 * 
*/
void run();

/**
 * main() will be executed by each node 
*/
int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    // execute the main operation for each node 
    run();
    
    // we are done with the MPI library so we must finalise it
    MPI_Finalize();
    return 0;
}

void run()
{
    if (world_rank == root)
    {
        // print the name and student number and processor model only once
        std::cout << "============================================ " << std::endl;
        std::cout << "Name: \e[1mRoberto Alejandro Rivera Mejia\e[0m" << std::endl;
        std::cout << "No: 3019536" << std::endl;
        std::cout << "uP: Intel(R) Core(TM) i5-3230M CPU @ 2.60GHz" << std::endl;
        std::cout << "============================================ " << std::endl;
        std::cout << "Rank " << world_rank << " printing ";
    }

    
    //variable to hold the result of the operation
    result = 0;

    //array with a size = world_size where we will store results from each node
    totalResults = new double[world_size];
    
    //execute the performOperation() and save the result
    result = performOperation();

    //gather the results from all the nodes including itself
    MPI_Gather(&result, 1, MPI_DOUBLE, totalResults, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);

    //print All of the results 
    if (world_rank == root)
    {
        //printing results contained in array totalResults
        std::cout << "results:\n" << std::endl;
        for (unsigned int i = 0; i < world_size; i++)
        {
            double rank_time = totalResults[i];
            printf("\tRuntime = %f\n", rank_time);
        }
        std::cout << std::endl;
    }
     //deallocate memory
    delete totalResults;


}

double performOperation()
{
    //variables to store initial time and ending time.
    double start, end;
    //save starting time
    start = MPI_Wtime();
    //perform the method primeCounter()
    primeCounter(pow(25, world_rank + 2));
    //save the final time
    end = MPI_Wtime();
    //return the elapsed time it took the rank to complete its operation
    return end - start;
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
