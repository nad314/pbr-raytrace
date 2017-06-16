#include <main>
namespace core {
#ifndef __WIN
template<>
#endif
	int imageRenderTask::squareSize = 16;
	vec2i imageRenderTask::squares;
	vec2i imageRenderTask::current;

	bool imageRenderTask::getNextRect(vec4i& rect) {
		static std::mutex mutex;
		std::unique_lock<std::mutex> lk(mutex);
		const vec2i c = current;
		++current.x;
		if (current.x > squares.x) {
			++current.y;
			current.x = 0;
		}
		if (current.y > squares.y)
			return 0;
		//lk.unlock();

		rect.x = std::min(c.x * squareSize, (int)pview->img.width);
		rect.y = std::min(c.y * squareSize, (int)pview->img.height);
		rect.z = std::min((c.x + 1) * squareSize, (int)pview->img.width);
		rect.w = std::min((c.y + 1) * squareSize, (int)pview->img.height);
		return 1;
	}

	void imageRenderTask::execute(Worker* pWorker) {
		if (pWorker == NULL)
			return;

		Worker& worker = *pWorker;
		PBVH& bvh = *pbvh;
		simdView &view = *pview;
		Image &img = Storage::get().renderImage;
		int* mp = reinterpret_cast<int*>(img.data);
		vec4 bp, bq; // bounding box projected coordinates
		Renderer::projectedBox(bvh, pview, bp, bq);


		const int w = img.width;
		const int h = img.height;

		matrixf inv = view.mat;
		inv.invert();
		matrixs sinv = inv;

		const int square = squareSize;
		vec4s lightPos = vec4s(inv*vec4(0.0f, 0.0f, -5.0f, 1.0f));

		Ray ray;
		std::pair<int, float> stack[256];
		int* priority = new int[bvh.inner.size()];
		memset(priority, 0, bvh.inner.size() * sizeof(int));

		Storage& data = Storage::get();
		//core::VolumetricShader& shader = data.volumetricShader;
		//core::RenderShader& shader = data.shader;
		const core::Renderer::PixelShader& shader = Controller::get().getShader();

		vec4s envScale = Settings::environmentStrength;
		const int ms = Settings::maxSamples;
		const float sms = sqrt((float)ms);
		const vec2 offset = vec2(0.0f, 0.0f);
		/*
		core::simdImage env;
		env = data.volumetricShader.hdri;
		core::simdImage frame;
		frame = data.simdFrame;*/
		core::simdImage& env = data.hdri;
		core::simdImage& frame = data.simdFrame;
		vec4i rect;
		const vec4s clearColor = vec4s(0.2f).w1();

		const matrixs sNormalMatrix = view.rotation.normalMatrix();
		//const matrixs sNormalMatrix;
		while (getNextRect(rect)) {
			const vec2i c = current;
			//repaint
			//task.onStartNode(pview, vec2i((int)rect.x / square, (int)rect.y / square), vec2i((int)std::ceil((float)(img.width) / square), (int)std::ceil((float)(img.height) / square)), square);
			for(int sample = 0; sample < ms; ++sample) {
				const vec2 offset = vec2(sample % (int)sms, sample / (int)sms) / sms;
				for (int i = rect.y; i < rect.w; ++i) {
					for (int j = rect.x; j < rect.z; ++j) {
						vec4s& simdFrag = frame.at(j, i);
						core::Renderer::unproject(ray, view, sinv, (float)j + offset.x, (float)h - i + offset.y);
						const float d = bvh.findFirst(ray, stack, priority, true);

						if (d > 0.0f)
							simdFrag += vec4s(shader.getColor(ray, d, ray.normal, ray.color, bvh, stack, priority)).w1();
						else
							//simdFrag += (envMap(env, sNormalMatrix*ray.sr1)*envScale).min(1.0f);
							simdFrag += clearColor;
						
						if (sample < ms - 1 /*&& (sample%256)!=0*/)
							continue;
							
								/*
						__m128i xmm0 = _mm_cvtsi32_si128(*(mp + j + i*w));
						xmm0 = _mm_unpacklo_epi8(xmm0, _mm_setzero_si128());
						xmm0 = _mm_unpacklo_epi16(xmm0, _mm_setzero_si128());
						vec4s xmm1 = _mm_cvtepi32_ps(xmm0);*/
						const vec4s frag = simdFrag * vec4s(255.0f) / vec4s(ms);
						//frag = frag*vec4s(255.0f)*_mm_permute_ps(frag, 0b11111111) + xmm1*(vec4s(1.0f) - _mm_permute_ps(frag, 0b11111111));
						__m128i fv = _mm_cvtps_epi32(frag);
						fv = _mm_packus_epi16(fv, fv);
						fv = _mm_packus_epi16(fv, fv);
						const int fragOut = _mm_cvtsi128_si32(fv);
						_mm_stream_si32(mp + j + i*w, fragOut);
						//memcpy(mp + j + i*w, &fragOut, sizeof(int));
					}
				}
			}
			//repaint
			if (worker.threadNumber == 0) {
				//printf("%.2f\n", (float)(c.x + c.y*squares.x)/(squares.x*squares.y));
				const float p = std::min(1.0f, (float)(c.x + c.y*squares.x)/(squares.x*squares.y));
				Statusbar::prog(p);
				
				//core::Surface& rw = MainWindow::get().getRenderWindow();
				//rw.onPaint(core::eventInfo(SDL_Event()));
				MainWindow::get().onPaint(core::eventInfo(SDL_Event()));
			}
			
		}
		delete[] priority;
	}
}
