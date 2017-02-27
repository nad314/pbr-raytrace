#pragma once

namespace core {
	struct Material {
		float metallic;
		float roughness;
		vec4s base;

		Material() : metallic(0.0f), roughness(0.075f), base(0.95f, 0.95f, 0.95f, 1.0f) {}
	};

	struct RenderShader final : public Renderer::PixelShader, public SIMD {
		const vec4s __lightPos = vec4s(-0.5f, -0.5f, 5.0f, 1.0f);
		vec4s lightPos;
		matrixs smat;
		matrixs sinv;
		matrixs nmat;
		matrixs sninv;
		Image hdri;
		Image hdri_l;
		Image hdri_d;

		RenderShader() {
			lightPos = __lightPos;
			Path::pushDir();
			Path::goHome();
			try {
				if (!hdri.loadPng("data/png/hdri3.png"))
					throw std::exception("Can't load PNG\n");
			}
			catch (std::exception e) {
				MessageBox(NULL, e.what(), "Error", MB_OK);
			}
			hdri_l = hdri;
			const float scale = 32.0f;
			hdri_l.linearDownscale((int)(hdri.width / scale), (int)(hdri.height / scale));
			hdri_d = hdri;
			hdri_d.linearDownscale((int)(hdri.width / 128.0f), (int)(hdri.height / 128.0f));
			hdri_d.loadPng("data/png/hdri2d.png");
			Path::popDir();
		}
		RenderShader(const simdView& view) { update(view); }
		inline void update(const simdView& view) {
			matrixf modelview = view.rotation*view.translation;
			matrixf inv = modelview;
			inv.invert();
			sinv = inv;
			lightPos = sinv*__lightPos;
			nmat = modelview.normalMatrix();
			smat = view.mat;
			matrixf ninv = modelview.normalMatrix();
			ninv.invert();
			sninv = ninv;
		}
		const vec4s getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const;
	};

	struct VolumetricShader final : public Renderer::PixelShader, public SIMD {
		Material material;
		int samples;
		matrixs smat;
		matrixs sinv;
		matrixs nmat;
		matrixs sninv;
		simdImage hdri;
		float envStrength;
		int bounces;
		VolumetricShader();
		void update(const simdView& view);
		const vec4s getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority, int rek) const;
		const vec4s getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const;
	};

	vec4s envMap(const Image& img, const vec4s& r);
}