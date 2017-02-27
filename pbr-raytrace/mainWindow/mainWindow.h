#pragma once

class MainWindow final : public core::WindowForm, public core::Getter<MainWindow> {
private:
	RenderWindow rwnd;
	Statusbar statusbar;
	Sidebar sidebar;
	MenuBar menubar;
public:
	inline core::Window& getRenderWindow() { return rwnd; }
	void onOpening() override;
	void onOpened() override;
	void onClosing() override;
	
	int onResize(const core::eventInfo& e) override;
	int onDropFiles(const core::eventInfo& e) override;
	void onEndPaint(const core::eventInfo& e) override;
	int onGetMinMaxInfo(const core::eventInfo& e) override;
	int onActivate(const core::eventInfo& e) override;
};

