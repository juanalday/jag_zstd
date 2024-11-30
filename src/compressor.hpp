#ifndef JAG_ZSTDPP_COMPRESSOR_HPP
#define JAG_ZSTDPP_COMPRESSOR_HPP

#include "zstd.h"
#include <zstd_errors.h>

#include <format>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <ranges>
#include <vector>

namespace jag::compressor::zstd {

	class Compression {
		using cctx_pointer_type = std::add_pointer_t<ZSTD_CCtx>;
		using dctx_pointer_type = std::add_pointer_t<ZSTD_DCtx>;
		constexpr static int s_defaultCompressionLevel = 1;

	public:
		static constexpr std::string_view zstdVersion() { return ZSTD_VERSION_STRING; }

		Compression& setParameter(ZSTD_cParameter param, int value) {
			if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx(), param, value))) [[unlikely]] {
				throw std::runtime_error{ "ZSTD_CCtx_setParameter() failed" };
			}
			return *this;
		}

		Compression& setCompressionLevel(int level) { return setParameter(ZSTD_c_compressionLevel, level); }

		int compressionLevel() const {
			int currentCompressionLevel;
			if (ZSTD_isError(ZSTD_CCtx_getParameter(m_cctx.get(), ZSTD_c_compressionLevel, &currentCompressionLevel))) {
				throw std::runtime_error{ "ZSTD_CCtx_getParameter() failed" };
			}
			return currentCompressionLevel;
		}
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
		size_t compress(const void* src, size_t srcSize, void* dst, size_t dstSize) {
			if (dstSize < ZSTD_compressBound(srcSize)) [[unlikely]] {
				throw std::runtime_error{ "dst buffer is too small" };
			}
			auto const compressedSize = ZSTD_compress2(cctx(), dst, dstSize, src, srcSize);
			if (ZSTD_isError(compressedSize)) {
				throw std::runtime_error{ "ZSTD_compressCCtx() failed" };
			}
			return compressedSize;
		}
	protected:
		cctx_pointer_type cctx() {
			if (!m_cctx) [[unlikely]] {
				m_cctx.reset(ZSTD_createCCtx());
				if (!m_cctx) [[unlikely]] {
					throw std::runtime_error{ "ZSTD_createCCtx() failed" };
				}
				setParameter(ZSTD_c_compressionLevel, s_defaultCompressionLevel);
			}
			return m_cctx.get();
		}
		dctx_pointer_type dctx() {
			if (!m_dctx) [[unlikely]] {
				m_dctx.reset(ZSTD_createDCtx());
				if (!m_dctx) [[unlikely]] {
					throw std::runtime_error{ "ZSTD_createDCtx() failed" };
				}
			}
			return m_dctx.get();
		}


	private:
		std::unique_ptr<ZSTD_CCtx, decltype(&ZSTD_freeCCtx)> m_cctx = { nullptr, ZSTD_freeCCtx };
		std::unique_ptr<ZSTD_DCtx, decltype(&ZSTD_freeDCtx)> m_dctx = { nullptr, ZSTD_freeDCtx };

	};
	class Compressor : public Compression {
	public:


		
	};

	class Decompressor : public Compression {
	public:

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
		size_t decompress(const void* src, size_t srcSize, void* dst, size_t dstSize) {
			size_t const cBuffSize = ZSTD_getFrameContentSize(src, srcSize);
			if (cBuffSize == ZSTD_CONTENTSIZE_ERROR) {
				throw std::runtime_error{ "ZSTD_getFrameContentSize() failed" };
			}
			if (cBuffSize > dstSize) {
				throw std::runtime_error{ "dst buffer is too small" };
			}

			auto const decompressedSize = ZSTD_decompressDCtx(dctx(), dst, dstSize, src, srcSize);
			if (ZSTD_isError(decompressedSize)) {
				throw std::runtime_error{ "ZSTD_decompressDCtx() failed" };
			}
			return decompressedSize;
		}
	};

} // end of namespace jag::compressor::zstd

#endif // JAG_ZSTDPP_COMPRESSOR_HPP