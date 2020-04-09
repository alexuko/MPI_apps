#include <iostream>
#include <mpi.h>
#include <math.h>
//variables to determine size of the world, rank and root.
int world_size, world_rank, size, partitionSize, offset, totalPrimesCount, root = 0;
//results variables
int *newArr, *partition, *originalArray;
//Declaring the methods we are going to need
//int primeCounter(int *intArray, int size);
int primeCounter(int *arr, int n);
int newPrimeCounter(int *intArr, int arrSize);
void printArray(int *primesArray, int size);
void master(int size);
void slave();
int *generatenewArr(int size);

//main() will be executed by each node
int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    size = 4000;
    originalArray = generatenewArr(size);

    newPrimeCounter(originalArray, size);

    // we are done with the MPI library so we must finalise it
    MPI_Finalize();
    delete originalArray;

    return 0;
}

int newPrimeCounter(int *intArr, int arrSize)
{
    if (world_rank == root)
    {
        //print the name and student number and processor model
        std::cout << "Name: Roberto Alejandro Rivera Mejia" << std::endl;
        std::cout << "No: 3019536" << std::endl;
        std::cout << "uP: Intel(R) Core(TM) i5-3230M CPU @ 2.60GHz" << std::endl;
        std::cout << "============== Rank " << root << " (root) =============== " << std::endl;
    }
    //new size that we will need to initialize the array
    //std::cout << "OriginalSize " << arrSize << std::endl;
    //we ceil the new size as it might contain decimal points
    //int newSize = (arrSize / 2) + 1;
    int newSize = (int)ceil(arrSize / 2) + 1;
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

    // if(world_rank == root){
    //     printArray(newArr, newSize);
    // }

    //as the new array is not evenly divisible by the world size we will increase its size by 1
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
        //e.g partitionSize = 252, offset = 5, worldSIze = 8
        // (offset * ((world_size / 2) - 1) --> = 5 * (8/2) - 1 --> 5 * 3 Then --> 252-15 = 237         
        for (unsigned int i = partitionSize; i > partitionSize - (offset * ((world_size / 2) - 1)); i--)
        {
            partition[i -1] = 0;
        }
        
    }
    printArray(partition,partitionSize);
    //primeCounter(partition, partitionSize);
}

int *generatenewArr(int size)
{
    int *arr = new int[size];
    for (unsigned int i = 0; i < size; i++)
    {
        int number = i + 1;
        arr[i] = number;
    }
    return arr;
}

int primeCounter(int *arr, int size)
{
    //initialise counter to 0
    unsigned int i, j,number, count = 0;
    bool isPrime;
    std::cout << "Rank " << world_rank  << " Primes: ";
    //the first of the prime numbers is 2, so we set our a variable = 2
    for (i = 0; i < size; i++){
        number = arr[i];
        //we will work with anything above 1
        if(number > 1){
            isPrime =true;
            for (j = 2; j <= number / 2; j++)
            {
                if(number % j == 0){
                    isPrime = false;
                    break;
                }                
            }
            if(isPrime){
                std::cout << number << ", ";
                count++;
            }        

        }   
        
    }
    std::cout << std::endl;
    std::cout << "Total: " << count << std::endl;
    //return the total count of prime numbers found
    return count;
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
/**
 public static int getLargestPrime(int number) {
        if(number > 1){
            for(int i =2; i < number; i++) {
                //System.out.println("first loop");
                while(number % i == 0 && number/i != 1) {
                    number /= i;
                //   System.out.println("number is now: "+number);
                }
            }
            return number;
        }
        return -1;
    } 
 * /

// int primeCounter(int n)
// {
//     //initialise counter to 0
//     int count = 0;
//     //the first of the prime numbers is 2, so we set our a variable = 2
//     for (int a = 2; a < n; a++)
//     {
//         //set the variable to true
//         bool isPrime = true;
//         //and then procced with the check to find out if its a prime number
//         for (int c = 2; c * c <= a; c++)
//         {
//             //if operation is equally divisible, then its not a prime number
//             if (a % c == 0)
//             {
//                 isPrime = false;
//                 break;
//             }
//         }
//         if (isPrime)
//         {
//             //if it is prime increase the counter by 1

//             count++;
//         }
//     }
//     //return the total count of prime numbers found
//     std::cout << "Rank " << world_rank << " prime numbers found: " << count << std::endl;
//     return count;
// }
// void master(int arrSize)
// {
//     //print the name and student number and processor model
//     std::cout << "Name: Roberto Alejandro Rivera Mejia" << std::endl;
//     std::cout << "No: 3019536" << std::endl;
//     std::cout << "uP: Intel(R) Core(TM) i5-3230M CPU @ 2.60GHz" << std::endl;
//     std::cout << "============== Rank " << root << " (root) =============== " << std::endl;

//     //new size that we will need to initialize the array
//     std::cout << "OriginalSize " << arrSize << std::endl;

//     //inititalise array
//     int *originalArray = new int[arrSize];
//     for (unsigned int i = 0; i < arrSize; i++)
//     {
//         int number = i + 1;
//         originalArray[i] = number;
//     }

//     //we ceil the new size as it might contain decimal points
//     //int newSize = (arrSize / 2) + 1;
//     int newSize = (int)ceil(arrSize / 2) + 1;
//     std::cout << "NewSize " << newSize << std::endl;
//     //initialize the array with the correct size we are going to need

//     newArr = new int[newSize];

//     //test
//     //for (unsigned int i = 0; i < newSize; i++) newArr[i] = 0;

//     //the following loop will ger rid of almost half of the non prime number before start the distribution of work
//     //set an increment to 1, we'll need it for moving spaces backwards to place numbers in order.
//     //as arrays start from Zero and we are starting from number 1, we increment 1 to our number to compare
//     //if the number is equals to 2 which is the only pair prime or number is NOT equaly divisible then its more likely to be a prime number
//     //Note: A prime number is any number that only has one and itself as factors. By definition even numbers have a factor of two.
//     //Therefore even numbers cannot be prime, with the exception of 2 itself. So 2 is the only even prime number.

//     int increment = 1;
//     for (unsigned int i = 0; i < arrSize; i++)
//     {
//         int number = i + 1;
//         if (number == 2 || number % 2 != 0)
//         {
//             if (number > 3)
//             {
//                 newArr[i - increment] = number;
//                 increment++;
//             }
//             else
//             {
//                 newArr[i] = number;
//             }
//         }
//         //end of the loop
//     }
//     //as the new array is not evenly divisible by the world size we will increase 1 to have enough space in eace partition
//     // partitionSize = (newSize / world_size + 1);
//     //partitionSize = newSize / world_size + 1;

//     offset = 0;
//     while (newSize % world_size != 0)
//     {
//         offset++;
//         newSize += offset;
//     }

//     // std::cout << "offset " << offset << std::endl;

//     //we now get the partition size for each node
//     partitionSize = (newSize / world_size);
//     //std::cout << "partitionSize " << partitionSize << std::endl;

//     std::cout << "Rank " << world_rank << " partitionSize " << partitionSize << std::endl;
//     partition = new int[partitionSize];
//     //printArray(newArr, newSize);
//     //send the partition size to the rest of the nodes
//     MPI_Bcast(&partitionSize, 1, MPI_INT, root, MPI_COMM_WORLD);
//     //this offset needs to reach the last node worldsize -1
//     MPI_Bcast(&offset, 1, MPI_INT, root, MPI_COMM_WORLD);
//     //scatter data to all of the nodes
//     MPI_Scatter(newArr, partitionSize, MPI_INT, partition, partitionSize, MPI_INT, root, MPI_COMM_WORLD);
//     printArray(partition, partitionSize);
// }

// void slave()
// {
//     //variable where we'll save the partition size
//     partitionSize = 0;
//     offset = 0;
//     //take part in the broadcast and receive the partition size that the root sent
//     MPI_Bcast(&partitionSize, 1, MPI_INT, root, MPI_COMM_WORLD);
//     //std::cout << "Rank " << world_rank <<" partition received " << partitionSize << std::endl;

//     //recive the offset from root, so we can
//     //this is gonna be used by the last node to make sure that the data sent in the extra spaces by root is Zero
//     MPI_Bcast(&offset, 1, MPI_INT, root, MPI_COMM_WORLD);
//     //std::cout << "Rank " << world_rank <<" offset received " << offset << std::endl;

//     //initializing array with the correct partition size
//     partition = new int[partitionSize];
//     //take part in the scatter operation and receive the partition of data from the main array contained in root
//     MPI_Scatter(NULL, partitionSize, MPI_INT, partition, partitionSize, MPI_INT, root, MPI_COMM_WORLD);
//     printArray(partition, partitionSize);
//     // int count;

//     // std::cout << "'\e[1m" << count << "\e[0m' Primes found in node " << world_rank << std::endl;
//     // return count;
// }

/*
compile
mpic++ Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2.cpp -o Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
run with 4 nodes
mpiexec -n 1 Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
mpiexec -n 8 --oversubscribe Roberto_Alejandro_Rivera_Mejia_3019536_Ass2_part2
*/
