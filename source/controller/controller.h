#pragma once
class Controller: public oven::SIMD, public oven::EventKernel, public::oven::Getter<Controller> {
public:
	Storage* storage;
	oven::RenderSurface* parent;
	oven::simdView* view;
	bool valid = 0;
	oven::vec2i mouse;
	bool rotating = 0;
	bool dragging = 0;
	oven::WorkerGroup* wg;
	int samples = 2;
	vec4 clickPoint;
	float renderTime = 0.0f;
	oven::Timer<float> timer;
	bool benchMode = false;
	int mode = 0;
	int shaderID = 0;
	oven::Timer<float> benchTimer;
	int frameCounter = 0;
	bool veryBusy = 0;
	
	Controller(oven::RenderSurface* p, Storage* st);
	~Controller() {delete wg; wg = NULL;}

	inline void invalidate() { valid = 0; Storage::get().shader.update(*view); Storage::get().volumetricShader.update(*view); Storage::get().pbsShader.update(*view); }
	inline void validate() { valid = 1; }
	int onLButtonDown(const oven::eventInfo& e) override;
	int onLButtonUp(const oven::eventInfo& e) override;
	int onRButtonDown(const oven::eventInfo& e) override;
	int onRButtonUp(const oven::eventInfo& e) override;
	int onMouseMove(const oven::eventInfo& e) override;
	int onKeyDown(const oven::eventInfo& e) override;
	int onMousewheel(const oven::eventInfo& e) override;
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
	inline const oven::Renderer::PixelShader& getShader() const {if (shaderID == 0)return Storage::get().shader; else if (shaderID==1) return Storage::get().pbsShader; else return Storage::get().volumetricShader;}

	bool loadHDRI(const std::string& path);
	void makeMipmaps();
	void makePreconvolvedImage();

	//inlines
	inline oven::Ray getRay(const float x, const float y) const {
		return oven::Renderer::unproject(*view,
		(float)x, 
		(float)y);
	}


};
