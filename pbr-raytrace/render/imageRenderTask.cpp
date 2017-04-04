#include <main>
namespace core {
#ifndef __WIN
template<>
#endif
	renderShowTask* Getter<renderShowTask>::getter = NULL;

	renderShowTask::renderShowTask(simdView* view, int nthreads) {
		std::lock_guard<std::mutex> lk(mutex);
		set(*this);
		img = view->img;
		threadSquare = new vec2i[nthreads];
		nt = nthreads;
	}


	void renderShowTask::onStartNode(simdView* view, vec2i cStep, vec2i nStep, int square) {
		{
			std::lock_guard<std::mutex> lk(mutex);
			//Renderer::drawRect(vec4i(cStep.x*square, cStep.y*square, std::min((cStep.x + 1)*square, (int)img.width), std::min((cStep.y + 1)*square, (int)img.height)).shrink(1), vec4b(255, 255, 120, 255), Controller::get().view->img);
			/*
			SDL_Event e;
			e.type = SDL_WINDOWEVENT;
			e.window.windowID = 0;
			e.window.event = SDL_WINDOWEVENT_EXPOSED;
			Controller::get().parent->surfaceInvalidate();
			MainWindow::get().__invalidate();
			MainWindow::get().onPaint(e);*/
			//cv.notify_all();
		}
	}

	void renderShowTask::onEndNode(simdView* view, vec2i cStep, vec2i nStep, int square) {
		{
			std::lock_guard<std::mutex> lk(mutex);
			/*
			const int x = cStep.x*square;
			for (int j = cStep.y*square; j < std::min(cStep.y*square + square, (int)img.height); ++j)
					memcpy(reinterpret_cast<int*>(img.data) + j*img.width + x, reinterpret_cast<int*>(view->img.data) + j*img.width + x, sizeof(int)*square);
			cv.notify_all();
			*/
			//printf("end block %d %d\n", cStep.x, cStep.y);
			/*
			SDL_Event e;
			e.type = SDL_WINDOWEVENT;
			e.window.windowID = 0;
			e.window.event = SDL_WINDOWEVENT_EXPOSED;
			Controller::get().parent->surfaceInvalidate();
			MainWindow::get().__invalidate();
			MainWindow::get().onPaint(e);*/

		}
		
	}

	int imageRenderTask::squareSize = 12;
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
		lk.unlock();

		rect.x = std::min(c.x * squareSize, (int)pview->img.width);
		rect.y = std::min(c.y * squareSize, (int)pview->img.height);
		rect.z = std::min((c.x + 1) * squareSize, (int)pview->img.width);
		rect.w = std::min((c.y + 1) * squareSize, (int)pview->img.height);
		return 1;
	}

	void imageRenderTask::execute(Worker* pWorker) {
		if (pWorker == NULL)
			return;
		core::LocalTask& task = renderShowTask::get();
/*
		core::Renderer::quickTrace(*pbvh, pview, pWorker->threadNumber, pWorker->threadCount, samples, 32, &renderShowTask::get(), *shader);
		return;
*/
		Worker& worker = *pWorker;
		PBVH& bvh = *pbvh;
		simdView &view = *pview;
		Image &img = view.img;
		int* mp = reinterpret_cast<int*>(img.data);
		vec4 bp, bq; // bounding box projected coordinates
		Renderer::projectedBox(bvh, pview, bp, bq);

		/*
		if (worker.threadNumber == 0) {
			++Storage::get().renderedSamples;
			tc = 1;
		}
		else while (tc == 0)
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));*/

		const int w = img.width;
		const int h = img.height;

		matrixf inv = view.mat;
		inv.invert();
		matrixs sinv = inv;

		const int square = squareSize;
		vec4s lightPos = vec4s(inv*vec4(0.0f, 0.0f, -5.0f, 1.0f));

		Ray ray;
		PBVH::Ray oray;
		std::pair<int, float> stack[256];
		int* priority = new int[bvh.inner.size()];
		memset(priority, 0, bvh.inner.size() * sizeof(int));

		Storage& data = Storage::get();
		//core::VolumetricShader& shader = data.volumetricShader;
		//core::RenderShader& shader = data.shader;
		core::Renderer::PixelShader& shader = Controller::get().getShader();

		vec4s envScale = Settings::environmentStrength;
		const int ms = Settings::maxSamples;
		const float sms = sqrt((float)ms);
		const vec2 offset = vec2(0.0f, 0.0f);
		/*
		core::simdImage env;
		env = data.volumetricShader.hdri;
		core::simdImage frame;
		frame = data.simdFrame;*/
		core::simdImage& env = data.volumetricShader.hdri;
		core::simdImage& frame = data.simdFrame;
		vec4i rect;
		while (getNextRect(rect)) {
			//repaint
			//task.onStartNode(pview, vec2i((int)rect.x / square, (int)rect.y / square), vec2i((int)std::ceil((float)(img.width) / square), (int)std::ceil((float)(img.height) / square)), square);
			for(int sample = 0; sample < ms; ++sample) {
				const vec2 offset = vec2(sample % (int)sms, sample / (int)sms) / sms;
				for (int i = rect.y; i < rect.w; ++i) {
					for (int j = rect.x; j < rect.z; ++j) {
						vec4s& simdFrag = frame.at(j, i);
							
						ray.sr0 = sinv*view.unproject(vec4s(vec4((float)j + offset.x, (float)h - i + offset.y, 0.0f, 1.0f)));
						ray.sr0 /= _mm_permute_ps(ray.sr0, 0b11111111);
						ray.sr1 = sinv*view.unproject(vec4s(vec4((float)j + offset.x, (float)h - i + offset.y, 1.0f, 1.0f)));
						ray.sr1 /= _mm_permute_ps(ray.sr1, 0b11111111);
						ray.sr1 = (ray.sr1 - ray.sr0);
						ray.sr1 /= _mm_sqrt_ps(_mm_dp_ps(ray.sr1, ray.sr1, 0x7F));
						ray.sinvr1 = _mm_rcp_ps(ray.sr1);

						oray = ray;
						oray.d = 100.0f;


						if (bvh.findFirst(oray, stack, priority, true) > 0.0f)
							simdFrag += vec4s(shader.getColor(ray, oray.d, oray.plane, oray.color, bvh, stack, priority)).w1();
						else
							simdFrag += (envMap(env, ray.sr1)*envScale).min(1.0f);
						
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
				}/*
				if (sample%256 == 0)
					task.onEndNode(pview, vec2i((int)rect.x / square, (int)rect.y / square), vec2i((int)std::ceil((float)(img.width) / square), (int)std::ceil((float)(img.height) / square)), square);
					*/
			}
			//repaint
			/*
			if (worker.threadNumber == 0)
				task.onEndNode(pview, vec2i((int)rect.x / square, (int)rect.y / square), vec2i((int)std::ceil((float)(img.width) / square), (int)std::ceil((float)(img.height) / square)), square);
			*/
		}
		delete[] priority;
		//task.onEndNode(pview, vec2i(0, 0), vec2i(0, 0), square);
	}
}
