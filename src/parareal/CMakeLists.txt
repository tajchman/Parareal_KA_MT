enable_testing()

set(CODE PararealCode)

add_executable(${CODE} main.cxx ../wave2D.cxx)
set_target_properties(${CODE} PROPERTIES INSTALL_RPATH "$ORIGIN")
target_link_directories(${CODE} PUBLIC ${DEAL_II_PATH}/lib)
target_link_libraries(${CODE} PUBLIC ${DEAL_II_LIB} common MPI::MPI_CXX)

install(TARGETS ${CODE} DESTINATION .)

add_test(NAME Parareal_1 
         COMMAND mpirun -n 3 ${CMAKE_INSTALL_PREFIX}/PararealCode -tmax 4.5 -dt_coarse 0.05 -dt_fine 0.001
         WORKING_DIRECTORY ${TEST_DIR})
         
add_test(NAME Plot_1 
         COMMAND python ${CMAKE_SOURCE_DIR}/plot.py --dir .
         WORKING_DIRECTORY ${TEST_DIR})