#pragma once
class RenderWindow final : public oven::RenderSurface {
private:
public:
	oven::simdView view;
	bool alive = 0;
	//oven::Image* imgptr = NULL;

	inline operator oven::simdView&() { return view; }
	/*
	inline virtual oven::Image& image() { return imgptr?*imgptr:view.img; }
	inline operator oven::Image&() { return imgptr?*imgptr:view.img; }
*/
	void onOpening() override;
	void onOpened() override;
	int onReshape() override;

	void move(int xw, int yw);
};
