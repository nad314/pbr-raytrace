#pragma once

namespace core {
	inline vec4s envMap(const Image& img, const vec4s& r) {
		const vec4s m(r);
		const float my = asin(m[1])/(M_PI/2.0f); //mapped y
		const float y = (my / (2.0f) + 0.5f)*(img.height);
		const float x = (atan2(m[0], m[2]) / (M_PI*2.0f) + 0.5f)*(img.width);
		return imageLinear(img, x, y);
	}

	inline const vec4s envMap(const simdImage& img, const vec4s& r) {
		const m128 m(r);
		const float my = asin(m[1])/(M_PI/2.0f); //mapped y
		const float y = (my / (2.0f) + 0.5f)*(img.height);
		const float x = (atan2(m[0], m[2]) / (M_PI*2.0f) + 0.5f)*(img.width );
		return imageLinear(img, x, y);
	}

	inline const vec4s mcrotate(const vec4s& v, const vec4s& roughness) {
		const vec4s rv = core::RanduinWrynn::topdeck() * roughness;
		return (v + rv).normalized3d();
	}

	inline const vec4s mcrotate(const vec4s& v) {
		const vec4s rv = core::RanduinWrynn::topdeck();
		return (v + rv*0.9f).normalized3d(); //we do just a tiny bit of importance sampling to avoid impossible colisions
	}
}
