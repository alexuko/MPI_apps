#include <iostream>
#include <mpi.h>
#include <cstdlib>

int main(int argc, char **argv){
    MPI_Init(&argc, &argv);
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int local_value;
    local_value = rand() % 50;
    //local_value = atoi(argv[1]);

    int other_value;
   
    
// v1 = rand() % 100;         // v1 in the range 0 to 99
// v2 = rand() % 100 + 1;     // v2 in the range 1 to 100
// v3 = rand() % 30 + 1985;   // v3 in the range 1985-2014 


  if (rank == 0) {
        // 1- Send the value to process 1 
        MPI_Send(&local_value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        // 2- Receive the value from process 1 (in other_value)
        MPI_Recv(&other_value, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // 3- Print the sum of the two values on stdout 
        std::cout << "Sum: " <<  local_value + other_value   << std::endl;
  }
  else {
        other_value = rand() % 10;
        // 1- Receive the value from process 0 (in other_value)
        MPI_Recv(&local_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // 2- Send the value to process 0
        MPI_Send(&other_value, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        // 3- Print the product of the two values on stdout
        std::cout << "other_value: " <<  other_value << " & local_value: " << local_value << std::endl;
    
  }

    MPI_Finalize();
    return 0;

}
