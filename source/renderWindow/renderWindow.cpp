#include <main>

void RenderWindow::onOpening() {
	RenderSurface::onOpening();
}

void RenderWindow::onOpened() {
	RenderSurface::onOpened();
	const int width = surfaceWidth();
	const int height = surfaceHeight();
	if (width == 0 || height == 0) 
		return;
	view.make(width, height);
	view.home();
}

int RenderWindow::onReshape() {
	//RenderSurface::onReshape();
	const int width = surfaceWidth();
	const int height = surfaceHeight();
	if (width == 0 || height == 0)
		return 0;
	core::Renderer::clearImage(*this, core::vec4b(51, 51, 51, 255));
	view.make(width, height);
	view.updateMatrix();

	if (!alive)
		return 0;
	Controller& c = Controller::get();
	c.makeSIMDFrame();
	c.invalidate();
}


void RenderWindow::move(int xw, int yw) {
	const int titleBar = 8;
	const int menuBar = 30;
	const int sideBar = 256;
	const int statusBar = 24;
	const int borderSize = 8;
	xw -= borderSize + sideBar;
	yw -= borderSize + statusBar;

	Rect r = vec4i(borderSize, borderSize + titleBar + menuBar, xw, yw);
	//setSurfaceRect(r);
	//onReshape();

	//RenderSurface::resizeSurface(r);
	//invalidate();
	if (alive)
		Controller::get().frameCounter = 0;
	RenderSurface::moveSurface(r);
}


