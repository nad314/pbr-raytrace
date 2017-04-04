#pragma once

namespace core {
	struct Material {
		float metallic;
		float roughness;
		vec4s base;

		Material() : metallic(0.0f), roughness(0.9f), base(0.95f, 0.95f, 0.95f, 1.0f) {}
	};

	struct RenderShader final : public Renderer::PixelShader, public SIMD {
		vec4s __lightPos = vec4s(-0.5f, -0.5f, 5.0f, 1.0f);
		vec4s lightPos;
		matrixs smat;
		matrixs sinv;
		matrixs nmat;
		matrixs sninv;
		Material material;

		RenderShader() {
			lightPos = __lightPos;
			Path::pushDir();
			Path::goHome();
			Path::popDir();
		}
		RenderShader(const simdView& view) { update(view); }
		void update(const simdView& view);
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