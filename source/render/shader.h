#pragma once

namespace oven {
	struct Material {
		float metallic;
		float roughness;
		vec4s base;

		Material() : metallic(0.0f), roughness(0.75f), base(0.8f, 0.73f, 0.61f, 1.0f) {}
	};

	struct RenderShader final : public Renderer::PixelShader, public SIMD {
		vec4s __lightPos = vec4s(-0.5f, -0.5f, 5.0f, 1.0f);
		vec4s lightPos;
		matrixs smat;
		matrixs sinv;
		matrixs nmat;
		matrixs sninv;
		Material material;
		float envStrength;

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
		matrixs nmat;
		float envStrength;
		int bounces;
		void update(const simdView& view);
		const vec4s getColor(const Ray& ray, const PBVH& bvh, std::pair<int, float>* stack, int rek) const;
		const vec4s getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const;
	};
	
	struct PBSShader final : public Renderer::PixelShader, public SIMD {
		Material material;
		matrixs nmat;
		float envStrength;
		void update(const simdView& view);
		const vec4s getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const;
	};
}