#include <main>

void Controller::render() {
	wg->executeLocal();
}

void Controller::getPoint(const float x, const float y) {
	core::simdView& view = (static_cast<RenderWindow*>(parent))->view;
	core::PBVH& bvh = storage->pbvh;

	core::Ray ray = core::Renderer::unproject(view, matrixs(view.mat.inverted()), (float)x, (float)view.img.height - y);

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

void Controller::renderPBRImage() {
	core::Ray ray;
	core::simdView& view = *this->view;
	core::simdImage& img = Storage::get().hdri;
	const matrixs sinv = view.mat.inverted();
	vec4 p;
	for (int i=0;i<view.img.width;++i)
		for (int j = 0; j < view.img.height; ++j) {
			core::Renderer::unproject(ray, view, sinv, (float)i, (float)view.img.height - j);
			(core::envMap(img, ray.sr1)*255.0f).store(p);
			core::Core2D::putPixel(i, j, vec4b((byte)p.x, (byte)p.y, (byte)p.z, (byte)p.w), view.img);
		}
}

void Controller::clearSIMDImage() {
	Storage& data = Storage::get();
	data.renderedSamples = 0;
	data.simdFrame.clear(vec4s(0.0f));
	renderTime = 0.0f;
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
	Storage::get().renderImage = view->img;
	bool done = false;
	//view->clear();
	storage->renderedSamples = Settings::maxSamples;
	core::renderShowTask task(view, 32);
	core::RenderShader shader(*view);
	wg->clearTasks().pushTask<core::imageRenderTask>(&storage->pbvh, view);
	core::Timer<float> t;

	t.start();
	wg->executeLocal();
	t.stop();

	view->img = Storage::get().renderImage;

	veryBusy = 0;
	
	//print message
	char msg[256];
	sprintf(msg, "Frame %d: %s", frameCounter, formatTime((int)t.ms()).c_str());
	core::Debug::info(msg);
	core::Debug::print(msg);
}
