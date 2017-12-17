
if(DEFINED TALSH_INSTALL_PATH AND EXISTS ${TALSH_INSTALL_PATH}/GPU/libtalsh.a)
message(STATUS "TALSH found at: ${TALSH_INSTALL_PATH}")
ADD_CUSTOM_TARGET(TALSH ALL)

else()
set (TALSH_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/TALSH)

if(EXISTS ${CMAKE_INSTALL_PREFIX}/TALSH/GPU/libtalsh.a)
    ADD_CUSTOM_TARGET(TALSH ALL)

else()
    message("Building TALSH")
    include(ExternalProject)
    
    ExternalProject_Add(TALSH_CPU
    GIT_REPOSITORY https://github.com/DmitryLyakh/TAL_SH.git
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_Fortran_COMPILER=${CMAKE_Fortran_COMPILER}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/TALSH/CPU -DMPI_INCLUDE_PATH=${MPI_INCLUDE_PATH}
    INSTALL_COMMAND make install)

    ExternalProject_Add(TALSH_GPU
    GIT_REPOSITORY https://github.com/DmitryLyakh/TAL_SH.git
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_Fortran_COMPILER=${CMAKE_Fortran_COMPILER}
    -DTALSH_GPU=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/TALSH/GPU -DMPI_INCLUDE_PATH=${MPI_INCLUDE_PATH}
    INSTALL_COMMAND make install)
endif()

endif()
