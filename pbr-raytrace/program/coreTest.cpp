#include <main>

int CoreTest::loadData() {
	if (!Controller::get().loadHDRI("../data/hdri/pond_bridge_night_1k.hdr"))
		return 0;
	if (1||controller->benchMode)
		return storage->load("../data/model/pantherUniform.cloud");
	else
		return 0;
}

int CoreTest::onLoad() {
	std::string inst = core::CPUID::vectorInstructions();
	core::Debug::info("CPU: (%dx) %s", std::thread::hardware_concurrency(), core::CPUID::brandName().c_str());
	core::Debug::info("Instructions: %s", inst.c_str());
	if (inst.find("avx") == std::string::npos) {
		core::Debug::error("AVX not supported. Quitting.");
		return 1;
	}

	core::Path::goHome();
	core::Path::mkdir("../capture");
	core::Path::mkdir("../turntable");
	core::RanduinWrynn::construct(1024);
	//core::Debug::enable();
	return 0;
}

int CoreTest::onDispose() {
	GL::deleteContext();
	return 0;
}

int CoreTest::onStart() {
	wnd.open();
	/*GL::setVsync(0);/*
	core::eventInfo e(wnd, WM_PAINT, wnd.width, wnd.height);
	wnd.onPaint(e);*/
	glClear(GL_COLOR_BUFFER_BIT);
	GL::swapBuffers(wnd);
	return 0;
}

int CoreTest::onStop() {
	wnd.close();
	return 0;
}

int CoreTest::main() {
	char text[256];
	bool done = 0;
	globalTimer.start();
	int nframes(0);
	float renderTime(0);

	memset(text, 0, 256);

	RenderWindow& rw = static_cast<RenderWindow&>(wnd.getRenderWindow());
	//if (!rw)done = 1;

	storage = new Storage;
	controller = new Controller(&rw, storage);
	//controller->benchMode = true;

	rw.view.rotation.init();
	rw.view.updateMatrix();

	wnd.setFormTitle("Core Renderer - Ready!");

	if (loadData())
		done = 1;

	//update UI
	Sidebar::get().updateUI();

	//push initial render task
	
	//storage->simdFrame.make(rw.surfaceWidth(), rw.surfaceHeight());
	controller->clearSIMDImage();
	controller->wg->pushTask<core::progRenderTask>(&storage->pbvh, controller->view);
	controller->timer.start();
	
	GL::makeCurrent(wnd);
	//GL::setVsync(0);
	Statusbar::get().prog(0.0f);

	rw.alive = 1;
	rw.drawable = 1;
	bool dirty = 0;
	while (!done) {
		while (wnd.peekMessageAsync(done))
			continue;

		if (controller->valid) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		timer.start();
		//Render Multithreaded
		core::Renderer::invalidate();
		core::RanduinWrynn::mulligan();
		if (storage->pbvh.pointCount > 0 && controller->getMode()!=2)
			controller->render();
		timer.stop();

		if (controller->getMode() == 1) {
			controller->view->rotation.init();
			//controller->view->rotation.rotate(10.0f, 1.0f, 0.0f, 0.0f);
			controller->view->rotation.rotate(controller->benchTimer.stop().s()*30.0f, 0.0f, 1.0f, 0.0f);
			controller->view->updateMatrix();
			controller->wg->pushTask<core::progRenderTask>(&storage->pbvh, controller->view);
			controller->invalidate();
			controller->clearSIMDImage();
		} else if (controller->getMode() == 2) {
			controller->view->rotation.init();
			//controller->view->rotation.rotate(10.0f, 1.0f, 0.0f, 0.0f);
			controller->view->rotation.rotate(1.0f * controller->frameCounter, 0.0f, 1.0f, 0.0f); //1.0 == 30fps
			controller->view->updateMatrix();
			//controller->wg->pushTask<core::imageRenderTask>(&storage->pbvh, controller->view);
			controller->invalidate();
			controller->fullRender();
			if (1 || storage->renderedSamples >= Settings::maxSamples) {
				//save image
				core::Path::goHome();
				core::Image img = rw.image();
				img.flipV();
				char path[256];
				sprintf(path, "../turntable/img-%04d.png", controller->frameCounter);
				img.savePng(path);
				//go on with our lyfe
				controller->clearSIMDImage();
				++controller->frameCounter;	
				if (controller->frameCounter == 360) {
					controller->setMode(0);
					//Settings::maxSamples = 0; // remove this line after tests
					Sidebar::get().updateUI();
				}
			}
		}
		else if ((storage->renderedSamples < Settings::maxSamples || Settings::maxSamples == 0) && controller->timer.stop().ms()>500.0f) {
			controller->wg->pushTask<core::progRenderTask>(&storage->pbvh, controller->view);
			controller->invalidate();
			if (dirty)
				controller->clearSIMDImage();
			dirty = 0;
		}
		else if (controller->timer.stop().ms()>500.0f) {
			controller->validate();
			std::this_thread::sleep_for(std::chrono::microseconds(2000));
		} else dirty = 1;
		
		renderTime += timer;
		controller->renderTime += timer;
		++nframes;

		const float rt = controller->renderTime/1000.0f;	
		if (/*1 || storage->renderedSamples == Settings::maxSamples*/controller->getMode() != 2) {
			if (storage->pbvh.pointCount > 0)
				core::Debug::print("Points: %d - Atom: %.3fmm - Avg: %.2fms - Cur: %.2fms - Samples: %d: %d - Time: %02d:%02d:%02d", 
					storage->cloud.points.count(), sqrt(storage->pbvh.radiusSquared)*1000.0f, renderTime / nframes, timer.ms(), controller->frameCounter, storage->renderedSamples, (int)(rt/3600.0f), (int)(rt/60.0f)%60, (int)(rt)%60);
			Statusbar::get().prog(Settings::maxSamples == 0 ? 0.0f : (float)storage->renderedSamples/ Settings::maxSamples);
			//printf("%.2f\n", timer.ms());
			rw.invalidate();
			//controller->validate();
		}
		
	}
	rw.detach();
	delete controller;
	delete storage;

	return 0;
}


