#include <main>

namespace core {
	void PBSShader::update(const simdView& view) {
		nmat = view.left.normalMatrix();
		material = Storage::get().material;
		envStrength = Settings::environmentStrength;
	}

	const vec4s PBSShader::getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const {
        const simdImage& hdri = Storage::get().hdri;

		const vec4s roughness = material.roughness;

		const vec4s L = mcrotate(normal, roughness); //function normalizes it
		const vec4s V = (ray.sr1*vec4s(-1.0f)).normalized3d();
		const vec4s H = (L + V).normalized3d();
		const vec4s N = normal.normalized3d();
		const vec4s newDir = mcrotate(normal).normalized3d();

		const vec4s ndl = vec4s(0.0f).max(N.dot3(L));
		const vec4s ndv = vec4s(0.0f).max(N.dot3(V));
		const vec4s ndh = vec4s(0.0f).max(N.dot3(H));
		//const vec4s hdv = vec4s(0.001f).max(H.dot3(V));

		const vec4s specFactor = mix(vec4s(0.04f), material.base, material.metallic);
		const vec4s fresnel = brdf::fresnelFactor(specFactor, ndv).w1();
        //maybe multiply cooktorrance with H.dot3(L)?
		const vec4s specular = (brdf::GGX_BRDF_Fast(ndl, ndv, ndh, fresnel, roughness) + fresnel).min(1.0f); 

        //reflection values
		const vec4s rdir = SSE::reflect(V, L);
		const vec4s envspec = envMap(hdri, (nmat*rdir).normalized3d()) * envStrength;

        //diffuse environment
		const vec4s envdiff = envMap(Storage::get().hdri, (nmat*newDir).normalized3d()) * envStrength;


		const vec4s reflected_light = envspec * specular;
		const vec4s diffuse_light = envdiff * mix(material.base, vec4s(0.0f).w1(), material.metallic) * (vec4s(1.0f) - specular) * newDir.dot3(N);

		return (diffuse_light + reflected_light);
	}

}
