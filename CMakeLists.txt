cmake_minimum_required(VERSION 3.27)
project(mpi_shared_memory_example C)

# Specification de la revision du langage C utilisee
set(CMAKE_C_STANDARD 17)

# Specification du fichier a compiler et le nom de notre executable
add_executable(mpi_shared_memory_example mpi_shared_memory_example.c)

# Specification de la liaison de notre programme a la bibliotheque MSMPI
target_link_libraries(mpi_shared_memory_example msmpi)

# Specification du chemin vers les fichier d'entete la bibliotheque MSMPI
target_include_directories(mpi_shared_memory_example PRIVATE $ENV{MSMPI_INC})

# Specification du chemin vers la bibliotheque MSMPI
target_link_directories(mpi_shared_memory_example PRIVATE $ENV{MSMPI_LIB64})
