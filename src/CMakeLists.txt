add_library(zstdpp STATIC compressor.hpp compressor.cpp)
target_link_libraries(zstdpp PUBLIC zstd::zstd)
target_include_directories(zstdpp PUBLIC ${zstd_SOURCE_DIR}/lib)
target_compile_definitions(zstdpp PUBLIC ZSTD_STATIC_LINKING_ONLY=1)


if(ENABLE_TEST)
  enable_testing()
  include(GoogleTest)
  add_executable(compressor_test compressor.t.cpp)
  target_link_libraries(compressor_test PUBLIC zstdpp GTest::gtest_main GTest::gmock_main)
  #target_include_directories(compressor_test PUBLIC ${zstd_SOURCE_DIR}/lib)
  gtest_discover_tests(compressor_test)
endif()