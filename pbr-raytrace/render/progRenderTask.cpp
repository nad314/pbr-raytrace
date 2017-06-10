#include <main> 

namespace core {
	std::atomic_int progRenderTask::tc;
	void progRenderTask::execute(Worker* pWorker) {
		if (pWorker == NULL)
			return;
		Worker& worker = *pWorker;
		PBVH& bvh = *pbvh;
		simdView &view = *pview;
		Image &img = view.img;
		int* mp = reinterpret_cast<int*>(img.data);
		vec4 bp, bq; // bounding box projected coordinates
		Renderer::projectedBox(bvh, pview, bp, bq);

		if (worker.threadNumber == 0) {
			++Storage::get().renderedSamples;
			tc = 1;
		}
		else while (tc == 0)
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));

		const int w = img.width;
		const int h = img.height;

		matrixf inv = view.mat;
		inv.invert();
		matrixs sinv = inv;

		const int square = 8;
		vec4s lightPos = vec4s(inv*vec4(0.0f, 0.0f, -5.0f, 1.0f));

		Ray ray;
		std::pair<int, float> stack[256];
		int* priority = new int[bvh.inner.size()];
		memset(priority, 0, bvh.inner.size() * sizeof(int));

		Storage& data = Storage::get();
		//core::VolumetricShader& shader = data.volumetricShader;
		//core::RenderShader& shader = data.shader;
		core::Renderer::PixelShader& shader = Controller::get().getShader();

		vec2 offset = vec2(rand() % 128, rand() % 128) / 128;

		vec4s envScale = Settings::environmentStrength;

		for (int gy = 0; gy < img.height; gy += square) {
			/*if (gy > bq.y || (gy + square) < bp.y)
				continue;*/
			for (int gx = square*worker.threadNumber; gx < w; gx += square*worker.threadCount) {
				/*if (gx > bq.x || (gx + square) < bp.x)
					continue;*/
				const int mx = std::min(gx + square, w);
				const int my = std::min(gy + square, h);
				for (int i = gy; i < my; ++i) {
					for (int j = gx; j < mx; ++j) {
						core::Renderer::unproject(ray, view, sinv, (float)j + offset.x, (float)h - i + offset.y);
						const float d = bvh.findFirst(ray, stack, priority, true);

						if (d > 0.0f)
							data.simdFrame.at(j, i) += vec4s(shader.getColor(ray, d, ray.normal, ray.color, bvh, stack, priority)).w1();
						else
							data.simdFrame.at(j, i) += /*(envMap(data.volumetricShader.hdri, ray.sr1)*envScale).min(1.0f)*/vec4s(0.2f).w1();
							

						__m128i xmm0 = _mm_cvtsi32_si128(*(mp + j + i*w));
						xmm0 = _mm_unpacklo_epi8(xmm0, _mm_setzero_si128());
						xmm0 = _mm_unpacklo_epi16(xmm0, _mm_setzero_si128());
						vec4s xmm1 = _mm_cvtepi32_ps(xmm0);
						vec4s frag = data.simdFrame.at(j, i) / vec4s((float)data.renderedSamples);
						frag = frag*vec4s(255.0f)*_mm_permute_ps(frag, 0b11111111) + xmm1*(vec4s(1.0f) - _mm_permute_ps(frag, 0b11111111));
						__m128i fv = _mm_cvtps_epi32(frag);
						fv = _mm_packus_epi16(fv, fv);
						fv = _mm_packus_epi16(fv, fv);
						int fragOut = _mm_cvtsi128_si32(fv);
						//_mm_stream_si32(mp + j + i*w, fragOut);
						memcpy(mp + j + i*w, &fragOut, sizeof(int));
					}
				}
			}
		}
		delete[] priority;
	}
}
