#pragma once

class MainWindow final : public oven::Form, public oven::Getter<MainWindow> {
private:
	RenderWindow rwnd;
	Statusbar statusbar;
	Sidebar sidebar;
	MenuBar menubar;
public:
	inline oven::Surface& getRenderWindow() { return rwnd; }
	void onOpening() override;
	void onOpened() override;
	void onClosing() override;
	
	int onResize(const oven::eventInfo& e) override;
	int onDropFiles(const oven::eventInfo& e) override;
	void onEndPaint(const oven::eventInfo& e) override;
};

