# include <stdlib.h>
# include <stdio.h>

# include "mpi.h"

/**
 * Type d'objet representant un tableau a deux dimensions.
 */
typedef struct {
    int rows;
    int columns;
    double *data;
} Array2D;

/**
 * Lit un element d'un Array2D
 * @param array Objet de type Array2D contenant des donnees
 * @param row ligne de l'element
 * @param column colonne de l'element
 * @return valeur lue
 */
double getAt(Array2D array, int row, int column);

/**
 *
 * @param array Objet de type Array2D
 * @param row ligne de l'element
 * @param column colonne de l'element
 * @param value valeur a ecrire
 */
void setAt(Array2D array, int row, int column, double value);

/**
 * Affiche le contenu d'un Array2D
 * @param array Objet de type Array2D
 */
void print2DArray(Array2D array);

/**
 * Ce programme montre un cas d'utilisation de la memoire partagée de MPI pour effectuer la
 * multiplication d'une matrice par 100
 */
int main(int argc, char *argv[]) {
    int rank, size, i, j;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Win win;

    Array2D shared;
    shared.rows = shared.columns = size;
    int displacementUnit = sizeof(double);
    MPI_Aint sharedMemorySize = displacementUnit * shared.rows * shared.columns;

    // Appel collectif de la fonction d'allocation de memoire partagée
    // Seul le processeur de rank 0 alloue effectivement de la memoire
    MPI_Win_allocate_shared(rank == 0 ? sharedMemorySize : 0, displacementUnit, MPI_INFO_NULL, MPI_COMM_WORLD, &(shared.data), &win);

    if (rank != 0) {
        // Les processeurs de rang different de 0 récupèrent chacun un pointeur vers
        // la memoire allouée par le processeur de rang 0
        MPI_Win_shared_query(win, 0, &sharedMemorySize, &displacementUnit, &(shared.data));
    }

    // Le processeur de rang 0 rempli la matrice de sorte que chaque element a(i,j) contienne la somme i + j;
    if (rank == 0) {
        for (i = 0; i < shared.rows; i++) {
            for (j = 0; j < shared.columns; j++) {
                setAt(shared, i, j, i + j);
            }
        }

        // Affichage de la matrice avant calcul
        puts("Matrice avant le calcul");
        print2DArray(shared);
    }

    // Avant d'effectuer les calculs , on s'assure que le processeur de rang 0
    // a terminé l'initialisation
    MPI_Barrier(MPI_COMM_WORLD);

    // chaque processeur verouille la ligne correspondant a son rang et la mutiplie par 100
    for (i = 0; i < shared.rows; i++) {
        for (j = 0; j < shared.columns; j++) {
            if (i == rank) {
                double value = getAt(shared, i, j) * 100; // Accès en lecture a la memoire partagée
                setAt(shared, i, j, value); // Accès en écriture a la memoire partagée
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Affichage de la matrice apres calcul
    if (rank == 0) {
        puts("Matrice apres le calcul");
        print2DArray(shared);
    }

    MPI_Win_free(&win);
    MPI_Finalize();
    return EXIT_SUCCESS;
}


double getAt(Array2D array, int row, int column) {
    return array.data[row * array.columns + column];
}

void setAt(Array2D array, int row, int column, double value) {
    array.data[row * array.columns + column] = value;
}

void print2DArray(Array2D array) {
    for (int i = 0; i < array.rows; i++) {
        for (int j = 0; j < array.columns; j++) {
            printf("%g%s", getAt(array, i, j), (j + 1 == array.columns ? "\n" : "\t"));
        }
    }
}