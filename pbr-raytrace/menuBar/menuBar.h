#pragma once

class MenuBar final: public core::Form {
public:
	core::ImageButton button[8];

	MenuBar(): Form() {}
	void onOpening() override;
	void onOpened() override;
	void onClosing() override;

	int onLButtonDown(const core::eventInfo& e) override;
	int onLButtonUp(const core::eventInfo& e) override;
	int onResize(const core::eventInfo& e) override;
};
