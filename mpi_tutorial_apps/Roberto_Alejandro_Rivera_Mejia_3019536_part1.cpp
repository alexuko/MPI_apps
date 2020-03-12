#include <iostream>
#include <mpi.h>

int world_size, world_rank, HashInput1, HashInput2, HashInput3;

int worldsWorstworldsWorstHash1 (int ip) {
	for (int l=1; l<100; l++) {
		ip = ip * l % 254;
	}
	return ip;
}

int worldsWorstworldsWorstHash2 (int ip) {
	for (int l=1; l<50; l++) {
		ip = ((ip * l) + 2) % 254;
	}
	return ip;
}

void coordinator(){    
    HashInput1 = worldsWorstworldsWorstHash1(422) - worldsWorstworldsWorstHash2(233);

    MPI_Send(&HashInput1,1,MPI_INT,1,0,MPI_COMM_WORLD);
    std::cout << "Rank "<< world_rank << " sending its result: " << HashInput1 << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    
}

void participant(){
    HashInput1 = 0;

    MPI_Recv(&HashInput1,1,MPI_INT,(world_rank + world_size - 1) % world_size,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    std::cout << "Rank "<< world_rank << " received ------> "<< HashInput1 << std::endl;

    HashInput2 = worldsWorstworldsWorstHash1(HashInput1) - worldsWorstworldsWorstHash2(HashInput1);
    HashInput3 = worldsWorstworldsWorstHash1(HashInput2) - worldsWorstworldsWorstHash2(HashInput2);
    std::cout << "Rank "<< world_rank << " result: " << HashInput2 << std::endl;

    MPI_Send(&HashInput2,1,MPI_INT,(world_rank + 1) % world_size,0, MPI_COMM_WORLD);
    std::cout << "Rank "<< world_rank << " sending its result: " << HashInput2 << std::endl;

    std::cout << "Result: " << "hash1= " << worldsWorstworldsWorstHash1(HashInput3) << " , " << "hash2= " << worldsWorstworldsWorstHash2(HashInput3)<<  std::endl;
    std::cout << "---------------------------------------" << std::endl;
    

}


int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    if(world_rank == 0){
        //print the name and student number
        std::cout << "---------------------------------------" << std::endl;
        std::cout << "Roberto Alejandro Rivera Mejia"<< std::endl;
        std::cout << "Student number: 3019536"<< std::endl;
        std::cout << "---------------------------------------" << std::endl;
        coordinator();
    }else{
        participant();
    }
    
    

    // we are done with the MPI library so we must finalise it
    MPI_Finalize();
    return 0;
}
/*

compile
mpic++ Roberto_Alejandro_Rivera_Mejia_3019536_part1.cpp -o Roberto_Alejandro_Rivera_Mejia_3019536_part1
run with 4 nodes
mpiexec -n 4 Roberto_Alejandro_Rivera_Mejia_3019536_part1
run with 8 nodes
mpiexec -n 8 --oversubscribe Roberto_Alejandro_Rivera_Mejia_3019536_part1

*/