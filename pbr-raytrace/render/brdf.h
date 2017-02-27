#pragma once

namespace core {
	template <class _t> inline _t mix(const _t& x, const _t& y, const float& a) {
		return x * (_t(1.0f) - a) + y * a;
	}

	inline float D_GGX(const float& roughness, const float& NdH) {
		const float m = roughness * roughness;
		const float m2 = m * m;
		const float d = (NdH * m2 - NdH) * NdH + 1.0f;
		return m2 / (M_PI * d * d);
	}

	inline float G_schlick(const float& roughness, const float& NdV, const float& NdL) {
		const float fac = (float)(1.0f / M_PI);
		float k = roughness * roughness * fac;
		float V = NdV * (1.0f - k) + k;
		float L = NdL * (1.0f - k) + k;
		return NdL * NdV / (V * L);
	}

	inline vec4s cooktorrance_specular(const float NdL, const float NdV, const float NdH, const vec4s fresnel, const float roughness) {
		const float D = D_GGX(roughness, NdH);
		const float G = G_schlick(roughness, NdV, NdL);
		return fresnel * G * D / (NdL * NdV * 4.0f);
	}

	inline vec4s fresnel_factor(const vec4s& f0, const float& product) {
		return f0 + (vec4s(1.0f) - f0)*pow(1.0f - product, 5.0f);
		//return mix(f0, base, (float)pow(1.01f - product, 5.0f));
	}

	inline vec4s reflect(const vec4s& r, const vec4s& n) {
		return (n*r.dot3(n)*2.0f - r).normalized3d();
	}

	inline vec4s imageNearest(const Image& img, const int& x, const int& y) {
		return (vec4s((float)img.at_c(x, y, 0), (float)img.at_c(x, y, 1), (float)img.at_c(x, y, 2), 255.0f) / 255.0f);
	}

	inline vec4s imageNearest(const simdImage& img, const int& x, const int& y) {
		return img.at_c(x, y);
	}

	inline vec4s imageLinear(const Image& img, const float& x, const float& y) {
		const float u = x - 0.5f;
		const float v = y - 0.5f;
		const int ix = (int)u%(img.width - 1);
		const int iy = (int)v%(img.height - 1);
		const float ru = u - ix;
		const float rv = v - iy;
		const int iix = (ix + 1) % (img.width - 1);
		const int iiy = (iy + 1) % (img.height - 1);

		return (imageNearest(img, ix, iy)*(1.0f - ru) + imageNearest(img, iix, iy)*ru)*(1.0f - rv) +
			(imageNearest(img, ix, iiy)*(1.0f - ru) + imageNearest(img, iix, iiy)*ru)*rv;
	}

	inline vec4s imageLinear(const simdImage& img, const float& x, const float& y) {
		const float u = x - 0.5f;
		const float v = y - 0.5f;
		const int ix = (int)u % (img.width - 1);
		const int iy = (int)v % (img.height - 1);
		const float ru = u - ix;
		const float rv = v - iy;
		const int iix = (ix + 1) % (img.width - 1);
		const int iiy = (iy + 1) % (img.height - 1);

		return (imageNearest(img, ix, iy)*(1.0f - ru) + imageNearest(img, iix, iy)*ru)*(1.0f - rv) +
			(imageNearest(img, ix, iiy)*(1.0f - ru) + imageNearest(img, iix, iiy)*ru)*rv;
	}

	inline vec4s envMap(const Image& img, const vec4s& r) {
		const m128 m(r);
		const float y = (m[1] / (2.0f) + 0.5f)*(img.height - 1);
		const float x = (atan2(m[0], m[2]) / (M_PI*2.0f) + 0.5f)*(img.width - 1);
		return imageLinear(img, x, y);
	}

	inline vec4s envMap(const simdImage& img, const vec4s& r) {
		const m128 m(r);
		const float y = (m[1] / (2.0f) + 0.5f)*(img.height - 1);
		const float x = (atan2(m[0], m[2]) / (M_PI*2.0f) + 0.5f)*(img.width - 1);
		return imageLinear(img, x, y);
	}

	inline vec4s mcrotate(const vec4s v, const float roughness) {
		const vec4s rv = vec4s((float)(rand() % 1000 - 500), (float)(rand() % 1000 - 500), (float)(rand() % 1000 - 500), 500.0f) / 500.0f * roughness;
		return (v + rv).normalized3d();
	}
}