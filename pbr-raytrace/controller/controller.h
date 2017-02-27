#pragma once
class Controller: public core::SIMD, public core::EventListener, public::core::Getter<Controller> {
public:
	Storage* storage;
	core::Window* parent;
	core::simdView* view;
	bool valid = 0;
	core::vec2i mouse;
	bool rotating = 0;
	bool dragging = 0;
	core::Renderer::WorkerGroup* wg;
	int samples = 2;
	vec4 clickPoint;
	float renderTime = 0.0f;
	core::Timer<float> timer;
	bool benchMode = false;
	
	Controller(core::Window* p, Storage* st);
	~Controller();

	inline void invalidate() { valid = 0; Storage::get().shader.update(*view); Storage::get().volumetricShader.update(*view);
	}
	inline void validate() { valid = 1; }
	int onLButtonDown(const core::eventInfo& e) override;
	int onLButtonUp(const core::eventInfo& e) override;
	int onRButtonDown(const core::eventInfo& e) override;
	int onRButtonUp(const core::eventInfo& e) override;
	int onMouseMove(const core::eventInfo& e) override;
	int onKeyDown(const core::eventInfo& e) override;
	int onMousewheel(const core::eventInfo& e) override;
	void render();
	void getPoint(const float x, const float y);
	core::Ray getRay(const float x, const float y) const;
	void home();

	void renderPBRImage();
	void clearSIMDImage();
	void makeSIMDImage();
};
