#pragma once

namespace core {
	template <class _t> inline _t mix(const _t& x, const _t& y, const float& a) {
		return x * (_t(1.0f) - a) + y * a;
	}

	inline vec4s dGGX(const vec4s& roughness, const vec4s& NdH) {
		const vec4s m = roughness * roughness;
		const vec4s m2 = m * m;
		const vec4s d = (NdH * m2 - NdH) * NdH + 1.0f;
		return m2 / (d * d * M_PI);
	}

	inline vec4s gSchlick(const vec4s& roughness, const vec4s& NdV, const vec4s& NdL) {
		const vec4s fac = _mm_rcp_ps(_mm_set1_ps(M_PI));
		const vec4s k = roughness * roughness * fac;
		const vec4s V = NdV * (vec4s(1.0f) - k) + k;
		const vec4s L = NdL * (vec4s(1.0f) - k) + k;
		return NdL * NdV / (V * L);
	}

	inline vec4s gSchlickPart(const vec4s& roughness, const vec4s& NdV, const vec4s& NdL) {
		const vec4s V_PI = _mm_set1_ps(M_PI);
		const vec4s k = roughness * roughness / V_PI;
		const vec4s vk = vec4s(1.0f) - k;
		return (NdV * vk + k) * (NdL * vk + k) * V_PI;
	}

	inline vec4s GGX_BRDF(const vec4s& NdL, const vec4s& NdV, const vec4s& NdH, const vec4s& fresnel, const vec4s& roughness) {
		const vec4s D = dGGX(roughness, NdH);
		const vec4s G = gSchlick(roughness, NdV, NdL);
		return fresnel * G * D / (NdL * NdV * M_PI);
	}

	inline vec4s GGX_BRDF_Fast(const vec4s& NdL, const vec4s& NdV, const vec4s& NdH, const vec4s& fresnel, const vec4s& roughness) {
		const vec4s D = dGGX(roughness, NdH);
		const vec4s G = gSchlickPart(roughness, NdV, NdL);
		return fresnel * G * D;
	}

	inline vec4s fresnelFactor(const vec4s& f0, const vec4s& product) {
		const vec4s v1 = vec4s(1.0f);
		const vec4s p1 = v1 - product;
		const vec4s p2 = p1*p1;
		return f0 + (v1 - f0)*p2*p2*p1; // *pow(1.0f - product, 5.0f);
	}

	inline vec4s reflect(const vec4s& r, const vec4s& n) {
		return (n*r.dot3(n)*2.0f - r).normalized3d();
	}

	inline vec4s imageNearest(const Image& img, const int& x, const int& y) {
		return (vec4s((float)img.at_c(x, y, 0), (float)img.at_c(x, y, 1), (float)img.at_c(x, y, 2), 255.0f) / 255.0f);
	}

	inline vec4s imageNearest(const simdImage& img, const int& x, const int& y) {
		int X = abs(x);
		int Y = abs(y);
		if (X>=img.width)
			X = 0;
		if (Y>=img.height)
			Y = 0;
		return img.at_c(X, Y);
	}

	inline vec4s imageLinear(const Image& img, const float& x, const float& y) {
		const float u = x - 0.5f;
		const float v = y - 0.5f;
		const int ix = (int)u%(img.width);
		const int iy = (int)v%(img.height);
		const float ru = u - ix;
		const float rv = v - iy;
		const int iix = (ix + 1) % (img.width);
		const int iiy = (iy + 1) % (img.height);

		return (imageNearest(img, ix, iy)*(1.0f - ru) + imageNearest(img, iix, iy)*ru)*(1.0f - rv) +
			(imageNearest(img, ix, iiy)*(1.0f - ru) + imageNearest(img, iix, iiy)*ru)*rv;
	}

	inline vec4s imageLinear(const simdImage& img, const float& x, const float& y) {
		const float u = x - 0.5f;
		const float v = y - 0.5f;
		const int ix = (int)u % (img.width);
		const int iy = (int)v % (img.height);
		const float ru = u - ix;
		const float rv = v - iy;
		const int iix = (ix + 1) % (img.width);
		const int iiy = (iy + 1) % (img.height);

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

	inline vec4s mcrotate(const vec4s& v, const vec4s& roughness) {
		const vec4s rv = core::RanduinWrynn::topdeck() * roughness;
		return (v + rv).normalized3d();
	}

	inline vec4s mcrotate(const vec4s& v) {
		const vec4s rv = core::RanduinWrynn::topdeck();
		return (v + rv).normalized3d();
	}

}
