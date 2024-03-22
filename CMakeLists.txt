cmake_minimum_required(VERSION 3.27)
project(mpi_shared_memory_example C)

# Specification de la revision du langage C utilisee
set(CMAKE_C_STANDARD 11)

# Specification du fichier a compiler et le nom de notre executable
add_executable(mpi_shared_memory_1d_array mpi_shared_memory_1d_array.c)
# Specification de la liaison de notre programme a la bibliotheque MSMPI
target_link_libraries(mpi_shared_memory_1d_array msmpi)
# Specification du chemin vers les fichier d'entete la bibliotheque MSMPI
target_include_directories(mpi_shared_memory_1d_array PRIVATE $ENV{MSMPI_INC})
# Specification du chemin vers la bibliotheque MSMPI
target_link_directories(mpi_shared_memory_1d_array PRIVATE $ENV{MSMPI_LIB64})


add_executable(mpi_shared_memory_2d_array_v1 mpi_shared_memory_2d_array_v1.c)
target_link_libraries(mpi_shared_memory_2d_array_v1 msmpi)
target_include_directories(mpi_shared_memory_2d_array_v1 PRIVATE $ENV{MSMPI_INC})
target_link_directories(mpi_shared_memory_2d_array_v1 PRIVATE $ENV{MSMPI_LIB64})

add_executable(mpi_shared_memory_2d_array_v2 mpi_shared_memory_2d_array_v2.c)
target_link_libraries(mpi_shared_memory_2d_array_v2 msmpi)
target_include_directories(mpi_shared_memory_2d_array_v2 PRIVATE $ENV{MSMPI_INC})
target_link_directories(mpi_shared_memory_2d_array_v2 PRIVATE $ENV{MSMPI_LIB64})