#pragma once

class Sidebar final : public oven::Frame, public oven::Getter<Sidebar> {
public:
	oven::Label label[16];
	oven::Slider slider[16];
	oven::Label header[4];

	Sidebar() :Frame() { set(*this); }
	void onOpening() override;
	void onOpened() override;

	int onLButtonDown(const oven::eventInfo& e) override;
	int onLButtonUp(const oven::eventInfo& e) override;
	int onResize(const oven::eventInfo& e) override;
	void updateUI();
};
