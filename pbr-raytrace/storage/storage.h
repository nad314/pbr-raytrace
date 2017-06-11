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
	core::simdImage hdriRef; //preconvolved reflection

	int renderedSamples;
	core::Image renderImage;

	core::Material material;

	Storage(){ set(*this); }
	int load(const char* path);
	void dispose();
};
