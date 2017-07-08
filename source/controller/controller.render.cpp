#include <main>

void Controller::render() {
	wg->executeLocal();
}

void Controller::getPoint(const float x, const float y) {
	core::simdView& view = (static_cast<RenderWindow*>(parent))->view;
	core::PBVH& bvh = storage->pbvh;

	core::Ray ray = core::Renderer::unproject(view, view._mm_imvp, (float)x, (float)y);

	static std::pair<int, float> stack[256];
	int* priority = new int[bvh.inner.size()];
	memset(priority, 0, bvh.inner.size() * sizeof(int));

	const float d = bvh.findFirst(ray, stack, priority, false);
	if (d > 0.0f) {
		const vec4s point = ray.sr0 + ray.sr1*vec4s(d);
		point.store(clickPoint);
	}
	else clickPoint = vec4(0.0f);
	delete[] priority;
}


inline std::string formatTime(const int& ms) {
	const int s = ms / 1000;
	const int m = s / 60;

	char ss[256];
	memset(ss, 0, sizeof(ss));
	sprintf(ss, "%02d:%02d:%03dms", m, s%60, ms%1000);

	return std::string(ss);
}

void Controller::fullRender() {
	core::Form* wnd = dynamic_cast<core::Form*>(parent->getRoot());
	if (!wnd)
		return;
	veryBusy = 1;
	RenderWindow& rwnd = dynamic_cast<RenderWindow&>(*parent);
	Storage::get().renderImage = *Storage::get().realtimeImage;
	bool done = false;
	//view->clear();
	storage->renderedSamples = Settings::maxSamples;
	core::RenderShader shader(*view);
	wg->clearTasks().pushTask<core::imageRenderTask>(&storage->pbvh, view);
	core::Timer<float> t;

	t.start();
	wg->executeLocal();
	t.stop();

	*Storage::get().realtimeImage = Storage::get().renderImage;

	veryBusy = 0;
	
	//print message
	char msg[256];
	sprintf(msg, "Frame %d: %s", frameCounter, formatTime((int)t.ms()).c_str());
	core::Debug::info(msg);
	core::Debug::print(msg);
}
