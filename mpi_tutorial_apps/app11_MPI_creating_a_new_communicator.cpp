#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char **argv)
{
    // initialise the MPI libary
    MPI_Init(&argc, &argv);
    // determine the world size and the world rank
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int *group_ranks = new int[world_size / 2];
    if (world_rank < world_size / 2)
    {
        for (int i = 0; i < world_size / 2; i++)
            group_ranks[i] = i;
    }
    else
    {
        for (int i = world_size / 2, j = 0; i < world_size; i++, j++)
            group_ranks[j] = i;
    }

    // print out the group ranks
    std::cout << "rank " << world_rank << " group ranks: ";
    for (int i = 0; i < world_size / 2; i++)
        std::cout << group_ranks[i] << ", ";
    std::cout << std::endl;

    // create a new group out of the ranks and then create the communicator with that group
    MPI_Group new_group;
    MPI_Group_incl(world_group, world_size / 2, group_ranks, &new_group);
    MPI_Comm sub_comm;
    MPI_Comm_create(MPI_COMM_WORLD, new_group, &sub_comm);

    // we are done with the MPI library so we must finalise it
    MPI_Finalize();

    return 0;
}
