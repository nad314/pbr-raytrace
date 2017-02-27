#pragma once
struct Storage: public core::SIMD, public core::Getter<Storage> {
	core::PointCloud cloud;
	core::PointOctree cloudTree;
	core::PBVH pbvh;

	core::RenderShader shader;
	core::VolumetricShader volumetricShader;

	core::simdImage simdFrame;
	core::simdImage hdri;
	int renderedSamples;

	core::Material material;

	Storage(){ set(*this); }
	int load(const char* path);
	void dispose();
};
