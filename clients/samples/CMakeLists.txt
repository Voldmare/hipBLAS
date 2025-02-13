# ########################################################################
# Copyright 2016-2022 Advanced Micro Devices, Inc.
# ########################################################################

set( hipblas_samples_common ../common/utility.cpp )

add_executable( example-sscal example_sscal.cpp ${hipblas_samples_common} )
add_executable( example-sgemm example_sgemm.cpp ${hipblas_samples_common} )
add_executable( example-sgemm-strided-batched example_sgemm_strided_batched.cpp ${hipblas_samples_common} )
add_executable( example-c example-c.c ${hipblas_samples_common} )
add_executable( example-hip-complex-her2 example_hip_complex_her2.cpp ${hipblas_samples_common} )

if( CMAKE_CXX_COMPILER MATCHES ".*/hcc$|.*/hipcc$" )
  add_executable( example-hgemm example_hgemm.cpp ${hipblas_samples_common} )
endif( )

if(NOT WIN32)
    #add_executable( example-sscal-fortran example_sscal_fortran.f90 )
endif()

# We test for C99 compatibility in the example-c.c test
set_source_files_properties(example-c.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(example-c.c PROPERTIES COMPILE_FLAGS "-xc -std=c99")

# We test for C++11 compatibility in one of the samples
#set_source_files_properties(example_sgemm_strided_batched.cpp PROPERTIES COMPILE_FLAGS "-std=c++11")

if( NOT TARGET hipblas )
  find_package( hipblas CONFIG PATHS ${ROCM_PATH}/hipblas )
  if( NOT hipblas_FOUND )
    message( FATAL_ERROR "hipBLAS is a required dependency and is not found; try adding rocblas path to CMAKE_PREFIX_PATH")
  endif( )
endif( )

list (APPEND example-executables example-sscal example-sgemm example-sgemm-strided-batched example-c example-hip-complex-her2)
if( CMAKE_CXX_COMPILER MATCHES ".*/hcc$|.*/hipcc$" )
  list (APPEND example-executables example-hgemm)
endif( )

foreach( exe IN LISTS example-executables)

  # External header includes included as SYSTEM files
  target_include_directories( ${exe}
    SYSTEM PRIVATE
      $<BUILD_INTERFACE:${HIP_INCLUDE_DIRS}>
  )

  target_include_directories( ${exe} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include> )

  target_compile_options( ${exe} PRIVATE -mf16c )

  target_link_libraries( ${exe} PRIVATE roc::hipblas )

  if( NOT USE_CUDA )
    target_link_libraries( ${exe} PRIVATE hip::host )

    if( CUSTOM_TARGET )
      target_link_libraries( ${exe} PRIVATE hip::${CUSTOM_TARGET} )
    endif( )

  else( )
    target_compile_definitions( ${exe} PRIVATE __HIP_PLATFORM_NVCC__ )

    target_include_directories( ${exe}
      PRIVATE
        $<BUILD_INTERFACE:${CUDA_INCLUDE_DIRS}>
    )

    target_link_libraries( ${exe} PRIVATE ${CUDA_LIBRARIES} )
  endif( )

  set_target_properties( ${exe} PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
    DEBUG_POSTFIX "-d"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/staging" )

  rocm_install(TARGETS ${exe} COMPONENT samples)

endforeach( )
