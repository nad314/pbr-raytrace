#include <main>

Statusbar* core::Getter<Statusbar>::getter = NULL;

void Statusbar::onOpening() {
	Form::onOpening();
	setSize(256, 256);
	setStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	setBackColor(core::Theme::controlBackColor);
	memset(text, 0, 256);
}

void Statusbar::onOpened() {
	Form::onOpened();
	setBackColor(Color(64, 64, 68, 255));
	push(progBar.make(vec4i(0, 0, 200, 20), *this));
	progBar.set(0.5f);
	setControlColors();
}

void Statusbar::onStartPaint(const core::eventInfo& e) {
	__invalidate();
	Form::onStartPaint(e);
	core::Font& f = core::Font::get();
	vec4b c = f.getColor();
	f.setColor(vec4b(200, 200, 200, 255));
	f.print(text, *this, 4, 4);
	f.setColor(c);
}

void Statusbar::onEndPaint(const core::eventInfo& e) {
	Form::onEndPaint(e);
}

int Statusbar::onResize(const core::eventInfo& e) {
	Form::onResize(e);
	const int sidebar = 256;
	progBar.move(Rect(width - sidebar + 3, 2, width - 3, height - 2));
	return 0;
}

void Statusbar::print(const char* s) {
	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
	strcpy(text, s);
	int n = strlen(text);
	for (int i = 0; i < n; ++i)
		if (text[i] == '\n')
			text[i] = ' ';
	invalidate();
	log(s);
}

void Statusbar::log(const char* s) {
	std::lock_guard<std::mutex> lock(iomutex);
	fprintf(output, s);
}

void Statusbar::info(const char* s) {
	std::lock_guard<std::mutex> lock(iomutex);
	fprintf(output, s);
}

void Statusbar::error(const char* s) {
	std::lock_guard<std::mutex> lock(iomutex);
	fprintf(output, s);
}

void Statusbar::prog(const float& amount) {
	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
	get().progBar.set(amount);
}
