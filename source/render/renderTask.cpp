#include <main> 

namespace core {
	void RenderTask::execute(Worker* pWorker) {
		if (pWorker == NULL)
			return;
		Worker& worker = *pWorker;
		PBVH& bvh = *pbvh;
		simdView &view = *pview;
		Image &img = *Storage::get().realtimeImage;
		int* mp = reinterpret_cast<int*>(img.data);
		vec4 bp, bq; // bounding box projected coordinates
		Renderer::projectedBox(bvh, pview, bp, bq);

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

		vec4s envScale = Settings::environmentStrength;
		Storage& data = Storage::get();
		const core::Renderer::PixelShader& shader = Controller::get().getShader();

		__m128i dfv = _mm_cvtps_epi32(vec4s(0.2f).w1()*vec4s(255.0f));
		dfv = _mm_packus_epi16(dfv, dfv);
		dfv = _mm_packus_epi16(dfv, dfv);
		const int defFragOut = _mm_cvtsi128_si32(dfv);

		//__m128 svmin;
		const matrixs sNormalMatrix = view.rotation.normalMatrix();
		//const matrixs sNormalMatrix;
		for (int gy = 0; gy < img.height; gy += square) {
			if (gy > bq.y || (gy + square) < bp.y)
				continue;
			for (int gx = square*worker.threadNumber; gx < w; gx += square*worker.threadCount) {
				if (gx > bq.x || (gx + square) < bp.x)
					continue;
				const int mx = std::min(gx + square, w);
				const int my = std::min(gy + square, h);
				for (int i = gy; i < my; ++i) {
					for (int j = gx; j < mx; ++j) {
						core::Renderer::unproject(ray, view, sinv, (float)j, (float)h - i);
						const float d = bvh.findFirst(ray, stack, priority, true);

						if (d > 0.0f) {
							//const vec4s pminusl = (lightPos - (ray.sr0 + ray.sr1*vec4s(oray.d)));
							//const vec4s ndotl = oray.plane.dot3(pminusl / _mm_sqrt_ps(pminusl.dot3(pminusl)));
							const vec4s frag = _mm_mul_ps(shader.getColor(ray, d, ray.normal, ray.color, bvh, stack, priority), _mm_set1_ps(255.0f));
							__m128i fv = _mm_cvtps_epi32(frag);
							fv = _mm_packus_epi16(fv, fv);
							fv = _mm_packus_epi16(fv, fv);
							int fragOut = _mm_cvtsi128_si32(fv);
							_mm_stream_si32(mp + j + i*w, fragOut);
						}
						else
							_mm_stream_si32(mp + j + i*w, defFragOut);
					}
				}
			}
		}
		delete[] priority;

	}
}