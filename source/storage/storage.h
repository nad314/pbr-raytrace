#pragma once
struct Storage: public oven::SIMD, public oven::Getter<Storage> {
	oven::PointCloud cloud;
	oven::PointOctree cloudTree;
	oven::PBVH pbvh;

	oven::RenderShader shader;
	oven::VolumetricShader volumetricShader;
	oven::PBSShader pbsShader;

	oven::simdImage simdFrame;

	oven::simdImage hdri;
	oven::simdImage hdriDiff; //preconvolved
	oven::simdImage hdriMipmap[8];

	int renderedSamples;
	oven::Image renderImage;
	oven::Image* realtimeImage;

	oven::Material material;

	Storage(){ set(*this); }
	int load(const char* path);
	void dispose();
};
