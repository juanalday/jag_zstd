CMAKE_MINIMUM_REQUIRED(VERSION 3.14 FATAL_ERROR)
PROJECT(zstd CXX)

option(ENABLE_TEST "Enable tests" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(WIN32)
  add_compile_options(/Zc:preprocessor /Zc:__cplusplus)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -wd5105")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd5105")
endif()


# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(FetchContent)
set(ZSTD_BUILD_STATIC ON)
set(ZSTD_BUILD_SHARED OFF)
FetchContent_Declare(zstd
	GIT_REPOSITORY https://github.com/facebook/zstd
	GIT_TAG dev
	SOURCE_SUBDIR build/cmake
)
FetchContent_MakeAvailable(zstd)
if (NOT TARGET zstd::zstd)
	add_library(zstd::zstd ALIAS libzstd_static)
endif()

if(ENABLE_TEST)
  include(FetchContent)
  FetchContent_Declare(googletest
     GIT_REPOSITORY https://github.com/google/googletest.git
	 GIT_TAG main
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

add_subdirectory(src)