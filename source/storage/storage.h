#pragma once
struct Storage: public core::SIMD, public core::Getter<Storage> {
	core::PointCloud cloud;
	core::PointOctree cloudTree;
	core::PBVH pbvh;

	core::RenderShader shader;
	core::VolumetricShader volumetricShader;
	core::PBSShader pbsShader;

	core::simdImage simdFrame;

	core::simdImage hdri;
	core::simdImage hdriDiff; //preconvolved
	core::simdImage hdriMipmap[8];

	int renderedSamples;
	core::Image renderImage;
	core::Image* realtimeImage;

	core::Material material;

	Storage(){ set(*this); }
	int load(const char* path);
	void dispose();
};
