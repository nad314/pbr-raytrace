#pragma once

class MainWindow final : public core::Form, public core::Getter<MainWindow>, public core::SIMD {
private:
	RenderWindow rwnd;
	Statusbar statusbar;
	Sidebar sidebar;
	MenuBar menubar;
public:
	inline core::Surface& getRenderWindow() { return rwnd; }
	void onOpening() override;
	void onOpened() override;
	void onClosing() override;
	
	int onResize(const core::eventInfo& e) override;
	int onDropFiles(const core::eventInfo& e) override;
	void onEndPaint(const core::eventInfo& e) override;
};

