#include <iostream>
#include <mpi.h>
#include <math.h>
//variables to determine size of the world, rank and root.
int world_size, world_rank, size, partitionSize, offset, totalPrimesCount, root = 0;
//results variables
int *newArr, *partition, *originalArray;
//Declaring the methods we are going to need
int newPrimeCounter(int *intArr, int arrSize);
void printArray(int *primesArray, int size);
int *generateNewArr(int size);

//main() will be executed by each node
int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    size = 4000;
    originalArray = generateNewArr(size);

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
        //print the name and student number and processor model
        std::cout << "============================================ " << std::endl;
        std::cout << "Name: \e[1mRoberto Alejandro Rivera Mejia\e[0m" << std::endl;
        std::cout << "No: 3019536" << std::endl;
        std::cout << "uP: Intel(R) Core(TM) i5-3230M CPU @ 2.60GHz" << std::endl;
        std::cout << "============================================ " << std::endl;
    }
    //new size that we will need to initialize the array
    //std::cout << "OriginalSize " << arrSize << std::endl;
    //we ceil the new size as it might contain decimal points
    //int newSize = (arrSize / 2) + 1;
    int newSize = (int) ceil(arrSize / 2) + 1;
    //std::cout << "NewSize " << newSize << std::endl;
    //initialize the array with the correct size we are going to need

    newArr = new int[newSize];
    //get rid of all of the non odd numbers except by number 2
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

    //as the new array may not be evenly divisible by the world size we will increase its size accordingly
    //until its evenly divisible, hence partitions will be equal
    offset = 0;
    while (newSize % world_size != 0)
    {
        offset++;
        newSize += offset;
    }

    //std::cout << "offset " << offset << std::endl;

    //we now get the partition size for each node
    partitionSize = (newSize / world_size);

    //std::cout << "Rank " << world_rank << " partitionSize " << partitionSize << std::endl;
    partition = new int[partitionSize];

    //send the partition size to the rest of the nodes
    MPI_Bcast(&partitionSize, 1, MPI_INT, root, MPI_COMM_WORLD);
    //this offset needs to reach the last node worldsize -1
    MPI_Bcast(&offset, 1, MPI_INT, root, MPI_COMM_WORLD);
    //scatter data to all of the nodes
    MPI_Scatter(newArr, partitionSize, MPI_INT, partition, partitionSize, MPI_INT, root, MPI_COMM_WORLD);
    
    //we need to get rid of the offset to avoid errors in data (just for the last node)
    if(world_rank == world_size -1){
        // e.g partitionSize = 252, offset = 5, worldSIze = 8
        // (offset * ((world_size / 2) - 1) --> = 5 * (8/2) - 1 --> 5 * 3 Then --> 252-15 = 237         
        for (unsigned int i = partitionSize; i > partitionSize - (offset * ((world_size / 2) - 1)); i--)
        {
            partition[i -1] = 0;
        }
        
    }
    //printArray(partition,partitionSize);
    // get the total number of prime numbers per node
    //int primesInNode = primeCounter(partition, partitionSize);
    
    //initialise counter to 0
    unsigned int number, primesInNode = 0;
    bool isPrime;
    std::cout << "Rank " << world_rank ;
    //std::cout << " Primes: ";
    //the first of the prime numbers is 2, so we set our a variable = 2
    for (unsigned int i = 0; i < partitionSize; i++){
        number = partition[i];
        //we will work with anything above 1
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
                //std::cout << number << ", ";
                primesInNode++;
            }        

        }   
        
    }
    //std::cout << std::endl;
    std::cout << " -> total: " << primesInNode << std::endl;
    //sum all of the primes from each node 
    MPI_Reduce(&primesInNode,&totalPrimesCount,1,MPI_INT,MPI_SUM,root,MPI_COMM_WORLD);
    
    //we wait for every node to reach this point 
    MPI_Barrier(MPI_COMM_WORLD);
    if(world_rank == root) 
        std::cout << "Rank: " << world_rank << " Primes total count: \e[1m" << totalPrimesCount << "\e[0m" <<std::endl;

    
}

int *generateNewArr(int size)
{
    int *arr = new int[size];
    for (unsigned int i = 0; i < size; i++)
    {
        int number = i + 1;
        arr[i] = number;
    }
    return arr;
}


void printArray(int *printArray, int size)
{
    if (printArray && size > 0)
    {

        std::cout << "Rank \e[1m" << world_rank;
        std::cout << "\e[0m, array size \e[1m" << size << "\e[0m values: ";
        for (unsigned int i = 0; i < size; i++)
        {
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
mpiexec -n 1 Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
mpiexec -n 8 --oversubscribe Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
*/
