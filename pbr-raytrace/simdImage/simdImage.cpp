#include <main>
#include <memory>
#include <hdrloader/hdrloader.h>

namespace core {
	void simdImage::make(size_t w, size_t h) {
		dispose(); 
		data = new vec4s[(w+1)*(h+1)]; 
		width = w; 
		height = h;
	}

	void simdImage::clear(const vec4s& color) {
		for (size_t i = 0; i < width*height; ++i)
			data[i] = color;
	}

	bool simdImage::loadHDR(const char* path) {
		HDRLoaderResult result;
		bool ret = HDRLoader::load(path, result);
		if (ret) {
			dispose();
			make(result.width, result.height);
			for (int i = 0; i < width*height; ++i)
				data[i] = vec4s((vec4s(result.cols[i * 3], result.cols[i * 3 + 1], result.cols[i * 3 + 2], 1.0f))).w1();
			flipV();
		}
		return ret;
	}

	bool simdImage::loadPNG(const char* path) {
		Image img;
		if (!img.loadPng(path))
			return 0;
		dispose();
		make(img.width, img.height);
		for (int i = 0; i < width; ++i)
			for (int j = 0; j < height; ++j)
				at(i, j) = vec4s(img.at(i, j, 0), img.at(i, j, 1), img.at(i, j, 2), img.at(i, j, 3)) / 255.0f;
		return 1;
	}

	void simdImage::flipV() {
		for (size_t i = 0; i < width; ++i)
			for (size_t j = 0; j < height / 2; ++j)
				std::swap(at(i, j), at(i, height - j));
	}

	void simdImage::tonemap() {
		for (int i = 0; i < width*height; ++i) {
			data[i] /= data[i] + 1.0f;
			data[i].w1();
		}
	}

	void simdImage::gammaCompress(const float& gamma) {
		vec4 v;
		for (int i = 0; i < width*height; ++i) {
			data[i].store(v);
			v.x = pow(v.x, gamma);
			v.y = pow(v.y, gamma);
			v.z = pow(v.z, gamma);
			data[i] = v;
		}
	}

	void simdImage::preconvolveDiffuse() {
		
	}
}