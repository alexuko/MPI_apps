#include <iostream>
#include <mpi.h>
#include <math.h> 
//variables that we are going to need for the MPI program
int *arr, world_size, world_rank, arr_size = 40;    

//printArray() will print out the values in an array 
void printArray(int *arr, int size){
    std::cout << "Values in Array: " << std::endl;
    for (int i = 0; i < size; i++){
        //print out each value of the array
        std::cout << arr[i] << ", ";
        if(i % 10 == 9) std::cout << std::endl;
    }
    
}

//sum()
float sum(int *arr, int size){
    int sum = 0;
    for (int i = 0; i < size; i++){
        //sum all of the values from the array
        sum += arr[i];
    }
    return sum;    
}

float get_median(int *arr, int size){
    //print out to check results are correct
    std::cout << "Median: " << sum(arr,size)/size << std::endl;
    //return --> median = Divide the sum by how array size  
    return sum(arr,size) / size;   
}

float get_variance(int *arr, int size){
    //declare variable that will hold the sum
    float sum = 0;
    //declare variable and get the median value 
    float median = get_median(arr,size); 
    for (unsigned int i = 0; i < size; i++){
        //Square all of the numbers from each of the subtractions and add to the sum
        sum += (arr[i] - median) * (arr[i] - median);
        //std::cout << i << " variance val: " << (arr[i] - median) * (arr[i] - median) << std::endl;
    }
    //print out the variance to che it is correct
    std::cout << "Variance: " << sum / (size - 1) << std::endl;
    // return --> variance = Divide the sum of squares by (n-1)
    return sum / (size - 1);
}

float get_standar_deviation(int *arr, int size){
    //declare variable and get variance
    float variance = get_variance(arr,size);
    //print out the Standard Deviation to chek it is correct
    std::cout << "Standar deviation: " << sqrt(variance) << std::endl;
    //Standard Deviation = the square root of the variance
    return sqrt(variance);
}

float sumDiferences(int *arr, int size, float median){
    //variable to hold the sum
    int sum = 0;
        for (int i = 0; i < size; i++){
            //Square all of the numbers from each of the subtractions and add to the sum
            sum += (arr[i] - median) * (arr[i] - median);
        }
    //return the sum of the differences    
    return sum;
    
}

int *generate_array(int arr_size){
    //inittialize the array with the desired number of values
    int *arr = new int[arr_size];
    //for loop to generate numbers in the array 
    for (unsigned int i = 0; i < arr_size; i++){
        //assign a random value for each of the positions in the array
        arr[i] = rand() % 50;
    }
    //return the array
    return arr;
}

void coordinator(){
    //print the name and student number
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Roberto Alejandro Rivera Mejia"<< std::endl;
    std::cout << "Student number: 3019536"<< std::endl;
    std::cout << "---------------------------------------" << std::endl;
    //seed the random number generator with the value of 1
    srand(1);
    //generate the array with the desired size
    arr = generate_array(arr_size);
    //variable to hold the partition 
    int partition = arr_size / world_size;
    printArray(arr,arr_size);
    
}

void participant(){
    
}


int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
       
    if(world_rank == 0){
        coordinator();
    }else{
        participant();
    }
     

    // we are done with the MPI library so we must finalise it
    MPI_Finalize();
    //deallocate memory
    delete arr;
    return 0;
}
/*

compile
mpic++ Roberto_Alejandro_Rivera_Mejia_3019536_part2.cpp -o Roberto_Alejandro_Rivera_Mejia_3019536_part2
run with 4 nodes
mpiexec -n 4 Roberto_Alejandro_Rivera_Mejia_3019536_part2
run with 8 nodes
mpiexec -n 8 --oversubscribe Roberto_Alejandro_Rivera_Mejia_3019536_part2

*/