#pragma once

class CoreTest final : public core::Module, public core::SIMD {
private:
	MainWindow wnd;
	core::Timer<float> timer;
	core::Timer<float> globalTimer;

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
