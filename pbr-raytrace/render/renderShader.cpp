#include <main>

namespace core {

	void RenderShader::update(const simdView& view) {
		__lightPos = vec4s(-0.5f, -0.5f, 50.0f, 1.0f);

		matrixf modelview = view.rotation*view.translation;
		matrixf inv = modelview;
		inv.invert();
		sinv = inv;
		lightPos = sinv*__lightPos;
		nmat = view.rotation.normalMatrix();
		smat = view.mat;
		matrixf ninv = modelview.normalMatrix();
		ninv.invert();
		sninv = ninv;
		material = Storage::get().material;
		envStrength = Settings::environmentStrength;
	}

	const vec4s RenderShader::getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const {
/*
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

		vec4s reflected_light = vec4s(specref)*A;
		vec4s diffuse_light = vec4s(diffref)*A;
		
		return (diffuse_light*mix(color, vec4s(0.0f, 0.0f, 0.0f, 1.0f), material.metallic) + 
			reflected_light).min(vec4s(1.0f));
*/

		Storage& data = Storage::get();
        const simdImage& hdri = Storage::get().hdri;

		const vec4s roughness = material.roughness;

		const vec4s L = normal; //function normalizes it
		const vec4s V = (ray.sr1*vec4s(-1.0f)).normalized3d();
		const vec4s H = (L + V).normalized3d();
		const vec4s N = normal.normalized3d();

		const vec4s ndl = vec4s(0.0f).max(N.dot3(L));
		const vec4s ndv = vec4s(0.0f).max(N.dot3(V));
		const vec4s ndh = vec4s(0.0f).max(N.dot3(H));
		//const vec4s hdv = vec4s(0.001f).max(H.dot3(V));

		const vec4s specFactor = mix(vec4s(0.04f), material.base, material.metallic);
		const vec4s fresnel = fresnelFactor(specFactor, ndv).w1();
        //maybe multiply cooktorrance with H.dot3(L)?
		const vec4s specular = (GGX_BRDF_Fast(ndl, ndv, ndh, fresnel, roughness) + fresnel).min(1.0f);

        //reflection values
		const int low = std::max(0, (int)(roughness[0]*8) - 1);
		const int high = std::min(7, low + 1);
		const float interp = roughness[0]*8.0f - high;
		const vec4s rdir = (nmat*reflect(V, N)).normalized3d();
		const vec4s envspec = mix(envMap(data.hdriMipmap[low], rdir), envMap(data.hdriMipmap[high], rdir), interp) * envStrength;

        //diffuse environment
		const vec4s envdiff = envMap(data.hdriDiff, (nmat*normal).normalized3d()) * envStrength;


		const vec4s reflected_light = envspec * specular;
		const vec4s diffuse_light = envdiff * mix(material.base, vec4s(0.0f).w1(), material.metallic) * (vec4s(1.0f) - specular);

		return (diffuse_light + reflected_light);

	}
}
