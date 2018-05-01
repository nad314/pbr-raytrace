#include <main>

#ifndef __WIN
template<>
#endif
Controller* oven::Getter<Controller>::getter = NULL;

Controller::Controller(oven::RenderSurface* p, Storage* st) {
	set(*this);
	storage = st;
	parent = p;
	p->attach(this);
	view = &(static_cast<RenderWindow*>(parent))->view;
	samples = 2;
	wg = new oven::WorkerGroup();
	makeSIMDFrame();
	timer.start();
}

int Controller::onLButtonDown(const oven::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || dragging || benchMode)
		return e;
	rotating = 1;
	getPoint(e.x(), e.y());
	mouse.x = e.x();
	mouse.y = e.y();
	invalidate();
	parent->setCapture();
	return e;
}

int Controller::onLButtonUp(const oven::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || benchMode)
		return e;
	if (rotating || dragging)
		clearSIMDFrame();
	rotating = 0;
	parent->releaseCapture();
	return e;
}

int Controller::onRButtonDown(const oven::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || rotating || benchMode)
		return e;
	dragging = 1;
	getPoint(e.x(), e.y());
	mouse.x = e.x();
	mouse.y = e.y();
	invalidate();
	parent->setCapture();
	return e;
}

int Controller::onRButtonUp(const oven::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || benchMode)
		return e;
	if (rotating || dragging)
		clearSIMDFrame();
	dragging = 0;
	parent->releaseCapture();
	return e;
}

int Controller::onMousewheel(const oven::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || benchMode)
		return e;
	if (e.delta() < 0) {
		if (view->fov > 41.5f)
			view->fov += 2.0f;
		else view->fov *= 1.2f;
	}
	else if (view->fov > 41.5f)
		view->fov -= 2.0f;
	else view->fov /= 1.2f;
	view->updateProjectionMatrix(*(Storage::get().realtimeImage));
	view->updateMatrix();
	wg->pushTask<oven::subRenderTask>(&storage->pbvh, view);
	clearSIMDFrame();
	invalidate();
	timer.start();
	return e;
}

int Controller::onMouseMove(const oven::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || benchMode)
		return e;
	if (rotating) {
		clickPoint.w = 1.0f;
		vec4 point = view->left*clickPoint;

		view->left.translate(-point.x, -point.y, -point.z);
		view->left.rotate(0.2f*(e.x() - mouse.x), 0.0f, 1.0f, 0.0f);
		view->left.rotate(0.2f*(e.y() - mouse.y), 1.0f, 0.0f, 0.0f);
		view->left.translate(point.x, point.y, point.z);
		
		view->updateMatrix();
		invalidate();
		wg->clearTasks();
		wg->pushTask<oven::subRenderTask>(&storage->pbvh, view);
		//clearSIMDFrame();
		timer.start();
	}
	else if (dragging) {
		vec4 d;
		const matrixf rot = view->left.normalMatrix().inverted();
		vec4 plane = (rot*vec4(0.0f, 0.0f, 1.0f, 1.0f)).normalize3d();
		plane.w = -oven::Math::dot3(plane, clickPoint);

		oven::Ray ray = getRay((float)e.x(), (float)e.y());
		oven::Ray lray = getRay((float)mouse.x, (float)mouse.y);
		vec4s t0 = oven::SSE::rayPlaneT(ray.sr0, ray.sr1, vec4s(plane));
		vec4s t1 = oven::SSE::rayPlaneT(lray.sr0, lray.sr1, vec4s(plane));
		vec4s delta = (ray.sr0 + ray.sr1*t0) - (lray.sr0 + lray.sr1*t1);
		delta = matrixs(rot.inverted())*delta;


		delta.store(d);
		matrixf mat;
		mat.translate(d.x, d.y, d.z);
		view->right = mat*view->right;
		view->updateMatrix();
		wg->clearTasks();
		wg->pushTask<oven::subRenderTask>(&storage->pbvh, view);
		invalidate();
		//clearSIMDFrame();
		timer.start();
	}
	mouse = oven::vec2i(e.x(), e.y());
	return e;
}

int Controller::onKeyDown(const oven::eventInfo& e) {
	if (benchMode)
		return e;
	//add keyboard input
	return e;
}

void Controller::home() {
	view->home();
	view->right.translate(0.0f, 0.0f, -2.5f);
	view->updateMatrix();
	view->updateProjectionMatrix(*Storage::get().realtimeImage);
	clearSIMDFrame();
}

void Controller::makeSIMDFrame() {
	Storage& data = Storage::get();
	if (parent)
		data.simdFrame.make(parent->surfaceWidth(), parent->surfaceHeight());
	clearSIMDFrame();
}

void Controller::clearSIMDFrame() {
	Storage& data = Storage::get();
	data.renderedSamples = 0;
	data.simdFrame.clear(vec4s(0.0f));
	renderTime = 0.0f;
}

void Controller::setMode(const int& m){
	frameCounter = 0;
	if (!veryBusy) {
		clearSIMDFrame();
		wg->clearTasks();
	}
	if (m < 0 || m > 2)
		return;
	mode = m;
	switch (mode) {
		case 0: benchMode = 0; break;
		case 1: benchMode = 1; benchTimer.start(); break;
		case 2: benchMode = 1; benchTimer.start(); break;
		default: benchMode = 0; break;
	}
}


bool Controller::loadHDRI(const std::string& path) {
	std::string ext = oven::Path::getExt(path);
	Storage& data = Storage::get();

	if (ext == "hdr") {
		if (data.hdri.loadHDR(path.c_str())) {
			data.hdri.tonemap();
			makeMipmaps();
			makePreconvolvedImage();

		}
		clearSIMDFrame();
		invalidate();
		return 1;
	}
	else if (ext == "png"){
		data.hdri.loadPNG(path.c_str());
		makeMipmaps();
		makePreconvolvedImage();

		clearSIMDFrame();
		invalidate();
		return 1;
	} else return 0;
}

void Controller::makeMipmaps() {
	_CORE_PROFILE
	Storage& data = Storage::get();
	data.hdriMipmap[0].makeMipmap(data.hdri);
	for (int i = 1; i < 8; ++i)
		data.hdriMipmap[i].makeMipmap(data.hdriMipmap[i-1]);
}

void Controller::makePreconvolvedImage() {
	_CORE_PROFILE
	Storage& data = Storage::get();
	int i = 0;
	while (i<7 && data.hdriMipmap[i].width > 128)
		++i;
	data.hdriDiff = data.hdriMipmap[i];
	data.hdriDiff.preconvolveByAngle(90.0f);	
}
