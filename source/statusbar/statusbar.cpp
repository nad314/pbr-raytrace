#include <main>

#ifndef __WIN
template<>
#endif
Statusbar* oven::Getter<Statusbar>::getter = NULL;

void Statusbar::onOpening() {
	Surface::onOpening();
	setBackColor(oven::Theme::controlBackColor);
	memset(text, 0, 256);
}

void Statusbar::onOpened() {
	Surface::onOpened();
	setBackColor(Color(64, 64, 68, 255));
	const int sidebar = 256;
	Rect r = Rect(surfaceWidth() - sidebar + 3, 2, surfaceWidth() - 3, surfaceHeight() - 2);
	push(progBar.make(r, *this));
	progBar.set(0.5f);
	setControlColors();
}

void Statusbar::onStartPaint(const oven::eventInfo& e) {
	__invalidate();
	Surface::onStartPaint(e);
	oven::Font& f = oven::Font::get();
	vec4b c = f.getColor();
	Color clr = vec4b(200, 200, 200, 255);
	f.setColor(clr);
	f.print(text, *this, 4, 4);
	f.setColor(c);
}

void Statusbar::onEndPaint(const oven::eventInfo& e) {
	Surface::onEndPaint(e);
}

int Statusbar::onReshape() {
	const int sidebar = 256;
	Rect r = getSurfaceDim();
	progBar.move(Rect(r.z - sidebar + 3, 2, r.z - 3, r.w - 2));
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
}

void Statusbar::log(const char* s) {
	std::lock_guard<std::mutex> lock(iomutex);
	fprintf(output, "%s", s);
	printf("%s\n", s);
}

void Statusbar::info(const char* s) {
	std::lock_guard<std::mutex> lock(iomutex);
	printf("%s\n", s);
}

void Statusbar::error(const char* s) {
	std::lock_guard<std::mutex> lock(iomutex);
	fprintf(output, "%s", s);
}

void Statusbar::prog(const float& amount) {
	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
	get().progBar.set(amount);
	get().invalidate();
}
