#include <main>

int CoreTest::loadData() {
	if (1||controller->benchMode)
		return storage->load("data/model/asianDragon.cloud");
	else
		return 0;
}

int CoreTest::onLoad() {
	if (!wnd.goToHomeDirectory())
		return 1;
	//core::Debug::enable();
	return 0;
}

int CoreTest::onDispose() {
	GL::deleteContext();
	return 0;
}

int CoreTest::onStart() {
	wnd.open();
	GL::setVsync(0);
	core::eventInfo e(wnd, WM_PAINT, wnd.width, wnd.height);
	wnd.onPaint(e);
	glClear(GL_COLOR_BUFFER_BIT);
	GL::swapBuffers(wnd.getRenderWindow());
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
	if (!rw)done = 1;

	storage = new Storage;
	controller = new Controller(&rw, storage);
	rw.attach(controller);
	//controller->benchMode = true;

	glClear(GL_COLOR_BUFFER_BIT);
	GL::swapBuffers(rw);

	rw.view.rotation.init();
	rw.view.updateMatrix();

	wnd.setFormTitle("Core Renderer - Ready!");

	if (loadData())
		done = 1;

	//update UI
	Sidebar::get().updateUI();

	//push initial render task
	storage->simdFrame.make(rw.width, rw.height);
	controller->clearSIMDImage();
	controller->wg->pushTask<core::progRenderTask>(&storage->pbvh, controller->view);
	controller->timer.start();
	
	Statusbar::get().prog(0.0f);
	while (!done) {
		if (wnd.peekMessageAsync(done))
			continue;

		if (controller->valid)
			continue;

		//rw.view.clear();
		//controller->renderPBRImage();

		timer.start();
		//Render Multithreaded
		core::Renderer::invalidate();
		if (storage->pbvh.pointCount > 0)
			controller->render();
		timer.stop();


		if (controller->benchMode) {
			controller->view->rotation.init();
			controller->view->rotation.rotate(core::elapsedTime()*50.0f, 0.0f, 1.0f, 0.0f);
			controller->view->updateMatrix();
			controller->wg->pushTask<core::RenderTask>(&storage->pbvh, controller->view);
			controller->invalidate();
		}
		else if ((storage->renderedSamples < Settings::maxSamples || Settings::maxSamples == 0) && controller->timer.stop().ms()>500.0f) {
			controller->wg->pushTask<core::progRenderTask>(&storage->pbvh, controller->view);
			//controller->invalidate();
		}
		else if (controller->timer.stop().ms()>500.0f) {
			controller->validate();
			std::this_thread::sleep_for(std::chrono::microseconds(2000));
		}

		renderTime += timer;
		controller->renderTime += timer;
		++nframes;
		if (storage->pbvh.pointCount > 0)
			core::Debug::print("Points: %d - Atom: %.3fmm - Avg: %.2fms - Cur: %.2fms - Samples: %d - Time: %.2fs", 
				storage->cloud.points.count(), sqrt(storage->pbvh.radiusSquared)*1000.0f, renderTime / nframes, timer.ms(), storage->renderedSamples, controller->renderTime/1000.0f);
		/*
		core::Renderer::print("Left Click to Rotate, Right Click to Pan", rw, 10, rw.height - 26 - 4 * core::Font::get().height());
		core::Renderer::print("Mousewheel to Zoom", rw, 10, rw.height - 22 - 3 * core::Font::get().height());
		core::Renderer::print("F11 to reset camera", rw, 10, rw.height - 18 - 2 * core::Font::get().height());
		core::Renderer::print("F12 to render volumetric", rw, 10, rw.height - 14 - core::Font::get().height());
		*/
		Statusbar::get().prog(Settings::maxSamples == 0 ? 0.0f : (float)storage->renderedSamples/ Settings::maxSamples);

		GL::drawImageInverted(rw);
		GL::swapBuffers(rw);
		
	}
	rw.detach();
	delete controller;
	delete storage;

	return 0;
}


