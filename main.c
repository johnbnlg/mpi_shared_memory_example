# include <stdlib.h>
# include <stdio.h>

# include "mpi.h"

/**
 * Ce programme montre un cas d'utilisation de la memoire partagée de MPI pour calculer la somme des
 * des carres des n (n étant le nombre de processeur utilisés) premiers entiers strictement positifs.
 */

int main(int argc, char *argv[]) {
    int rank, size, i;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Win win;

    int *shared;
    int disp_unit = sizeof(int);
    MPI_Aint sharedMemorysize = disp_unit * size;

    // Appel collectif de la fonction d'allocation de memoire partagée
    // Seul le processeur de rank 0 alloue effectivement de la memoire
    MPI_Win_allocate_shared(rank == 0 ? sharedMemorysize : 0, disp_unit, MPI_INFO_NULL, MPI_COMM_WORLD, &shared, &win);

    if (rank != 0) {
        // Les processeurs de rang different de 0 récupèrent chacun un pointeur vers
        // la memoire allouée par le processeur de rang 0
        MPI_Win_shared_query(win, 0, &sharedMemorysize, &disp_unit, &shared);
    }

    // Le processeur de rang 0 rempli la memoire partagée de nombres allant de 1 au nombre de processeurs
    if (rank == 0) {
        // Exclusivité de l'accès à la memoire partagée au processeur de rang 0
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        for (i = 0; i < size; i++) {
            shared[i] = i + 1;
        }
        MPI_Win_unlock(0, win);// Déverrouillage de l'accès à la memoire
    }

    // Avant de faire effectuer les calculs par chaque processeur, on s'assure que le processeur de rang 0
    // a terminé l'initialisation
    MPI_Barrier(MPI_COMM_WORLD);
    shared[rank] = shared[rank] * shared[rank]; // Accès en lecture et en écriture a la memoire partagée
    MPI_Barrier(MPI_COMM_WORLD);

    // Avant de sommer et afficher, on s'assure que tout le monde a terminé le calcul
    if (rank == 0) {
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        int somme = 1;
        for (i = 0; i < size; i++) {
            somme += shared[i];
        }
        MPI_Win_unlock(0, win);
        printf("La somme des carres des %d premiers entiers positifs vaut %d ", size, somme);
    }

    MPI_Win_free(&win);
    MPI_Finalize();
    return EXIT_SUCCESS;
}
