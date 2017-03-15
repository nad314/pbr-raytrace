#include <main>

#ifndef __WIN
template<>
#endif
MainWindow* core::Getter<MainWindow>::getter = NULL;

void MainWindow::onOpening() {
	Form::onOpening();
	setTitle("Core Renderer");
	setSize(900, 616);
	setMinSize(900, 616);
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

int MainWindow::onResize(const core::eventInfo &e) {
	Form::onResize(e);
	if (width < 1 || height < 1)
		return e;
	statusbar.moveSurface(vec4i(3, height - 24, width - 3, height - 4));
	sidebar.moveSurface(vec4i(width - 256, 46, width - 8, height - 32));
	menubar.moveSurface(vec4i(8, 8, width - 8, 38));
	rwnd.move(width, height);
	return e;
}

void MainWindow::onEndPaint(const core::eventInfo& e) {
	Form::onEndPaint(e);
	/*
	core::Renderer::drawRect(rwnd.getSurfaceRect().expand(1), core::Color(31, 31, 31, 255), *this);
	core::Renderer::drawRect(sidebar.getSurfaceRect().expand(1), core::Color(31, 31, 31, 255), *this);
	*/
}

int MainWindow::onDropFiles(const core::eventInfo& e) {
	std::string path = e.droppedFile();
	std::string ext = core::Path::getExt(path);

	if (ext == "hdr") {
		if (Storage::get().volumetricShader.hdri.loadHDR(path.c_str()))
			Storage::get().volumetricShader.hdri.tonemap();
		Controller::get().clearSIMDImage();
		Controller::get().invalidate();
	}
	else if (ext == "png"){
		Storage::get().volumetricShader.hdri.loadPNG(path.c_str());
		Controller::get().clearSIMDImage();
		Controller::get().invalidate();
	} else Storage::get().load(path.c_str());

	rwnd.view.home();
	Controller& c = Controller::get();
	c.invalidate();
	return e;
}
