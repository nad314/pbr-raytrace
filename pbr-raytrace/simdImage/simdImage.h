#pragma once

namespace core {
	struct simdImage: public SIMD {
		size_t width;
		size_t height;
		vec4s *data;

		simdImage() : data(NULL) { width = height = 0; }
		~simdImage() { dispose(); }
		
		inline void dispose() { delete[] data; data = NULL; width = height = 0; }
		void make(size_t w, size_t h);
		inline vec4s& operator[](const size_t& x) { return data[x]; }
		inline vec4s& at(const size_t& x, const size_t& y) { return data[y*width + x]; }
		inline vec4s at_c(const size_t& x, const size_t& y) const { return data[y*width + x]; }

		void clear(const vec4s& color);
		bool loadHDR(const char* path);
		bool loadPNG(const char* path);
		void flipV();
		void tonemap();
		void gammaCompress(const float& gamma);
	};
}

