#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "compressor.hpp"

#include "zstd.h"

#include <array>
#include <ranges>
#include <vector>

using namespace jag::compressor::zstd;
TEST(Compressor, plain) {

	std::vector<int> src{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::vector<char>          const expectedCompressed = {40, -75, 47, -3,  32, 40, 65, 1, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 6, 0, 0, 0, 7, 0, 0, 0, 8, 0, 0, 0, 9, 0, 0, 0, 10, 0, 0,0};
	std::vector<unsigned char> const unsignedCompressed = {40, 181, 47, 253, 32, 40, 65, 1, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 6, 0, 0, 0, 7, 0, 0, 0, 8, 0, 0, 0, 9, 0, 0, 0, 10, 0, 0,0 };

	std::vector<char> compressed = Compression{}.compress(src);
	EXPECT_EQ(compressed.size(), 49);
	EXPECT_EQ(compressed, expectedCompressed);
	EXPECT_EQ(Compression{}.compress<unsigned char>(src), unsignedCompressed);
}

TEST(Decompressor, plain) {

	std::vector<char>          const         compressed = { 40, -75, 47, -3,  32, 40, 65, 1, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 6, 0, 0, 0, 7, 0, 0, 0, 8, 0, 0, 0, 9, 0, 0, 0, 10, 0, 0,0 };
	std::vector<unsigned char> const unsignedCompressed = { 40, 181, 47, 253, 32, 40, 65, 1, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 6, 0, 0, 0, 7, 0, 0, 0, 8, 0, 0, 0, 9, 0, 0, 0, 10, 0, 0,0 };

	std::vector<int> const expectedDecompressed{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	std::vector<int> uncompressed = Compression{}.decompress<int>(compressed);
	EXPECT_EQ(uncompressed.size(), 10);
	EXPECT_EQ(uncompressed, expectedDecompressed);
}

TEST(Decompressor, plain_stream) {

	std::vector<char>          const         compressed = { 40, -75, 47, -3,  32, 40, 65, 1, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 6, 0, 0, 0, 7, 0, 0, 0, 8, 0, 0, 0, 9, 0, 0, 0, 10, 0, 0,0 };
	std::vector<unsigned char> const unsignedCompressed = { 40, 181, 47, 253, 32, 40, 65, 1, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 6, 0, 0, 0, 7, 0, 0, 0, 8, 0, 0, 0, 9, 0, 0, 0, 10, 0, 0,0 };

	std::vector<int> const expectedDecompressed{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	std::vector<int> uncompressed = Compression{}.decompress_stream<int>(compressed);
	EXPECT_EQ(uncompressed.size(), 10);
	EXPECT_EQ(uncompressed, expectedDecompressed);
}