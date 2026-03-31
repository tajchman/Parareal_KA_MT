set(CODE SequentialCode)

add_executable(${CODE} main.cxx ../wave2D.cxx)
set_target_properties(${CODE} PROPERTIES INSTALL_RPATH "$ORIGIN")
target_link_directories(${CODE} PUBLIC ${DEAL_II_PATH}/lib)
target_link_libraries(${CODE} PUBLIC ${DEAL_II_LIB} common)

install(TARGETS ${CODE} DESTINATION .)

