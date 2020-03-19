#include <iostream>
#include <mpi.h>

//variables to determine size of the world, rank, HashInput1, HashInput2,HashInput3.
int world_size, world_rank, HashInput1, HashInput2, HashInput3;


//worldsWorstworldsWorstHash1() performs a diference an return an int
int worldsWorstworldsWorstHash1(int ip)
{
    for (int l = 1; l < 100; l++)
    {
        ip = ip * l % 254;
    }
    return ip;
}

//worldsWorstworldsWorstHash2() performs a diference an return an int
int worldsWorstworldsWorstHash2(int ip)
{
    for (int l = 1; l < 50; l++)
    {
        ip = ((ip * l) + 2) % 254;
    }
    return ip;
}
//coordinator() will compute the difference of 2 values and pass it to the next node
void coordinator()
{
    //print the name and student number
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Roberto Alejandro Rivera Mejia" << std::endl;
    std::cout << "Student number: 3019536" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    HashInput1 = worldsWorstworldsWorstHash1(422) - worldsWorstworldsWorstHash2(233);
    //pass the difference of the two hashes to the next node.
    MPI_Send(&HashInput1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    //print our the output sent to the next node
    std::cout << "Rank " << world_rank << " sending: " << HashInput1 << std::endl;
    std::cout << "---------------------------------------" << std::endl;
}

//participant() receives the difference from the previous node will compute the difference of 2 values and pass it to the next node
void participant()
{
    HashInput1 = 0;
    //receive difference from the previous node
    MPI_Recv(&HashInput1, 1, MPI_INT, (world_rank + world_size - 1) % world_size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //compute differences 
    HashInput2 = worldsWorstworldsWorstHash1(HashInput1) - worldsWorstworldsWorstHash2(HashInput1);
    HashInput3 = worldsWorstworldsWorstHash1(HashInput2) - worldsWorstworldsWorstHash2(HashInput2);
    //send results to the next node
    MPI_Send(&HashInput2, 1, MPI_INT, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
    //print our the output sent to the next node
    std::cout << "Rank " << world_rank << " sending: " << HashInput2 << std::endl;
    //Print the final two hash values to the console.
    std::cout << "Result: "
              << "hash1= " << worldsWorstworldsWorstHash1(HashInput3) << " , "
              << "hash2= " << worldsWorstworldsWorstHash2(HashInput3) << std::endl;
    std::cout << "---------------------------------------" << std::endl;
}

int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0)
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
/*

compile
mpic++ Roberto_Alejandro_Rivera_Mejia_3019536_part1.cpp -o Roberto_Alejandro_Rivera_Mejia_3019536_part1
run with 4 nodes
mpiexec -n 4 Roberto_Alejandro_Rivera_Mejia_3019536_part1
run with 8 nodes
mpiexec -n 8 --oversubscribe Roberto_Alejandro_Rivera_Mejia_3019536_part1

*/