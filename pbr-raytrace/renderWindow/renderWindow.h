#pragma once
class RenderWindow final : public core::RenderSurface, public core::SIMD {
private:
public:
	core::simdView view;
	bool alive = 0;

	inline operator core::simdView&() { return view; }
	
	inline virtual core::Image& image() { return view.img; }
	inline operator core::Image&() { return view.img; }

	void onOpening() override;
	void onOpened() override;
	int onReshape() override;

	void move(int xw, int yw);
};
