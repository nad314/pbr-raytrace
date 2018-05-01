#pragma once

class CoreTest final : public oven::Module, public oven::SIMD {
private:
	MainWindow wnd;
	oven::Timer<float> timer;
	oven::Timer<float> globalTimer;

	Storage* storage;
	Controller* controller;

public:
	int onLoad() override;
	int onDispose() override;
	int loadData();

	int onStart() override;
	int onStop() override;
	int main() override;
};
