add_library(zstdpp STATIC compressor.hpp compressor.cpp)
target_include_directories(zstdpp PRIVATE ${zstd_SOURCE_DIR}/lib)
target_link_libraries(zstdpp PUBLIC zstd_static)

if(ENABLE_TEST)
  enable_testing()
  include(GoogleTest)
  add_executable(compressor_test compressor.t.cpp)
  target_link_libraries(compressor_test GTest::gtest_main)

  gtest_discover_tests(compressor_test)
endif()