#include <main>

namespace core {

	void RenderShader::update(const simdView& view) {
		__lightPos = vec4s(-0.5f, -0.5f, 50.0f, 1.0f);

		matrixf modelview = view.modelview;
		matrixf inv = modelview;
		inv.invert();
		sinv = inv;
		lightPos = sinv*__lightPos;
		nmat = view.left.normalMatrix();
		smat = view.mat;
		matrixf ninv = modelview.normalMatrix();
		ninv.invert();
		sninv = ninv;
		material = Storage::get().material;
		envStrength = Settings::environmentStrength;
	}

	const vec4s RenderShader::getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const {
		Storage& data = Storage::get();
        const simdImage& hdri = Storage::get().hdri;

		const vec4s roughness = material.roughness;

		const vec4s N = normal.normalized3d();
		const vec4s L = N; //in this case L = N
		const vec4s V = (ray.sr1*vec4s(-1.0f)).normalized3d();
		const vec4s H = (L + V).normalized3d();

		const vec4s ndl = vec4s(0.0f).max(N.dot3(L));
		const vec4s ndv = vec4s(0.0f).max(N.dot3(V));
		const vec4s ndh = vec4s(0.0f).max(N.dot3(H));

		const vec4s specFactor = mix(vec4s(0.04f), material.base, material.metallic);
		const vec4s fresnel = fresnelFactor(specFactor, ndv).w1();
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
