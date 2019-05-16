#include <main>

void MenuBar::onOpening() {
	Surface::onOpening();
}

void MenuBar::onOpened() {
	Surface::onOpened();
	oven::Image img;
	setBackColor(Color(64, 64, 68, 255));
	setControlBackColor(Color(64, 64, 68, 255));
	setControlBackColorHover(Color(96, 96, 100, 255));
	vec4b hc = vec4b(200, 200, 200, 255);
	vec4b tc = vec4b(200, 200, 200, 255);

	oven::Path::pushDir();
	oven::Path::cd(common::source_directory);
	img.loadPng("data/image/open.png");
	img.linearDownscale(30, 30);
	img.flipV();
	push(button[0].make(nextHorizontal() + vec4i(0, 0, 30, 30), &img, *this, [](oven::Control& c, oven::Surface& f)->void {
		oven::Form* form = dynamic_cast<oven::Form*>(f.getRoot());
		if (!form)
			return;
		std::string path = form->getFileDialog("Cloud\0*.cloud\0OBJ\0*.obj\0\0", 0);
		if (path != "")
			Storage::get().load(path.c_str());
	}));
	button[0].prerender();

	img.loadPng("data/image/solid.png");
	img.linearDownscale(30, 30);
	img.flipV();
	push(button[1].make(nextHorizontal() + vec4i(2, 0, 32, 30), &img, *this, [](oven::Control& c, oven::Surface& f)->void {
		oven::Form* form = dynamic_cast<oven::Form*>(f.getRoot());
		if (!form)
			return;
		std::string path = form->getFileDialog("HDR\0*.hdr\0PNG\0*.png\0\0", 0);
		if (path != "") {
			std::string ext = oven::Path::getExt(path);
			Controller::get().loadHDRI(path);
		}
	}));
	button[1].prerender();

	img.loadPng("data/image/cameraReset.png");
	img.linearDownscale(30, 30);
	img.flipV();
	push(button[2].make(nextHorizontal() + vec4i(16, 0, 46, 30), &img, *this, [](oven::Control& c, oven::Surface& f)->void {
		Controller::get().home();
		Controller::get().invalidate();
	}));
	button[2].prerender();

	img.loadPng("data/image/capture.png");
	img.linearDownscale(30, 30);
	img.flipV();
	push(button[3].make(nextHorizontal() + vec4i(2, 0, 32, 30), &img, *this, [](oven::Control& c, oven::Surface& f)->void {
		oven::Form* form = dynamic_cast<oven::Form*>(f.getRoot());
		if (!form)
			return;
		oven::Image& img = *Storage::get().realtimeImage;
		std::string path = "";
		#ifdef __WIN
		path = form->getFileDialog("PNG\0*.png\0\0", 1);
		#else
		oven::Path::cd(common::source_directory);
		time_t t = std::time(nullptr);
		path = "capture/capture.png"+ std::to_string(t);
		#endif
		if (path != "") {
			path = oven::Path::pushExt("png", path);
			oven::Image simg = img;
			simg.flipV();
			simg.savePng(path.c_str());
		}
	}));
	button[3].prerender();
	oven::Path::popDir();

}

int MenuBar::onLButtonDown(const oven::eventInfo& e) {
	/*
	if (Controller::get().busy)
	return e;*/
	return Surface::onLButtonDown(e);
}

int MenuBar::onLButtonUp(const oven::eventInfo& e) {
	/*
	if (Controller::get().busy)
	return e;*/
	return Surface::onLButtonUp(e);
}

int MenuBar::onResize(const oven::eventInfo& e) {
	Surface::onResize(e);
	return 0;
}
