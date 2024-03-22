# include <stdlib.h>
# include <stdio.h>

# include "mpi.h"

/**
 * Affiche un matrice de nombre reels
 * @param matrix matrice a afficher
 * @param rows nombre de lignes
 * @param columns nombre de colonnes
 */
void printMatix(double **matrix, int rows, int columns);

/**
 * Ce programme montre un cas d'utilisation de la memoire partagée de MPI pour effectuer la
 * multiplication d'une matrice par 100
 */

int main(int argc, char *argv[]) {
    int rank, size, i, j;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    int rows = size, columns = size;
    // Objet window pour la premiere dimension
    MPI_Win win;
    // Objets window pour chaque ligne du tableau
    MPI_Win winD2[rows];

    double **shared;
    int d1DispUnit = sizeof(double *);
    int d2DispUnit = sizeof(double);
    MPI_Aint d1SharedMemorySize = rows * d1DispUnit;
    MPI_Aint d2SharedMemorySize = columns * d2DispUnit;

    // Appel collectif de la fonction d'allocation de memoire partagée pour la premiere dimension du tableau
    MPI_Win_allocate_shared(d1SharedMemorySize, d1DispUnit, MPI_INFO_NULL, MPI_COMM_WORLD, &shared, &win);
    // Appel collectif de la fonction d'allocation de memoire partagée pour la deuxieme dimension du tableau
    // Seul le processeur de rang 0 alloue effectivement de la memoire
    for (i = 0; i < rows; i++) {
        MPI_Win_allocate_shared(rank == 0 ? d2SharedMemorySize : 0, d2DispUnit, MPI_INFO_NULL, MPI_COMM_WORLD, &(shared[i]), &(winD2[i]));
    }

    if (rank != 0) {
        // Les processeurs de rang different de 0 récupèrent chacun un pointeur vers
        // la memoire allouée sur la deuxieme dimension par le processeur de rang 0
        for (i = 0; i < rows; i++) {
            MPI_Win_shared_query(winD2[i], 0, &d2SharedMemorySize, &d2DispUnit, &(shared[i]));
        }
    }

    // Le processeur de rang 0 rempli la matrice de sorte que chaque element a(i,j) contienne la somme i + j;
    if (rank == 0) {
        // Exclusivité de l'accès à la memoire partagée au processeur de rang 0
        for (i = 0; i < rows; i++) {
            for (j = 0; j < rows; j++) {
                shared[i][j] = i + j;
            }
        }

        // Affichage de la matrice avant calcul
        if (rank == 0) {
            puts("Matrice avant le calcul");
            printMatix(shared, rows, columns);
        }
    }

    // Avant d'effectuer les calculs , on s'assure que le processeur de rang 0
    // a terminé l'initialisation
    MPI_Barrier(MPI_COMM_WORLD);

    // chaque processeur verouille la ligne correspondant a son rang et la mutiplie par 100
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, winD2[rank]);
    for (i = 0; i < columns; i++) {
        shared[rank][i] = shared[rank][i] * 100; // Accès en lecture et en écriture a la memoire partagée
    }
    MPI_Win_unlock(rank, winD2[rank]);

    MPI_Barrier(MPI_COMM_WORLD);

    // Affichage de la matrice apres calcul
    if (rank == 0) {
        puts("Matrice apres le calcul");
        printMatix(shared, rows, columns);
    }

    MPI_Win_free(&win);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

void printMatix(double **matrix, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%g%s", matrix[i][j], (j + 1 == columns ? "\n" : "\t"));
        }
    }
}