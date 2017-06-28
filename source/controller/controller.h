#pragma once
class Controller: public core::SIMD, public core::EventKernel, public::core::Getter<Controller> {
public:
	Storage* storage;
	core::RenderSurface* parent;
	core::simdView* view;
	bool valid = 0;
	core::vec2i mouse;
	bool rotating = 0;
	bool dragging = 0;
	core::WorkerGroup* wg;
	int samples = 2;
	vec4 clickPoint;
	float renderTime = 0.0f;
	core::Timer<float> timer;
	bool benchMode = false;
	int mode = 0;
	int shaderID = 0;
	core::Timer<float> benchTimer;
	int frameCounter = 0;
	bool veryBusy = 0;
	
	Controller(core::RenderSurface* p, Storage* st);
	~Controller() {delete wg; wg = NULL;}

	inline void invalidate() { valid = 0; Storage::get().shader.update(*view); Storage::get().volumetricShader.update(*view); Storage::get().pbsShader.update(*view); }
	inline void validate() { valid = 1; }
	int onLButtonDown(const core::eventInfo& e) override;
	int onLButtonUp(const core::eventInfo& e) override;
	int onRButtonDown(const core::eventInfo& e) override;
	int onRButtonUp(const core::eventInfo& e) override;
	int onMouseMove(const core::eventInfo& e) override;
	int onKeyDown(const core::eventInfo& e) override;
	int onMousewheel(const core::eventInfo& e) override;
	void getPoint(const float x, const float y);
	void render();
	void fullRender();
	void home();

	void clearSIMDFrame();
	void makeSIMDFrame();

	inline int getMode() const {return mode;}
	void setMode(const int& m);
	inline int getShaderID() const {return shaderID;}
	inline void setShader(const int& n){shaderID = n;}
	inline const core::Renderer::PixelShader& getShader() const {if (shaderID == 0)return Storage::get().shader; else if (shaderID==1) return Storage::get().pbsShader; else return Storage::get().volumetricShader;}

	bool loadHDRI(const std::string& path) const;
	void makeMipmaps();
	void makePreconvolvedImage();

	//inlines
	inline core::Ray getRay(const float x, const float y) const {
		return core::Renderer::unproject(*view, 
		view->_mm_imvp, 
		(float)x, 
		(float)storage->realtimeImage->height - y);
	}


};
