#pragma once

class Statusbar final : public oven::Surface, public oven::Debugger, public oven::Getter<Statusbar> {
protected:
	FILE* output;
	char text[256];
	std::mutex iomutex;
public:
	oven::ProgBar progBar;
	Statusbar() :Surface(), Debugger() {
		set(*this);
		oven::Debug::attach(this);
		oven::Debug::enable();
		oven::Path::pushDir();
		oven::Path::goHome();
		output = fopen("log.txt", "w");
		oven::Path::popDir();
	}
	~Statusbar() { fclose(output); }
	void onOpening() override;
	void onOpened() override;

	void onStartPaint(const oven::eventInfo& e) override;
	void onEndPaint(const oven::eventInfo& e) override;
	int onReshape() override;
	void print(const char* s) override;
	void log(const char* s) override;
	void info(const char* s) override;
	void error(const char* s) override;

	static void prog(const float& amount);
};