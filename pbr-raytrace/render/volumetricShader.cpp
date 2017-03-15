#include <main>

namespace core {
	void VolumetricShader::update(const simdView& view) {
		matrixf inv = view.rotation*view.translation;
		inv.invert();
		matrixs sinv = inv;
		srand(rand());
		material = Storage::get().material;
		envStrength = Settings::environmentStrength;
		bounces = Settings::maxBounces;
	}

	VolumetricShader::VolumetricShader() {
		samples = 1;
		Path::pushDir();
		Path::goHome();
		try {
			if (!hdri.loadHDR("data/hdri/wobbly_bridge_4k.hdr"))
				throw core::exception("Can't load HDR\n");
			hdri.tonemap();
			//hdri.gammaCompress(0.66f);
		}
		catch (std::exception e) {
			core::Debug::log("%s\n", e.what());
		}
		Path::popDir();
	}

	const vec4s VolumetricShader::getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority, int rek) const {
		if (rek == 0)
			return _mm_setzero_ps();

		const vec4s point = (ray.sr0 + ray.sr1*vec4s(d));
		const vec4s vp = smat*point;

		Ray lightRay = ray;
		Ray reflectRay = ray;
		PBVH::Ray lray;
		vec4s nn;

		const vec4s roughness = material.roughness;

		const vec4s L = mcrotate(normal, roughness).normalized3d();
		const vec4s V = (lightRay.sr1*vec4s(-1.0f)).normalized3d();
		const vec4s H = (L + V).normalized3d();
		const vec4s N = normal.normalized3d();
		const vec4s newDir = mcrotate(normal).normalized3d();

		const vec4s ndl = vec4s(0.0f).max(N.dot3(L));
		const vec4s ndv = vec4s(0.0f).max(N.dot3(V));
		const vec4s ndh = vec4s(0.0f).max(N.dot3(H));
		const vec4s hdv = vec4s(0.001f).max(H.dot3(V));

		const vec4s specular = mix(vec4s(0.04f), material.base, material.metallic);
		const vec4s fresnel = fresnelFactor(specular, ndv).w1();

		const vec4s rdir = reflect(V, L);
		vec4s envdiff(0.0f);
		vec4s envspec(0.0f);
	
		reflectRay.sr0 = point + rdir*vec4s(0.0001f);
		reflectRay.sr1 = rdir;
		reflectRay.sinvr1 = _mm_rcp_ps(lightRay.sr1);
		lray = reflectRay;
		lray.d = -100.0f;

		if (bvh.findFirst(lray, stack, priority, false) < 0.0f)
			envspec = envMap(hdri, rdir) * envStrength;
		else
			envspec = getColor(reflectRay, lray.d, lray.plane, lray.color, bvh, stack, priority, rek - 1);

		//maybe multiply cooktorrance with H.dot3(L)?
		const vec4s iblspec = (GGX_BRDF_Fast(ndl, ndv, ndh, fresnel, roughness) + fresnel).min(1.0f);

		lightRay.sr0 = point + newDir*vec4s(0.0001f);
		lightRay.sr1 = newDir;
		lightRay.sinvr1 = _mm_rcp_ps(lightRay.sr1);
		lray = lightRay;
		lray.d = -100.0f;
		if (bvh.findFirst(lray, stack, priority, false) < 0.0f)
			envdiff = envMap(hdri, newDir) * envStrength;
		else 
			envdiff = getColor(lightRay, lray.d, lray.plane, lray.color, bvh, stack, priority, rek - 1);

		const vec4s nndl = N.dot3(newDir);
		const vec4s reflected_light = envspec * iblspec;
		const vec4s diffuse_light = envdiff * nndl * (vec4s(1.0f) - iblspec);

		return (diffuse_light*mix(material.base, vec4s(0.0f, 0.0f, 0.0f, 1.0f), material.metallic) +
			reflected_light);
	}

	const vec4s VolumetricShader::getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const {
		return getColor(ray, d, normal, color, bvh, stack, priority, bounces);
	}
}