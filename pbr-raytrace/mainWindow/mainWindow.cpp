#include <main>

MainWindow* core::Getter<MainWindow>::getter = NULL;

void MainWindow::onOpening() {
	WindowForm::onOpening();
	setTitle("Core Renderer");
	setSize(900, 640);
	addExStyle(WS_EX_ACCEPTFILES);
	set(*this);
}

void MainWindow::onOpened() {
	WindowForm::onOpened();

	rwnd.setParent(this);
	rwnd.open();
	core::vec4i r = getClientRect();
	rwnd.move(r.z - r.x, r.w - r.y);

	statusbar.setParent(this);
	statusbar.open();
	statusbar.move(vec4i(3, height - 24, width - 3, height - 4));

	sidebar.setParent(this);
	sidebar.open();
	sidebar.move(vec4i(width - 256, 70, width - 8, height - 32));

	menubar.setParent(this);
	menubar.open();
	menubar.move(vec4i(8, 32, width - 8, 62));

	SetFocus(rwnd);
}

void MainWindow::onClosing() {
	WindowForm::onClosing();
	menubar.close();
	sidebar.close();
	statusbar.close();
	rwnd.close();
}

int MainWindow::onResize(const core::eventInfo &e) {
	WindowForm::onResize(e);
	if (width < 1 || height < 1)
		return e;
	core::vec4i r = getClientRect();
	rwnd.move(r.z-r.x, r.w-r.y);
	statusbar.move(vec4i(3, height - 24, width - 3, height - 4));
	sidebar.move(vec4i(width - 256, 70, width - 8, height - 32));
	menubar.move(vec4i(8, 32, width - 8, 62));
	return e;
}

void MainWindow::onEndPaint(const core::eventInfo& e) {
	WindowForm::onEndPaint(e);
	/*
	core::Renderer::drawRect(rwnd.getChildRect().expand(1), core::Color(31, 31, 31, 255), *this);
	core::Renderer::drawRect(sidebar.getChildRect().expand(1), core::Color(31, 31, 31, 255), *this);
	*/
}

int MainWindow::onGetMinMaxInfo(const core::eventInfo& e) {
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)e.lP;
	lpMMI->ptMinTrackSize.x = 900;
	lpMMI->ptMinTrackSize.y = 640;
	return e;
}


int MainWindow::onDropFiles(const core::eventInfo& e) {
	char path[256];
	if (DragQueryFileA((HDROP)e.wP, 0, path, 256)) {
		DragFinish((HDROP)e.wP);
		Storage::get().load(path);
		rwnd.view.home();
		Controller& c = Controller::get();
		c.wg->pushTask<core::msRenderTask>(&c.storage->pbvh, &rwnd.view, c.samples);
		c.invalidate();
	}
	return e;
}

int MainWindow::onActivate(const core::eventInfo& e) {
	WindowForm::onActivate(e);
	if (e.wP != 0 && rwnd.isOpened()) {
		SetForegroundWindow(rwnd);
		SetActiveWindow(rwnd);
		SetFocus(rwnd);
	}
	return 0;
}
