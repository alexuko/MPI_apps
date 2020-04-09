#include <iostream>
#include <mpi.h>
#include <math.h>
//variables to determine total number of processes, node, array  sizes, primes counter, and root.
int world_size, world_rank, size, partitionSize, offset, totalPrimesCount, root = 0;
//arrays that we are gonna need
int *newArr, *partition, *originalArray;

//Declare methods we are going to need
/**
 * newPrimeCounter() will take an array of ints and its size as input and return the total amount of prime numbers contained in the array. 
 * It gets rid of the all non odd numbers except number 2, divides the work between nodes, gather the multiple results with a MPI_reduce function
 * which sums up the count of primes from each node and return a single count which also prints out on the console.    
*/
int newPrimeCounter(int *intArr, int arrSize);

/**
 * printArray() is only created for testig purposes as a way to check what's been saved within our array  
*/
void printArray(int *primesArray, int size);

/**
 * generateNewArr() returns an array with the size passed as a parameter starting from 1
*/
int *generateNewArr(int size);

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
    //set the size
    size = 4000;
    //generate an array with the desired size
    originalArray = generateNewArr(size);
    //run the method newPrimeCounter thah will get all of the work done and pass on the parameters required
    newPrimeCounter(originalArray, size);
    // we are done with the MPI library so we must finalise it
    MPI_Finalize();
    //deallocate memory
    delete originalArray, partition, newArr;
    return 0;
}

int newPrimeCounter(int *intArr, int arrSize)
{   
    if (world_rank == root)
    {
        // print the name and student number and processor model only once
        std::cout << "============================================ " << std::endl;
        std::cout << "Name: \e[1mRoberto Alejandro Rivera Mejia\e[0m" << std::endl;
        std::cout << "No: 3019536" << std::endl;
        std::cout << "uP: Intel(R) Core(TM) i5-3230M CPU @ 2.60GHz" << std::endl;
        std::cout << "============================================ " << std::endl;
    }
    
    // new size that we will need to initialize the array. we ceil the new size as
    // it might not be a round number and could contain decimal points. 
    // we also add 1 extra space for number 2 as its the only prime even number 
    int newSize = (int) ceil(arrSize / 2) + 1;
    
    // initialize the newArray with the correct size we are going to need
    newArr = new int[newSize];
    
    // get rid of all of the non odd numbers except by number 2 as its the only even number that its prime
    // the increment variable will help us to move the spaces back and set the value in the correct order 
    // within the array
    int increment = 1;
    for (unsigned int i = 0; i < arrSize; i++)
    {
        int current = intArr[i];
        if (current <= 3)
        {
            newArr[i] = current;
        }
        else if (current % 2 != 0)
        {
            newArr[i - increment] = current;
            increment++;
        }
    }

    // as the resultant new array may not be evenly divisible by the total of processes we will increase its size accordingly
    // until its evenly divisible, hence partitions will be equal
    offset = 0;
    while (newSize % world_size != 0)
    {
        offset++;
        newSize += offset;
    }

    // we now set the partition size for each node
    partitionSize = (newSize / world_size);

    //initialize the partition array with the desired size
    partition = new int[partitionSize];

    //scatter data to all of the nodes
    MPI_Scatter(newArr, partitionSize, MPI_INT, partition, partitionSize, MPI_INT, root, MPI_COMM_WORLD);
    
    //we need to get rid of the offset to avoid errors in data (ONLY for the last node)
    //starting from the end of the array
    if(world_rank == world_size -1){
        // e.g partitionSize = 252, offset = 5, worldSIze = 8
        // (offset * ((world_size / 2) - 1) --> = 5 * (8/2) - 1 --> 5 * 3 Then --> 252 - 15 = 237         
        for (unsigned int i = partitionSize; i > partitionSize - (offset * ((world_size / 2) - 1)); i--)
        {
            partition[i - 1] = 0;
        }        
    }    
    
    //initialise Primes counter to 0 and 
    //number that will hold the value from each space in the array
    int number, primesInNode = 0;
    //variable will help us to know if the number is prime or not
    bool isPrime;
    std::cout << "Rank " << world_rank ;
    for (unsigned int i = 0; i < partitionSize; i++){
        number = partition[i];
        //we will work with anything above 1, as 0 and 1 are not primes
        if(number > 1){
            isPrime =true;
            for (unsigned int j = 2; j <= number / 2; j++)
            {
                if(number % j == 0){
                    isPrime = false;
                    break;
                }                
            }
            if(isPrime){
                //number is prime, so we add 1 to our count 
                primesInNode++;
            }       

        }   
        
    }

    std::cout << " -> total: " << primesInNode << std::endl;

    //sum all of the primes from each node and pass it to the root which is gonna hold the sum value in the totalPrimesCount variable
    MPI_Reduce(&primesInNode,&totalPrimesCount,1,MPI_INT,MPI_SUM,root,MPI_COMM_WORLD);
    
    //we wait for every node to reach this point 
    MPI_Barrier(MPI_COMM_WORLD);
    //the root will print the total of prime numbers found from all the nodes within their partition
    if(world_rank == root) 
        std::cout << "Rank: " << world_rank << " Primes total count: \e[1m" << totalPrimesCount << "\e[0m" <<std::endl;
        
    return totalPrimesCount;
}
 
int *generateNewArr(int size)
{   
    //initialize array with tha parameter passed (size)
    int *arr = new int[size];
    for (unsigned int i = 0; i < size; i++)
    {   
        //add 1 as arrays start from zero, so we have only pisitive numbers
        int number = i + 1;
        arr[i] = number;
    }
    //return the array
    return arr;
}


void printArray(int *printArray, int size)
{   //must be an array and has to be bigger that zero
    if (printArray && size > 0)
    {

        std::cout << "Rank \e[1m" << world_rank;
        std::cout << "\e[0m, array size \e[1m" << size << "\e[0m values: ";
        for (unsigned int i = 0; i < size; i++)
        {   
            //print the values in a single line of 20 values 
            std::cout << printArray[i] << ", ";
            if (i % 20 == 19)
                std::cout << std::endl;
        }
        std::cout << "\n----------------------------------------------" << std::endl;
    }
}

/*
compile
mpic++ Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2.cpp -o Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
run with 4 nodes
mpiexec -n 4 Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
mpiexec -n 8 --oversubscribe Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
*/
