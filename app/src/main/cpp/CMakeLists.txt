cmake_minimum_required(VERSION 3.10.2)
project(katago)

set(CMAKE_CXX_STANDARD 14)

include_directories(external)
include_directories(external/tclap-1.2.2/include)
include_directories(SYSTEM external/filesystem-1.5.8/include) #SYSTEM suppresses a few warnings
include_directories(external)
AUX_SOURCE_DIRECTORY(${CMAKE_SOURCE_DIR}/../eigen EIGEN3_INCLUDE_DIRS)



##--------------------------- PLATFORM SPECIFIC -----------------------------------------------------

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColorReset   "${Esc}[m")
  set(ColorBold    "${Esc}[1m")
  set(ColorRed     "${Esc}[31m")
  set(ColorBoldRed "${ColorRed}${ColorBold}")
endif()
##--------------------------------------------------------------------------------------------------

##--------------------------------------------zlib set----------------------------------------------
#file(GLOB ZIP_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/zip/*.c ${CMAKE_CURRENT_SOURCE_DIR}/zip/*.h)
#set(Zip_Source ${ZIP_SRC})
#add_library(mzip SHARED ${ZIP_SRC})

add_library(mzip SHARED IMPORTED)
set_target_properties(mzip PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libmzip.so)
##--------------------------------------------------------------------------------------------------


##--------------------------- CMAKE VARIABLES (partly for Cmake GUI) -------------------------------
#
#set(BUILD_DISTRIBUTED 0 CACHE BOOL "Build with http support for contributing to distributed training")
#set(USE_BACKEND CACHE STRING "Neural net backend")
#string(TOUPPER "${USE_BACKEND}" USE_BACKEND)
#set_property(CACHE USE_BACKEND PROPERTY STRINGS "" CUDA TENSORRT OPENCL EIGEN)
#
#set(USE_TCMALLOC 0 CACHE BOOL "Use TCMalloc")
#set(NO_GIT_REVISION 0 CACHE BOOL "Disable embedding the git revision into the compiled exe")
#set(USE_AVX2 0 CACHE BOOL "Compile with AVX2")
#set(USE_BIGGER_BOARDS_EXPENSIVE 0 CACHE BOOL "Allow boards up to size 29. Compiling with this will use more memory and slow down KataGo, even when playing on boards of size 19.")
##--------------------------------------------------------------------------------------------------


##--------------------------- NEURAL NET BACKEND ---------------------------------------------------
#if(USE_BACKEND STREQUAL "OPENCL")
#  message(STATUS "-DUSE_BACKEND=OPENCL, using OpenCL backend.")
#  set(NEURALNET_BACKEND_SOURCES
#    neuralnet/openclbackend.cpp
#    neuralnet/openclkernels.cpp
#    neuralnet/openclhelpers.cpp
#    neuralnet/opencltuner.cpp
#    )
#elseif(USE_BACKEND STREQUAL "EIGEN")
#  message(STATUS "-DUSE_BACKEND=EIGEN, using eigen1.Eigen CPU backend.")
#  if(NOT USE_AVX2)
#    message(STATUS "You can also specify USE_AVX2 (-DUSE_AVX2=1 on command line) if you have a modern CPU for better performance.")
#  endif()
#  set(NEURALNET_BACKEND_SOURCES
#    neuralnet/eigenbackend.cpp
#    )
#elseif(USE_BACKEND STREQUAL "")
#  message(WARNING "${ColorBoldRed}WARNING: Using dummy neural net backend, intended for non-neural-net testing only, will fail on any code path requiring a neural net. To use neural net, specify -DUSE_BACKEND=CUDA or -DUSE_BACKEND=TENSORRT or -DUSE_BACKEND=OPENCL or -DUSE_BACKEND=EIGEN to compile with the respective backend.${ColorReset}")
#  set(NEURALNET_BACKEND_SOURCES neuralnet/dummybackend.cpp)
#else()
#  message(FATAL_ERROR "Unrecognized backend: " ${USE_BACKEND})
#endif()

message(STATUS "-DUSE_BACKEND=EIGEN, using Eigen CPU backend.")
if(NOT USE_AVX2)
  message(STATUS "You can also specify USE_AVX2 (-DUSE_AVX2=1 on command line) if you have a modern CPU for better performance.")
endif()
set(NEURALNET_BACKEND_SOURCES
        neuralnet/eigenbackend.cpp
        )
#set(NEURALNET_BACKEND_SOURCES
##        neuralnet/eigenbackend.cpp
#        neuralnet/dummybackend.cpp
#        )
##--------------------------------------------------------------------------------------------------

##--------------------------- KATAGO COMPILING AND LINKING -----------------------------------------
file(GLOB KATAGO_SRC
        ${CMAKE_SOURCE_DIR}/core/*.cpp ${CMAKE_SOURCE_DIR}/core/*.h
        ${CMAKE_SOURCE_DIR}/game/*.cpp ${CMAKE_SOURCE_DIR}/game/*.h
        ${CMAKE_SOURCE_DIR}/dataio/*.cpp ${CMAKE_SOURCE_DIR}/dataio/*.h
        ${CMAKE_SOURCE_DIR}/book/*.cpp ${CMAKE_SOURCE_DIR}/book/*.h
        ${CMAKE_SOURCE_DIR}/search/*.cpp ${CMAKE_SOURCE_DIR}/search/*.h
        ${CMAKE_SOURCE_DIR}/program/*.cpp ${CMAKE_SOURCE_DIR}/program/*.h
        ${CMAKE_SOURCE_DIR}/tests/*.cpp ${CMAKE_SOURCE_DIR}/tests/*.h
        ${CMAKE_SOURCE_DIR}/command/*.cpp ${CMAKE_SOURCE_DIR}/command/*.h
        ${NEURALNET_BACKEND_SOURCES}
        neuralnet/nninputs.cpp
        neuralnet/modelversion.cpp
        neuralnet/nneval.cpp
        neuralnet/desc.cpp
        main.cpp
        )
set(Katago_Source ${KATAGO_SRC})
add_library(katago
        SHARED
        ${Katago_Source}
        )



##----------------------------------------------eigen-----------------------------------------------
target_compile_definitions(katago PRIVATE USE_EIGEN_BACKEND)
if(NOT (MSVC OR MINGW))
#  find_package(Eigen3 REQUIRED)
  include_directories(SYSTEM ${EIGEN3_INCLUDE_DIRS})
  message(STATUS "Found Eigen3 at ${EIGEN3_INCLUDE_DIRS}")
else()
  if(EIGEN3_INCLUDE_DIRS)
    message(STATUS "EIGEN3_INCLUDE_DIRS is set to ${EIGEN3_INCLUDE_DIRS}, assuming that Eigen3 header files are here.")
    include_directories(SYSTEM ${EIGEN3_INCLUDE_DIRS})
  else()
#    find_package(Eigen3)
    if(NOT Eigen3_FOUND)
      set(EIGEN3_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIRS} CACHE PATH "Directory containing 'Eigen' and 'unsupported' subdirs with Eigen headers")
      message(FATAL_ERROR "*** KataGo: Eigen3 was not found. If you've downloaded Eigen3, you can IGNORE the whole above error from CMake and just set 'EIGEN3_INCLUDE_DIRS' to the root eigen3-3.*.* directory after unzipping it. Among other things it should contain 'Eigen' and 'unsupported' subdirs, which is all KataGo needs. Otherwise, you will need to 'build' Eigen3 even though it is header-only and install it via MSVC in order for cmake to find it via FindEigen3.cmake.")
    else()
      include_directories(SYSTEM ${EIGEN3_INCLUDE_DIRS})
      message(STATUS "Found Eigen3 at ${EIGEN3_INCLUDE_DIRS}")
    endif()
  endif()
endif()
##--------------------------------------------------------------------------------------------------


##------------------------------------------------opencl--------------------------------------------

#set(cl_stub_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#set(cl_stub_SRC "${cl_stub_DIR}/cl_stub-master")
#
#
#set(cl_stub_build_DIR ${PROJECT_BINARY_DIR}/cl_stub_outputs)
#file(MAKE_DIRECTORY ${cl_stub_build_DIR})
#
#include_directories( ${cl_stub_SRC}/include/ )
#add_subdirectory(${cl_stub_SRC} ${cl_stub_build_DIR})
#add_library(libcl_stub SHARED IMPORTED)
#set_target_properties(libcl_stub PROPERTIES IMPORTED_LOCATION ${cl_stub_build_DIR}/${ANDROID_ABI}/libcl_stub.so)



#target_compile_definitions(katago PRIVATE USE_OPENCL_BACKEND)
#add_library(libcl_stub SHARED IMPORTED)
#set_target_properties(libcl_stub PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libcl_stub.so)
##--------------------------------------------------------------------------------------------------


##--------------------------------------------board set---------------------------------------------
if(USE_BIGGER_BOARDS_EXPENSIVE)
  target_compile_definitions(katago PRIVATE COMPILE_MAX_BOARD_LEN=29)
endif()

#if(NO_GIT_REVISION AND (NOT BUILD_DISTRIBUTED))
#  target_compile_definitions(katago PRIVATE NO_GIT_REVISION)
#endif()
##--------------------------------------------------------------------------------------------------


##--------------------------------------------CMAKE_CXX---------------------------------------------
#if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
#  message(STATUS "Setting up build for GNU or Clang.")
#  if(NOT (${CMAKE_SYSTEM_PROCESSOR} MATCHES "(arm|aarch32|aarch64)"))
#    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -mfpmath=sse")
#  endif()
#  if(USE_AVX2)
#    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
#    target_compile_definitions(katago PRIVATE USE_AVX2)
#  endif()
#
#  find_package (Threads REQUIRED)
#  target_link_libraries(katago Threads::Threads)
#
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -pedantic -Wall -Wextra -Wno-sign-compare -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Wmissing-declarations -Wmissing-include-dirs -Woverloaded-virtual -Wredundant-decls -Wshadow -Wstrict-overflow=1 -Wswitch-default -Wfloat-conversion -Wunused")
#
#  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#    message(STATUS "Enabling GNU-specific build options.")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op -Wnoexcept -Wstrict-null-sentinel")
#    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6.1)
#      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnull-dereference -Walloc-zero -Wduplicated-branches -Wduplicated-cond -Wdangling-else -Wrestrict")
#    endif()
#  else()
#    message(STATUS "Enabling Clang-specific build options.")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnull-dereference -Wdangling-else")
#    target_link_libraries(katago "atomic")
#  endif()
#
#  if(USE_TCMALLOC)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
#  endif()
#
#endif()
#
#target_include_directories(katago PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
#---------------------------------------------------------------------------------------------------


#----------------------------------------------target link------------------------------------------

find_library(log-lib log)
#find_library(z-lib z)

target_link_libraries(katago
        mzip
#        ${z-lib}
#        libcl_stub
        ${log-lib}
#        ${EXTERNAL_LIB}
        jnigraphics)

#---------------------------------------------------------------------------------------------------


##--------------------------- TCMALLOC -------------------------------------------------------------
#
#if(USE_TCMALLOC)
#  message(STATUS "-DUSE_TCMALLOC=1 is set, using tcmalloc as the allocator")
#  find_library(TCMALLOC_LIB NAMES tcmalloc_minimal HINTS /usr)
#  if(NOT TCMALLOC_LIB)
#    message(FATAL_ERROR "Could not find tcmalloc")
#  endif()
#endif()
#
## set (Gperftools_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake/")
## find_package(Gperftools REQUIRED)
##--------------------------------------------------------------------------------------------------



##--------------------------- GIT ------------------------------------------------------------------
##
##if(NO_GIT_REVISION AND (NOT BUILD_DISTRIBUTED))
##  message(STATUS "-DNO_GIT_REVISION=1 is set, avoiding including the Git revision in compiled executable")
##  unset(GIT_HEADER_FILE_ALWAYS_UPDATED)
##else()
##  if(NO_GIT_REVISION AND BUILD_DISTRIBUTED)
##    message(STATUS "${ColorRed}NO_GIT_REVISION is set, but BUILD_DISTRIBUTED is also set and distributed requires git revision, so ignoring NO_GIT_REVISION.${ColorReset}")
##  elseif(BUILD_DISTRIBUTED)
##    message(STATUS "Including Git revision in the compiled executable")
##  else()
##    message(STATUS "Including Git revision in the compiled executable, specify -DNO_GIT_REVISION=1 to disable")
##  endif()
##  find_package(Git)
##  if(NOT GIT_FOUND)
##    set(GIT_EXECUTABLE ${GIT_EXECUTABLE} CACHE FILEPATH "Path to git executable")
##    mark_as_advanced(CLEAR GIT_EXECUTABLE)
##    if(BUILD_DISTRIBUTED)
##      message(SEND_ERROR "${ColorBoldRed}Git executable was not found, specify GIT_EXECUTABLE as the path to the git executable.${ColorReset}")
##    else()
##      message(SEND_ERROR "${ColorBoldRed}Git executable was not found. Either specify GIT_EXECUTABLE as the path to the git executable, or use NO_GIT_REVISION to disable.${ColorReset}")
##    endif()
##  endif()
##  set(GIT_HEADER_FILE_TEMPLATE_BARE program/gitinfotemplate.h)
##  set(GIT_HEADER_FILE_ALWAYS_UPDATED_BARE program/gitinfoupdated.h)
##  set(GIT_HEADER_FILE_BARE program/gitinfo.h)
##  set(GIT_HEADER_FILE_TEMPLATE ${CMAKE_SOURCE_DIR}/${GIT_HEADER_FILE_TEMPLATE_BARE})
##  set(GIT_HEADER_FILE_ALWAYS_UPDATED ${CMAKE_BINARY_DIR}/${GIT_HEADER_FILE_ALWAYS_UPDATED_BARE})
##  set(GIT_HEADER_FILE ${CMAKE_BINARY_DIR}/${GIT_HEADER_FILE_BARE})
##  add_custom_command(
##    OUTPUT ${GIT_HEADER_FILE_ALWAYS_UPDATED}
##    COMMAND ${CMAKE_COMMAND} -E copy ${GIT_HEADER_FILE_TEMPLATE} ${GIT_HEADER_FILE_ALWAYS_UPDATED}
##    COMMAND ${GIT_EXECUTABLE} describe --match=DummyTagNotExisting --always --abbrev=40 --dirty >> ${GIT_HEADER_FILE_ALWAYS_UPDATED}
##    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GIT_HEADER_FILE_ALWAYS_UPDATED} ${GIT_HEADER_FILE}
##    COMMAND ${CMAKE_COMMAND} -E remove ${GIT_HEADER_FILE_ALWAYS_UPDATED}
##    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
##    VERBATIM
##    )
##endif()
##---------------------------------------------------------------------------------------------------


##--------------------------------------------TCMALLOC set-------------------------------------------
#if(USE_TCMALLOC)
#  target_link_libraries(katago ${TCMALLOC_LIB})
#endif(USE_TCMALLOC)
#
#if(BUILD_DISTRIBUTED)
#  message(STATUS "-DBUILD_DISTRIBUTED=1 is set, compiling code and dependencies to contribute to distributed training")
#  target_compile_definitions(katago PRIVATE BUILD_DISTRIBUTED)
#  find_package(OpenSSL REQUIRED)
#  target_link_libraries(katago ${OPENSSL_SSL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARIES})
#  include_directories(${OPENSSL_INCLUDE_DIR})
#  include_directories(external/httplib)
#endif()
#
##---------------------------------------------------------------------------------------------------


##-----------------------------------------MSVC------------------------------------------------------
#
## add_compile_definitions(NDEBUG)
#
#if(MSVC)
#  message(STATUS "Setting up build for MSVC.")
#  # Suppress min and max macros on windows
#  # Also define a few other things for windows
#  target_compile_definitions(katago PRIVATE NOMINMAX)
#  target_compile_definitions(katago PRIVATE BYTE_ORDER=1234)
#  target_compile_definitions(katago PRIVATE LITTLE_ENDIAN=1234)
#  target_compile_definitions(katago PRIVATE BIG_ENDIAN=4321)
#  # core/rand.cpp uses winsock for a gethostname
#  target_link_libraries(katago ws2_32)
#
#  if(USE_AVX2)
#    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /arch:AVX2 -D__FMA__")
#    target_compile_definitions(katago PRIVATE USE_AVX2)
#  endif()
#
#  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:8388608")
#endif()
##---------------------------------------------------------------------------------------------------


