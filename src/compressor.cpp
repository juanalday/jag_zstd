#include "compressor.hpp"

using namespace jag::compressor::zstd;

Compression::Compression() {
	if (ZSTD_isError(ZSTD_CCtxParams_init(m_params.get(), s_defaultCompressionLevel))) {
		throw std::runtime_error{ "ZSTD_CCtxParams_init() failed" };
	}
}
std::add_pointer_t<Compression::cctx_type> Compression::cctx() {
	if (!m_cctx) [[unlikely]] {
		m_cctx.reset(ZSTD_createCCtx());
		if (!m_cctx) [[unlikely]] {
			throw std::runtime_error{ "ZSTD_createCCtx() failed" };
		}
	}
	return m_cctx.get();
}

std::add_pointer_t<Compression::dctx_type> Compression::dctx() {
	if (!m_dctx) [[unlikely]] {
		m_dctx.reset(ZSTD_createDCtx());
		if (!m_dctx) [[unlikely]] {
			throw std::runtime_error{ "ZSTD_createDCtx() failed" };
		}
	}
	return m_dctx.get();
}

Compression& Compression::setParameter(ZSTD_cParameter param, int value) {
	if (ZSTD_isError(ZSTD_CCtxParams_setParameter(m_params.get(), param, value))) [[unlikely]] {
		throw std::runtime_error{ "ZSTD_CCtxParams_setParameter() failed" };
	}
	if (ZSTD_isError(ZSTD_CCtx_setParametersUsingCCtxParams(cctx(), m_params.get()))) [[unlikely]] {
		throw std::runtime_error{ "ZSTD_CCtx_setParametersUsingCCtxParams() failed" };
	}
	return *this;
}

Compression& Compression::setCompressionLevel(int level) { return setParameter(ZSTD_c_compressionLevel, level); }

int Compression::compressionLevel() const {
	int currentCompressionLevel;
	if (ZSTD_isError(ZSTD_CCtxParams_getParameter(m_params.get(), ZSTD_c_compressionLevel, &currentCompressionLevel))) {
		throw std::runtime_error{ "ZSTD_CCtx_getParameter() failed" };
	}
	return currentCompressionLevel;
}

size_t Compression::compress(const void* src, size_t srcSize, void* dst, size_t dstSize) {
	if (dstSize < ZSTD_compressBound(srcSize)) [[unlikely]] {
		throw std::runtime_error{ "dst buffer is too small" };
	}
	auto const compressedSize = ZSTD_compress2(cctx(), dst, dstSize, src, srcSize);
	if (ZSTD_isError(compressedSize)) {
		throw std::runtime_error{ "ZSTD_compressCCtx() failed" };
	}
	return compressedSize;
}

size_t Compression::decompress(const void* src, size_t srcSize, void* dst, size_t dstSize) {
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