#include "common.h"
#include "rg_functions_c.h"

template<typename pixel_t, CModeProcessor<pixel_t> processor>
static void process_plane_c(const uint8_t* pSrc8, uint8_t* pDst8, int width, int height, ptrdiff_t srcPitch, ptrdiff_t dstPitch) {
	vsh::bitblt(pDst8, dstPitch, pSrc8, srcPitch, width * sizeof(pixel_t), 1);

	pixel_t* pDst = reinterpret_cast<pixel_t*>(pDst8);
	const pixel_t* pSrc = reinterpret_cast<const pixel_t*>(pSrc8);

	dstPitch /= sizeof(pixel_t);
	const ptrdiff_t srcPitchOrig = srcPitch;
	srcPitch /= sizeof(pixel_t);

	pSrc += srcPitch;
	pDst += dstPitch;
	for (int y = 1; y < height - 1; ++y) {
		pDst[0] = pSrc[0];
		for (int x = 1; x < width - 1; x += 1) {
			pixel_t result = processor((uint8_t*)(pSrc + x), srcPitchOrig);
			pDst[x] = result;
		}
		pDst[width - 1] = pSrc[width - 1];

		pSrc += srcPitch;
		pDst += dstPitch;
	}

	vsh::bitblt((uint8_t*)pDst, dstPitch * sizeof(pixel_t), (uint8_t*)pSrc, srcPitch * sizeof(pixel_t), width * sizeof(pixel_t), 1);
}

template<typename pixel_t, CModeProcessor<pixel_t> processor>
static void process_halfplane_c(const uint8_t* pSrc8, uint8_t* pDst8, int width, int height, ptrdiff_t srcPitch, ptrdiff_t dstPitch) {
	pixel_t* pDst = reinterpret_cast<pixel_t*>(pDst8);
	const pixel_t* pSrc = reinterpret_cast<const pixel_t*>(pSrc8);

	dstPitch /= sizeof(pixel_t);
	const ptrdiff_t srcPitchOrig = srcPitch;
	srcPitch /= sizeof(pixel_t);

	pSrc += srcPitch;
	pDst += dstPitch;
	for (int y = 1; y < height / 2; ++y) {
		pDst[0] = (pSrc[srcPitch] + pSrc[-srcPitch] + (sizeof(pixel_t) == 4 ? 0 : 1)) / 2; // float: no round
		for (int x = 1; x < width - 1; x += 1) {
			pixel_t result = processor((uint8_t*)(pSrc + x), srcPitchOrig);
			pDst[x] = result;
		}
		pDst[width - 1] = (pSrc[width - 1 + srcPitch] + pSrc[width - 1 - srcPitch] + (sizeof(pixel_t) == 4 ? 0 : 1)) / 2; // float: no +1 rounding
		pSrc += srcPitch;
		pDst += dstPitch;

		vsh::bitblt((uint8_t*)pDst, dstPitch * sizeof(pixel_t), (uint8_t*)pSrc, srcPitch * sizeof(pixel_t), width * sizeof(pixel_t), 1); //other field

		pSrc += srcPitch;
		pDst += dstPitch;
	}
}

template<typename pixel_t, CModeProcessor<pixel_t> processor>
static void process_even_rows_c(const uint8_t* pSrc, uint8_t* pDst, int width, int height, ptrdiff_t srcPitch, ptrdiff_t dstPitch) {
	vsh::bitblt(pDst, dstPitch, pSrc, srcPitch, width * sizeof(pixel_t), 2); //copy first two lines

	process_halfplane_c<pixel_t, processor>(pSrc + srcPitch, pDst + dstPitch, width, height, srcPitch, dstPitch);
}

template<typename pixel_t, CModeProcessor<pixel_t> processor>
static void process_odd_rows_c(const uint8_t* pSrc, uint8_t* pDst, int width, int height, ptrdiff_t srcPitch, ptrdiff_t dstPitch) {
	vsh::bitblt(pDst, dstPitch, pSrc, srcPitch, width * sizeof(pixel_t), 1); //top border

	process_halfplane_c<pixel_t, processor>(pSrc, pDst, width, height, srcPitch, dstPitch);

	vsh::bitblt(pDst + dstPitch * (height - 1), dstPitch, pSrc + srcPitch * (height - 1), srcPitch, width * sizeof(pixel_t), 1); //bottom border
}

template<typename pixel_t>
static void copyPlane(const uint8_t* pSrc, uint8_t* pDst, int width, int height, ptrdiff_t srcPitch, ptrdiff_t dstPitch) {
	vsh::bitblt(pDst, dstPitch, pSrc, srcPitch, width * sizeof(pixel_t), height);
}

static PlaneProcessor* c_functions[] = {
	copyPlane<uint8_t>,
	process_plane_c<uint8_t, rg_mode1_cpp>,
	process_plane_c<uint8_t, rg_mode2_cpp>,
	process_plane_c<uint8_t, rg_mode3_cpp>,
	process_plane_c<uint8_t, rg_mode4_cpp>,
	process_plane_c<uint8_t, rg_mode5_cpp>,
	process_plane_c<uint8_t, rg_mode6_cpp>,
	process_plane_c<uint8_t, rg_mode7_cpp>,
	process_plane_c<uint8_t, rg_mode8_cpp>,
	process_plane_c<uint8_t, rg_mode9_cpp>,
	process_plane_c<uint8_t, rg_mode10_cpp>,
	process_plane_c<uint8_t, rg_mode11_cpp>,
	process_plane_c<uint8_t, rg_mode12_cpp>,
	process_even_rows_c<uint8_t, rg_mode13_and14_cpp>,
	process_odd_rows_c<uint8_t, rg_mode13_and14_cpp>,
	process_even_rows_c<uint8_t, rg_mode15_and16_cpp>,
	process_odd_rows_c<uint8_t, rg_mode15_and16_cpp>,
	process_plane_c<uint8_t, rg_mode17_cpp>,
	process_plane_c<uint8_t, rg_mode18_cpp>,
	process_plane_c<uint8_t, rg_mode19_cpp>,
	process_plane_c<uint8_t, rg_mode20_cpp>,
	process_plane_c<uint8_t, rg_mode21_cpp>,
	process_plane_c<uint8_t, rg_mode22_cpp>,
	process_plane_c<uint8_t, rg_mode23_cpp>,
	process_plane_c<uint8_t, rg_mode24_cpp>,
	process_plane_c<uint8_t, rg_mode25_cpp>,
	process_plane_c<uint8_t, rg_mode26_cpp>,
	process_plane_c<uint8_t, rg_mode27_cpp>,
	process_plane_c<uint8_t, rg_mode28_cpp>,
};

static PlaneProcessor* c_functions_10[] = {
	copyPlane<uint16_t>,
	process_plane_c<uint16_t, rg_mode1_cpp_16>,
	process_plane_c<uint16_t, rg_mode2_cpp_16>,
	process_plane_c<uint16_t, rg_mode3_cpp_16>,
	process_plane_c<uint16_t, rg_mode4_cpp_16>,
	process_plane_c<uint16_t, rg_mode5_cpp_16>,
	process_plane_c<uint16_t, rg_mode6_cpp_16<10>>,
	process_plane_c<uint16_t, rg_mode7_cpp_16>,
	process_plane_c<uint16_t, rg_mode8_cpp_16<10>>,
	process_plane_c<uint16_t, rg_mode9_cpp_16>,
	process_plane_c<uint16_t, rg_mode10_cpp_16>,
	process_plane_c<uint16_t, rg_mode11_cpp_16>,
	process_plane_c<uint16_t, rg_mode12_cpp_16>,
	process_even_rows_c<uint16_t, rg_mode13_and14_cpp_16>,
	process_odd_rows_c<uint16_t, rg_mode13_and14_cpp_16>,
	process_even_rows_c<uint16_t, rg_mode15_and16_cpp_16>,
	process_odd_rows_c<uint16_t, rg_mode15_and16_cpp_16>,
	process_plane_c<uint16_t, rg_mode17_cpp_16>,
	process_plane_c<uint16_t, rg_mode18_cpp_16>,
	process_plane_c<uint16_t, rg_mode19_cpp_16>,
	process_plane_c<uint16_t, rg_mode20_cpp_16>,
	process_plane_c<uint16_t, rg_mode21_cpp_16>,
	process_plane_c<uint16_t, rg_mode22_cpp_16>,
	process_plane_c<uint16_t, rg_mode23_cpp_16<10>>,
	process_plane_c<uint16_t, rg_mode24_cpp_16<10>>,
	process_plane_c<uint16_t, rg_mode25_cpp_16<10>>,
	process_plane_c<uint16_t, rg_mode26_cpp_16>,
	process_plane_c<uint16_t, rg_mode27_cpp_16>,
	process_plane_c<uint16_t, rg_mode28_cpp_16>,
};

static PlaneProcessor* c_functions_12[] = {
	copyPlane<uint16_t>,
	process_plane_c<uint16_t, rg_mode1_cpp_16>,
	process_plane_c<uint16_t, rg_mode2_cpp_16>,
	process_plane_c<uint16_t, rg_mode3_cpp_16>,
	process_plane_c<uint16_t, rg_mode4_cpp_16>,
	process_plane_c<uint16_t, rg_mode5_cpp_16>,
	process_plane_c<uint16_t, rg_mode6_cpp_16<12>>,
	process_plane_c<uint16_t, rg_mode7_cpp_16>,
	process_plane_c<uint16_t, rg_mode8_cpp_16<12>>,
	process_plane_c<uint16_t, rg_mode9_cpp_16>,
	process_plane_c<uint16_t, rg_mode10_cpp_16>,
	process_plane_c<uint16_t, rg_mode11_cpp_16>,
	process_plane_c<uint16_t, rg_mode12_cpp_16>,
	process_even_rows_c<uint16_t, rg_mode13_and14_cpp_16>,
	process_odd_rows_c<uint16_t, rg_mode13_and14_cpp_16>,
	process_even_rows_c<uint16_t, rg_mode15_and16_cpp_16>,
	process_odd_rows_c<uint16_t, rg_mode15_and16_cpp_16>,
	process_plane_c<uint16_t, rg_mode17_cpp_16>,
	process_plane_c<uint16_t, rg_mode18_cpp_16>,
	process_plane_c<uint16_t, rg_mode19_cpp_16>,
	process_plane_c<uint16_t, rg_mode20_cpp_16>,
	process_plane_c<uint16_t, rg_mode21_cpp_16>,
	process_plane_c<uint16_t, rg_mode22_cpp_16>,
	process_plane_c<uint16_t, rg_mode23_cpp_16<12>>,
	process_plane_c<uint16_t, rg_mode24_cpp_16<12>>,
	process_plane_c<uint16_t, rg_mode25_cpp_16<12>>,
	process_plane_c<uint16_t, rg_mode26_cpp_16>,
	process_plane_c<uint16_t, rg_mode27_cpp_16>,
	process_plane_c<uint16_t, rg_mode28_cpp_16>,
};

static PlaneProcessor* c_functions_14[] = {
	copyPlane<uint16_t>,
	process_plane_c<uint16_t, rg_mode1_cpp_16>,
	process_plane_c<uint16_t, rg_mode2_cpp_16>,
	process_plane_c<uint16_t, rg_mode3_cpp_16>,
	process_plane_c<uint16_t, rg_mode4_cpp_16>,
	process_plane_c<uint16_t, rg_mode5_cpp_16>,
	process_plane_c<uint16_t, rg_mode6_cpp_16<14>>,
	process_plane_c<uint16_t, rg_mode7_cpp_16>,
	process_plane_c<uint16_t, rg_mode8_cpp_16<14>>,
	process_plane_c<uint16_t, rg_mode9_cpp_16>,
	process_plane_c<uint16_t, rg_mode10_cpp_16>,
	process_plane_c<uint16_t, rg_mode11_cpp_16>,
	process_plane_c<uint16_t, rg_mode12_cpp_16>,
	process_even_rows_c<uint16_t, rg_mode13_and14_cpp_16>,
	process_odd_rows_c<uint16_t, rg_mode13_and14_cpp_16>,
	process_even_rows_c<uint16_t, rg_mode15_and16_cpp_16>,
	process_odd_rows_c<uint16_t, rg_mode15_and16_cpp_16>,
	process_plane_c<uint16_t, rg_mode17_cpp_16>,
	process_plane_c<uint16_t, rg_mode18_cpp_16>,
	process_plane_c<uint16_t, rg_mode19_cpp_16>,
	process_plane_c<uint16_t, rg_mode20_cpp_16>,
	process_plane_c<uint16_t, rg_mode21_cpp_16>,
	process_plane_c<uint16_t, rg_mode22_cpp_16>,
	process_plane_c<uint16_t, rg_mode23_cpp_16<14>>,
	process_plane_c<uint16_t, rg_mode24_cpp_16<14>>,
	process_plane_c<uint16_t, rg_mode25_cpp_16<14>>,
	process_plane_c<uint16_t, rg_mode26_cpp_16>,
	process_plane_c<uint16_t, rg_mode27_cpp_16>,
	process_plane_c<uint16_t, rg_mode28_cpp_16>,
};

static PlaneProcessor* c_functions_16[] = {
	copyPlane<uint16_t>,
	process_plane_c<uint16_t, rg_mode1_cpp_16>,
	process_plane_c<uint16_t, rg_mode2_cpp_16>,
	process_plane_c<uint16_t, rg_mode3_cpp_16>,
	process_plane_c<uint16_t, rg_mode4_cpp_16>,
	process_plane_c<uint16_t, rg_mode5_cpp_16>,
	process_plane_c<uint16_t, rg_mode6_cpp_16<16>>,
	process_plane_c<uint16_t, rg_mode7_cpp_16>,
	process_plane_c<uint16_t, rg_mode8_cpp_16<16>>,
	process_plane_c<uint16_t, rg_mode9_cpp_16>,
	process_plane_c<uint16_t, rg_mode10_cpp_16>,
	process_plane_c<uint16_t, rg_mode11_cpp_16>,
	process_plane_c<uint16_t, rg_mode12_cpp_16>,
	process_even_rows_c<uint16_t, rg_mode13_and14_cpp_16>,
	process_odd_rows_c<uint16_t, rg_mode13_and14_cpp_16>,
	process_even_rows_c<uint16_t, rg_mode15_and16_cpp_16>,
	process_odd_rows_c<uint16_t, rg_mode15_and16_cpp_16>,
	process_plane_c<uint16_t, rg_mode17_cpp_16>,
	process_plane_c<uint16_t, rg_mode18_cpp_16>,
	process_plane_c<uint16_t, rg_mode19_cpp_16>,
	process_plane_c<uint16_t, rg_mode20_cpp_16>,
	process_plane_c<uint16_t, rg_mode21_cpp_16>,
	process_plane_c<uint16_t, rg_mode22_cpp_16>,
	process_plane_c<uint16_t, rg_mode23_cpp_16<16>>,
	process_plane_c<uint16_t, rg_mode24_cpp_16<16>>,
	process_plane_c<uint16_t, rg_mode25_cpp_16<16>>,
	process_plane_c<uint16_t, rg_mode26_cpp_16>,
	process_plane_c<uint16_t, rg_mode27_cpp_16>,
	process_plane_c<uint16_t, rg_mode28_cpp_16>,
};

static PlaneProcessor* c_functions_32_luma[] = {
	copyPlane<float>,
	process_plane_c<float, rg_mode1_cpp_32>,
	process_plane_c<float, rg_mode2_cpp_32>,
	process_plane_c<float, rg_mode3_cpp_32>,
	process_plane_c<float, rg_mode4_cpp_32>,
	process_plane_c<float, rg_mode5_cpp_32>,
	process_plane_c<float, rg_mode6_cpp_32>,
	process_plane_c<float, rg_mode7_cpp_32>,
	process_plane_c<float, rg_mode8_cpp_32>,
	process_plane_c<float, rg_mode9_cpp_32>,
	process_plane_c<float, rg_mode10_cpp_32>,
	process_plane_c<float, rg_mode11_cpp_32>,
	process_plane_c<float, rg_mode12_cpp_32>,
	process_even_rows_c<float, rg_mode13_and14_cpp_32>,
	process_odd_rows_c<float, rg_mode13_and14_cpp_32>,
	process_even_rows_c<float, rg_mode15_and16_cpp_32>,
	process_odd_rows_c<float, rg_mode15_and16_cpp_32>,
	process_plane_c<float, rg_mode17_cpp_32>,
	process_plane_c<float, rg_mode18_cpp_32>,
	process_plane_c<float, rg_mode19_cpp_32>,
	process_plane_c<float, rg_mode20_cpp_32>,
	process_plane_c<float, rg_mode21_cpp_32>,
	process_plane_c<float, rg_mode22_cpp_32>,
	process_plane_c<float, rg_mode23_cpp_32<false>>,
	process_plane_c<float, rg_mode24_cpp_32<false>>,
	process_plane_c<float, rg_mode25_cpp_32<false>>, // false: luma, true: chroma
	process_plane_c<float, rg_mode26_cpp_32>,
	process_plane_c<float, rg_mode27_cpp_32>,
	process_plane_c<float, rg_mode28_cpp_32>,
};

static PlaneProcessor* c_functions_32_chroma[] = {
	copyPlane<float>,
	process_plane_c<float, rg_mode1_cpp_32>,
	process_plane_c<float, rg_mode2_cpp_32>,
	process_plane_c<float, rg_mode3_cpp_32>,
	process_plane_c<float, rg_mode4_cpp_32>,
	process_plane_c<float, rg_mode5_cpp_32>,
	process_plane_c<float, rg_mode6_cpp_32>,
	process_plane_c<float, rg_mode7_cpp_32>,
	process_plane_c<float, rg_mode8_cpp_32>,
	process_plane_c<float, rg_mode9_cpp_32>,
	process_plane_c<float, rg_mode10_cpp_32>,
	process_plane_c<float, rg_mode11_cpp_32>,
	process_plane_c<float, rg_mode12_cpp_32>,
	process_even_rows_c<float, rg_mode13_and14_cpp_32>,
	process_odd_rows_c<float, rg_mode13_and14_cpp_32>,
	process_even_rows_c<float, rg_mode15_and16_cpp_32>,
	process_odd_rows_c<float, rg_mode15_and16_cpp_32>,
	process_plane_c<float, rg_mode17_cpp_32>,
	process_plane_c<float, rg_mode18_cpp_32>,
	process_plane_c<float, rg_mode19_cpp_32>,
	process_plane_c<float, rg_mode20_cpp_32>,
	process_plane_c<float, rg_mode21_cpp_32>,
	process_plane_c<float, rg_mode22_cpp_32>,
	process_plane_c<float, rg_mode23_cpp_32<true>>,
	process_plane_c<float, rg_mode24_cpp_32<true>>,
	process_plane_c<float, rg_mode25_cpp_32<true>>, // false: luma, true: chroma
	process_plane_c<float, rg_mode26_cpp_32>,
	process_plane_c<float, rg_mode27_cpp_32>,
	process_plane_c<float, rg_mode28_cpp_32>,
};


static const VSFrame* VS_CC rgToolsGetFrame(int n, int activationReason, void* instanceData, void** frameData, VSFrameContext* frameCtx, VSCore* core, const VSAPI* vsapi) {
	auto d{ static_cast<RgToolsData*>(instanceData) };

	if (activationReason == arInitial) {
		vsapi->requestFrameFilter(n, d->node, frameCtx);
	}
	else if (activationReason == arAllFramesReady) {
		const VSFrame* src = vsapi->getFrameFilter(n, d->node, frameCtx);
		const VSVideoFormat* fi = vsapi->getVideoFrameFormat(src);
		int srch = vsapi->getFrameHeight(src, 0);
		int srcw = vsapi->getFrameWidth(src, 0);
		VSFrame* dst = vsapi->newVideoFrame(fi, srcw, srch, src, core);

		for (int plane{ 0 }; plane < d->vi->format.numPlanes; plane++) {
			const uint8_t* srcp = vsapi->getReadPtr(src, plane);
			const ptrdiff_t src_pitch = vsapi->getStride(src, plane);
			uint8_t* dstp = vsapi->getWritePtr(dst, plane);
			ptrdiff_t dst_pitch = vsapi->getStride(dst, plane);
			const int width{ vsapi->getFrameWidth(src, plane) };
			const int height{ vsapi->getFrameHeight(src, plane) };

			if (plane &&
				d->vi->format.colorFamily != cfRGB &&
				d->vi->format.sampleType == stFloat) {
				d->functions_chroma[d->mode](srcp, dstp, width, height, src_pitch, dst_pitch);
			}
			else {
				d->functions[d->mode](srcp, dstp, width, height, src_pitch, dst_pitch);
			}
		}

		vsapi->freeFrame(src);
		return dst;
	}
	return nullptr;
}

static void VS_CC rgToolsFree(void* instanceData, VSCore* core, const VSAPI* vsapi) {
	auto d{ static_cast<RgToolsData*>(instanceData) };
	vsapi->freeNode(d->node);
	delete d;
}

void VS_CC rgToolsCreate(const VSMap* in, VSMap* out, void* userData, VSCore* core, const VSAPI* vsapi) {
	auto d{ std::make_unique<RgToolsData>() };
	int err = 0;

	d->node = vsapi->mapGetNode(in, "clip", 0, nullptr);
	d->vi = vsapi->getVideoInfo(d->node);

	d->mode = vsapi->mapGetIntSaturated(in, "mode", 0, &err);
	if (err)
		d->mode = 3;

	int bits_per_pixel = d->vi->format.bitsPerSample;
	int pixelsize = d->vi->format.bytesPerSample;

	if (pixelsize == 1) {
		d->functions = c_functions;
	}
	else if (pixelsize == 2) {
		switch (bits_per_pixel) {
		case 10: d->functions = c_functions_10; break;
		case 12: d->functions = c_functions_12; break;
		case 14: d->functions = c_functions_14; break;
		case 16: d->functions = c_functions_16; break;
		}
	}
	else {
		d->functions = c_functions_32_luma;
		d->functions_chroma = c_functions_32_chroma;
	}


	VSFilterDependency deps[] = { {d->node, rpStrictSpatial} };
	vsapi->createVideoFilter(out, "RemoveGrain", d->vi, rgToolsGetFrame, rgToolsFree, fmParallel, deps, 1, d.get(), core);
	d.release();
}