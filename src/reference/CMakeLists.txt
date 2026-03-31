
add_executable(step_23_reference step_23.cxx )
set_target_properties(step_23_reference PROPERTIES INSTALL_RPATH "$ORIGIN")
target_link_directories(step_23_reference PUBLIC ${DEAL_II_PATH}/lib)
target_link_libraries(step_23_reference PUBLIC common ${DEAL_II_LIB})

install(TARGETS step_23_reference DESTINATION .)
