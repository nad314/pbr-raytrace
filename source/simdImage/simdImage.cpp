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

	void simdImage::preconvolveByAngle(const float& angle) {
		simdImage img = *this;
		const int xr = width/4;
		const int yr = height/2;

		const int step = 1;

		const float ca = cos(angle);
		int cc = std::thread::hardware_concurrency();
		core::Debug::info("Convolving %dx%d image using %d threads...", width, height, cc);
		core::Timer<float> timer;
		timer.start();
		std::thread *t = new std::thread[cc];
		for (int i = 0;i<cc;++i)
			t[i] = std::thread([&](int ct){
				for (int i=ct;i<width;i+=cc)
					for(int j=0;j<height;++j) {
						vec4s v = vec4s(0.0f);
						const float ip = ((float)i/width)*2.0f*M_PI;
						const float jp = ((float)j/height - 0.5f)*M_PI;
						const vec4s dir = vec4s(cos(ip), sin(ip), sin(jp), 1.0f).normalized3d();
						int a = 0;
						for (int x = 0; x<width; x+=step)
							for(int y = 0; y<height; y+=step) {
								const float ip = ((float)x/width)*2.0f*M_PI;
								const float jp = ((float)y/height - 0.5f)*M_PI;
								const vec4s dir2 = vec4s(cos(ip), sin(ip), sin(jp), 1.0f).normalized3d();
								
								//printf("%f\n", dir.dot3(dir2)[0]);

								const float dot = dir.dot3(dir2)[0];
								if (dot < ca)
									continue;
								v += img.at_c((x+width)%width, (y+height)%height) * dot; 
								++a;
							}
						at(i, j) = (v*M_PI/a).w1();
					}
			}, i);
		
		for (int i = 0;i<cc;++i)
			t[i].join();

		core::Debug::info("finished in %dms", (int)timer.stop().ms());
		delete[] t;
	}

	void simdImage::gauss3() {
		const simdImage img = *this;
		int cc = std::thread::hardware_concurrency();
		core::Debug::info("Gauss3 %dx%d image using %d threads...", width, height, cc);
		core::Timer<float> timer;
		timer.start();
		std::thread *t = new std::thread[cc];
		for (int i = 0;i<cc;++i)
			t[i] = std::thread([&](int ct){
				float kernel[3][3] = {{16.0f, 8.0f, 16.0f}, {8.0f, 4.0f, 8.0f}, {16.0f, 8.0f, 16.0f}};
				for (int x = 0; x<3; ++x)
					for(int y = 0; y<3; ++y)
						kernel[x][y] = 1.0f/kernel[x][y];
				
				for (int i=ct;i<width;i+=cc)
					for(int j=0;j<height;++j) {
						vec4s v(0.0f);
						for (int x = -1; x<2;++x)
							for(int y = -1; y<2;)
								v += img.at_c((i + x + width)%width, (j + y + height)%height)*kernel[x+1][++y];
						at(i, j) = v.w1();
					}
			}, i);
		
		for (int i = 0;i<cc;++i)
			t[i].join();

		core::Debug::info("finished in %dms", (int)timer.stop().ms());
		delete[] t;		
	}


	simdImage& simdImage::makeMipmap(const simdImage& img) {
		if (img.width < 4 || img.height < 4)
			return *this;
		simdImage gauss = img;
		gauss.gauss3();

		make(gauss.width/2, gauss.height/2);
		for (int i = 0; i < gauss.width; i += 2)
			for(int j = 0; j < gauss.height; j += 2)
				at(i/2, j/2) = gauss.at_c(i, j);
		return *this;
	}
}