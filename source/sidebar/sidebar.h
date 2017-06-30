#pragma once

class Sidebar final : public core::Frame, public core::Getter<Sidebar> {
public:
	core::Label label[16];
	core::Slider slider[16];
	core::Label header[4];

	Sidebar() :Frame() { set(*this); }
	void onOpening() override;
	void onOpened() override;

	int onLButtonDown(const core::eventInfo& e) override;
	int onLButtonUp(const core::eventInfo& e) override;
	int onResize(const core::eventInfo& e) override;
	void updateUI();
};
