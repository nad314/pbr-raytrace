#include <main>

void RenderWindow::onOpening() {
	setTitle("Core Frame");
	setClass("CoreFrame");
	setStyle(WS_CHILD | WS_VISIBLE);
	setFlags(0);
}

void RenderWindow::onOpened() {
	if (width == 0 || height == 0) 
		return;
	view.make(width, height);
	view.home();
	GL::createContext(*this);
	GL::init(*this);
}

int RenderWindow::onResize(const core::eventInfo& e) {
	Window::onResize(e);
	if (width == 0 || height == 0)
		return e;
	view.make(width, height);
	//view.home();
	view.updateMatrix();
	GL::ortho(*this);

	Controller& c = Controller::get();
	c.makeSIMDImage();
	c.clearSIMDImage();
	c.wg->pushTask<core::msRenderTask>(&c.storage->pbvh, &view, c.samples);
	c.invalidate();
}


void RenderWindow::move(int xw, int yw) {
	const int titleBar = 32;
	const int menuBar = 30;
	const int sideBar = 256;
	const int statusBar = 24;
	const int borderSize = 8;
	xw -= borderSize*2 + sideBar;
	yw -= borderSize*2 + titleBar + menuBar + statusBar;
	MoveWindow(hWnd, borderSize, borderSize + titleBar + menuBar, xw, yw, true);
	width = xw; 
	height = yw;
}


