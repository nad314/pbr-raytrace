#pragma once

class MenuBar final: public core::Surface {
public:
	core::ImageButton button[8];

	MenuBar(): Surface() {}
	void onOpening() override;
	void onOpened() override;

	int onLButtonDown(const core::eventInfo& e) override;
	int onLButtonUp(const core::eventInfo& e) override;
	int onResize(const core::eventInfo& e) override;
};
