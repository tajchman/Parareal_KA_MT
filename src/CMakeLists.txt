cmake_minimum_required(VERSION 3.12)
project(Parareal)
enable_testing()

set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(MPI REQUIRED)

include(ExternalProject)
include(ProcessorCount)

ProcessorCount(NCORES)
math(EXPR NPROCS "${NCORES}/3")

set(DEAL_II_VERSION "9.7.1")
ExternalProject_Add(DEAL.II
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/DEAL.II
  BINARY_DIR ${CMAKE_BINARY_DIR}/DEAL.II/build
  DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/download
  DOWNLOAD_EXTRACT_TIMESTAMP false
  URL_HASH SHA256=0f2096ef83db54fdcebe9f3d148fa713f63f1c3f567941b53bcb4a1a8ea7de43
  URL "https://github.com/dealii/dealii/releases/download/v${DEAL_II_VERSION}/dealii-${DEAL_II_VERSION}.tar.gz"
  PATCH_COMMAND python ${CMAKE_SOURCE_DIR}/patch.py ${CMAKE_SOURCE_DIR}
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/DEAL.II/install
             -DDEAL_II_WITH_TBB=OFF
             -DDEAL_II_WITH_VTK=OFF
             -DDEAL_II_WITH_MPI=OFF
             -DDEAL_II_WITH_KOKKOS=OFF
             -DDEAL_II_COMPONENT_DOCUMENTATION=OFF
             -DDEAL_II_COMPONENT_EXAMPLES=OFF
  BUILD_COMMAND ${CMAKE_COMMAND} -E env CMAKE_BUILD_PARALLEL_LEVEL=${NPROCS} ${CMAKE_COMMAND} --build .
  )
set(DEAL_II_PATH ${CMAKE_BINARY_DIR}/DEAL.II/install)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(DEAL_II_LIB deal_II)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(DEAL_II_LIB deal_II.g)
endif()

install(FILES ${DEAL_II_PATH}/lib/lib${DEAL_II_LIB}.so DESTINATION .)
install(FILES ${DEAL_II_PATH}/lib/lib${DEAL_II_LIB}.so.${DEAL_II_VERSION} DESTINATION .)


include_directories(${DEAL_II_PATH}/include)
include_directories(${DEAL_II_PATH}/include/deal.II/bundled)

add_subdirectory(common)
include_directories(common ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(reference)
add_subdirectory(sequential)
add_subdirectory(parareal)