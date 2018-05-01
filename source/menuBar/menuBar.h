#pragma once

class MenuBar final: public oven::Surface {
public:
	oven::ImageButton button[8];

	MenuBar(): Surface() {}
	void onOpening() override;
	void onOpened() override;

	int onLButtonDown(const oven::eventInfo& e) override;
	int onLButtonUp(const oven::eventInfo& e) override;
	int onResize(const oven::eventInfo& e) override;
};
