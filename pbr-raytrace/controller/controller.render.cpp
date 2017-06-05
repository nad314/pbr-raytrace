#include <main>

void Controller::render() {
	wg->executeLocal();
}

void Controller::getPoint(const float x, const float y) {
	core::simdView& view = (static_cast<RenderWindow*>(parent))->view;
	core::PBVH& bvh = storage->pbvh;
	matrixf inv = view.mat;
	inv.invert();
	matrixs sinv = inv;

	core::Ray ray;
	const float h = (float)view.img.height;

	ray.sr0 = sinv*view.unproject(vec4s(vec4(x, (float)h - y, 0.0f, 1.0f)));
	ray.sr0 /= _mm_permute_ps(ray.sr0, 0b11111111);
	ray.sr1 = sinv*view.unproject(vec4s(vec4(x, (float)h - y, 1.0f, 1.0f)));
	ray.sr1 /= _mm_permute_ps(ray.sr1, 0b11111111);
	ray.sr1 = (ray.sr1 - ray.sr0);
	ray.sr1 /= _mm_sqrt_ps(_mm_dp_ps(ray.sr1, ray.sr1, 0x7F));
	ray.sinvr1 = _mm_rcp_ps(ray.sr1);

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

core::Ray Controller::getRay(const float x, const float y) const {
	core::simdView& view = *this->view;
	matrixf inv = view.mat;
	inv.invert();
	matrixs sinv = inv;

	core::Ray ray;
	const float h = (float)view.img.height;

	ray.sr0 = sinv*view.unproject(vec4s(vec4(x, (float)h - y, 0.0f, 1.0f)));
	ray.sr0 /= _mm_permute_ps(ray.sr0, 0b11111111);
	ray.sr1 = sinv*view.unproject(vec4s(vec4(x, (float)h - y, 1.0f, 1.0f)));
	ray.sr1 /= _mm_permute_ps(ray.sr1, 0b11111111);
	ray.sr1 = (ray.sr1 - ray.sr0);
	ray.sr1 /= _mm_sqrt_ps(_mm_dp_ps(ray.sr1, ray.sr1, 0x7F));
	ray.sinvr1 = _mm_rcp_ps(ray.sr1);

	return ray;
}

void Controller::renderPBRImage() {
	core::Ray ray;
	core::simdView& view = *this->view;
	const float h = (float)view.img.height;
	matrixf inv = view.mat;
	inv.invert();
	matrixs sinv = inv;
	core::simdImage& img = Storage::get().volumetricShader.hdri;
	vec4 p;
	for (int i=0;i<view.img.width;++i)
		for (int j = 0; j < view.img.height; ++j) {
			ray.sr0 = sinv*view.unproject(vec4s(vec4((float)i, (float)h - j, 0.0f, 1.0f)));
			ray.sr0 /= _mm_permute_ps(ray.sr0, 0b11111111);
			ray.sr1 = sinv*view.unproject(vec4s(vec4((float)i, (float)h - j, 1.0f, 1.0f)));
			ray.sr1 /= _mm_permute_ps(ray.sr1, 0b11111111);
			ray.sr1 = (ray.sr1 - ray.sr0);
			ray.sr1 /= _mm_sqrt_ps(_mm_dp_ps(ray.sr1, ray.sr1, 0x7F));
			ray.sinvr1 = _mm_rcp_ps(ray.sr1);
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
	core::Debug::info("Frame %d: %s", frameCounter, formatTime((int)t.ms()).c_str());
}
