#ifndef JAG_ZSTDPP_COMPRESSOR_HPP
#define JAG_ZSTDPP_COMPRESSOR_HPP

#include "zstd.h"
#include <zstd_errors.h>

#include <memory>
#include <stdexcept>
#include <string_view>
#include <ranges>
#include <vector>

namespace jag::compressor::zstd {

	class Compression {
		using cctx_type = ZSTD_CCtx;
		using dctx_type = ZSTD_DCtx;
		constexpr static int s_defaultCompressionLevel = 1;

	public:
		static constexpr std::string_view zstdVersion() { return ZSTD_VERSION_STRING; }

		Compression();

		/**
		* @brief Set a parameter for the compression context.
		*
		* @param param The parameter to set.
		* @param value The value to set the parameter to.
		* @return *this
		* @throws std::runtime_error if ZSTD_CCtx_setParameter() fails.
		*/
		Compression& setParameter(ZSTD_cParameter param, int value);

		/**
		* @brief Set the compression level.
		*
		* @param level The compression level to set.
		* @return *this
		* @throws std::runtime_error if the operation fails.
		*/
		Compression& setCompressionLevel(int level);

		int compressionLevel() const;

		template <typename T = char, std::ranges::contiguous_range R1>
		std::vector<T> compress(const R1& src) {
			auto const srcSize = std::ranges::size(src) * sizeof(std::ranges::range_value_t<R1>);
			return compress<T>(std::ranges::data(src), srcSize);
		}

		template <std::ranges::contiguous_range R1, std::ranges::contiguous_range R2>
		size_t compress(const R1& src, R2& dst) {
			auto const srcSize = std::ranges::size(src) * sizeof(std::ranges::range_value_t<R1>);
			auto const dstSize = std::ranges::size(dst) * sizeof(std::ranges::range_value_t<R2>);

			return compress(std::ranges::data(src), srcSize, std::ranges::data(dst), dstSize);
		}

		template<typename T = char>
		std::vector<T> compress(const void* src, size_t srcSize) {
			std::vector<T> dst(ZSTD_compressBound(srcSize) / sizeof(T));
			size_t compressedSize = compress(src, srcSize, dst.data(), dst.size());
			dst.resize(compressedSize / sizeof(T));
			dst.shrink_to_fit();
			return dst;
		}
		size_t compress(const void* src, size_t srcSize, void* dst, size_t dstSize);

		template <typename T = char, std::ranges::contiguous_range R1>
		std::vector<T> decompress(const R1& src) {
			auto const srcSize = std::ranges::size(src) * sizeof(std::ranges::range_value_t<R1>);
			return decompress<T>(std::ranges::data(src), srcSize);
		}

		template <typename T = char>
		std::vector<T> decompress(const void* src, size_t srcSize) {
			size_t const cBuffSize = ZSTD_getFrameContentSize(src, srcSize);
			std::vector<T> dst(cBuffSize / sizeof(T));
			decompress(src, srcSize, dst.data(), cBuffSize);

			return dst;
		}

		template <std::ranges::contiguous_range R1, std::ranges::contiguous_range R2>
		size_t decompress(const R1& src, R2& dst) {
			auto const srcSize = std::ranges::size(src) * sizeof(std::ranges::range_value_t<R1>);
			auto const dstSize = std::ranges::size(dst) * sizeof(std::ranges::range_value_t<R2>);
			auto totalDecompresed = decompress(std::ranges::data(src), srcSize, std::ranges::data(dst), dstSize);
			return totalDecompresed / sizeof(std::ranges::range_value_t<R2>);
		}

		size_t decompress(const void* src, size_t srcSize, void* dst, size_t dstSize);
	private:
		/**
		 * @brief Get the compression context.
		 *
		 * @return a pointer to the compression context.
		 */
		std::add_pointer_t<cctx_type> cctx();

		/**
		 * @brief Get the decompression context.
		 *
		 * @return a pointer to the decompression context.
		 */
		std::add_pointer_t<dctx_type> dctx();

	private:
		std::unique_ptr<ZSTD_CCtx_params, decltype(&ZSTD_freeCCtxParams)> m_params = { ZSTD_createCCtxParams(), ZSTD_freeCCtxParams };
		std::unique_ptr<ZSTD_CCtx, decltype(&ZSTD_freeCCtx)> m_cctx = { nullptr, ZSTD_freeCCtx };
		std::unique_ptr<ZSTD_DCtx, decltype(&ZSTD_freeDCtx)> m_dctx = { nullptr, ZSTD_freeDCtx };

	};


} // end of namespace jag::compressor::zstd

#endif // JAG_ZSTDPP_COMPRESSOR_HPP