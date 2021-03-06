﻿#include <main>

#ifndef __WIN
template<>
#endif
MainWindow* oven::Getter<MainWindow>::getter = NULL;

void MainWindow::onOpening() {
	Form::onOpening();
	setTitle("Core Renderer");
	setSize(1280, 720);
	setMinSize(640, 360);
	addExStyle(WS_EX_ACCEPTFILES);
	set(*this);
}

void MainWindow::onOpened() {
	Form::onOpened();

	push(statusbar.setSurfaceRect(vec4i(3, height - 24, width - 3, height - 4)).openSurface());
	push(sidebar.setSurfaceRect(vec4i(width - 256, 46, width - 8, height - 32)).openSurface());
	push(menubar.setSurfaceRect(vec4i(8, 8, width - 8, 38)).openSurface());

	push(rwnd.setSurfaceRect(vec4i(0,0, width, height)).openSurface());
	rwnd.move(width, height);
}

void MainWindow::onClosing() {
	Form::onClosing();
}

int MainWindow::onResize(const oven::eventInfo &e) {
	Form::onResize(e);
	if (width < 1 || height < 1)
		return e;
	statusbar.moveSurface(vec4i(3, height - 24, width - 3, height - 4));
	sidebar.moveSurface(vec4i(width - 256, 46, width - 8, height - 32));
	menubar.moveSurface(vec4i(8, 8, width - 8, 38));
	rwnd.move(width, height);
	return e;
}

void MainWindow::onEndPaint(const oven::eventInfo& e) {
	Form::onEndPaint(e);
	/*
	oven::Renderer::drawRect(rwnd.getSurfaceRect().expand(1), oven::Color(31, 31, 31, 255), *this);
	oven::Renderer::drawRect(sidebar.getSurfaceRect().expand(1), oven::Color(31, 31, 31, 255), *this);
	*/
}

int MainWindow::onDropFiles(const oven::eventInfo& e) {
	Controller& c = Controller::get();
	std::string path = e.droppedFile();

	if (!c.loadHDRI(path)) {
		Storage::get().load(path.c_str());
		Controller::get().home();
	}
	c.invalidate();
	return e;
}
