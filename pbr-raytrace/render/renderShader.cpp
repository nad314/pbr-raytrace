#include <main>

namespace core {

	void RenderShader::update(const simdView& view) {
		__lightPos = vec4s(-0.5f, -0.5f, 50.0f, 1.0f);

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
		material = Storage::get().material;
	}

	const vec4s RenderShader::getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const {
		Ray lightRay;
		PBVH::Ray lray;
		const vec4s point = (ray.sr0 + ray.sr1*vec4s(d));
		const vec4s vp = smat*point;
		const vec4s lp = __lightPos;

		const vec4s lightDir = (lp-vp).normalized3d();
		const vec4s V = (nmat*ray.sr1*(-1.0f)).normalized3d();
		const vec4s H = (lightDir + V).normalized3d();
		vec4s N = (nmat*normal).normalized3d();
		const vec4s cond = _mm_cmpge_ps(N.dot3(lightDir), _mm_setzero_ps());
		N = vec4s(_mm_and_ps(N, cond)) + _mm_andnot_ps(cond, N*(-1.0f));

		const vec4s ndl = vec4s(0.0f).max(N.dot3(lightDir));
		const vec4s ndv = vec4s(0.0f).max(N.dot3(V));
		const vec4s ndh = vec4s(0.0f).max(N.dot3(H));
		const vec4s hdv = vec4s(0.001f).max(H.dot3(V));
		
		//light attenuation
		const vec4s lmp = lp - vp;
		float A = 4000.0f / m128(lmp.dot3(lmp))[0];

		const vec4s specular = mix(vec4s(0.04f), color, material.metallic);
		const vec4s specfresnel = fresnelFactor(specular, m128(ndv.m)[0]);
		const vec4s specref = GGX_BRDF_Fast(ndl, ndv, ndh, specfresnel, material.roughness) * ndl;


		const vec4s diffref = (vec4s(1.0f) - specfresnel) * ndl * (1.0f / M_PI);

		vec4s reflected_light = vec4s(specref)*A*color;
		vec4s diffuse_light = vec4s(diffref)*A*color;
		
		return (diffuse_light*mix(color, vec4s(0.0f, 0.0f, 0.0f, 1.0f), material.metallic) + 
			reflected_light).min(vec4s(1.0f));
	}
}
