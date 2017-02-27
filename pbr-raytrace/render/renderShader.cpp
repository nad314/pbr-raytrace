#include <main>

namespace core {
	const float metallic = 0.0f;
	const float roughness = 0.5f;
	const vec4s base = /*(vec4s(205, 127, 50, 255)/vec4s(255.0f)).w1()*/vec4s(1.0f);
	const vec4s ior = vec4s(1.0f)/vec4s(1.18f);

	const vec4s RenderShader::getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const {
		Ray lightRay;
		PBVH::Ray lray;
		const vec4s point = (ray.sr0 + ray.sr1*vec4s(d));
		const vec4s vp = smat*point;
		const vec4s lp = __lightPos;

		const vec4s lightDir = (lp-vp).normalized3d();
		const vec4s V = (nmat*ray.sr1*(-1.0f)).normalized3d();
		const vec4s H = (lightDir + V).normalized3d();
		const vec4s N = (nmat*normal).normalized3d();

		const vec4s ndl = vec4s(0.0f).max(N.dot3(lightDir));
		const vec4s ndv = vec4s(0.0f).max(N.dot3(V));
		const vec4s ndh = vec4s(0.0f).max(N.dot3(H));
		const vec4s hdv = vec4s(0.001f).max(H.dot3(V));

		//light attenuation
		const vec4s lmp = lp - vp;
		float A = 20.0f / m128(lmp.dot3(lmp))[0];
		A = 0.0f;

		const vec4s envdiff = envMap(hdri_d, normal);
		const vec4s envspec = envMap(hdri_l, sninv*reflect(V, N));

		const vec4s specular = mix(vec4s(0.04f), base, metallic);
		//const vec4s specular = ior*base;
		const vec4s specfresnel = fresnel_factor(specular, ndv.m.m128_f32[0]);
		//return specfresnel;
		vec4s specref = cooktorrance_specular(ndl.m.m128_f32[0], ndv.m.m128_f32[0], ndh.m.m128_f32[0], specfresnel, roughness);
		specref *= ndl;
		//return specref;

		const vec4s diffref = (vec4s(1.0f) - specfresnel) * ndl * (1.0f / M_PI);

		vec4s reflected_light = vec4s(specref)*A;
		vec4s diffuse_light = vec4s(diffref)*A;
		
		vec4s iblspec = vec4s(0.99f).min(fresnel_factor(specular, ndv.m.m128_f32[0]));
		reflected_light += envspec * iblspec;
		diffuse_light += envdiff * (vec4s(1.0f) - envspec);
		
		return (diffuse_light*mix(base, vec4s(0.0f, 0.0f, 0.0f, 1.0f), metallic) + 
			reflected_light).min(vec4s(1.0f));

		return vec4s(0.0f);
	}
}
