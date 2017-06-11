#include <main>

#ifndef __WIN
template<>
#endif
Sidebar* core::Getter<Sidebar>::getter = NULL;

void Sidebar::onOpening() {
	Surface::onOpening();
}

void Sidebar::onOpened() {
	Surface::onOpened();
	setBackColor(Color(64, 64, 68, 255));
	char t[256];
	vec4b hc = vec4b(200, 200, 200, 255);
	vec4b tc = vec4b(200, 200, 200, 255);

	push(header[0].make(nextVertical() + vec4i(8, 20, 240, 32), "- Settings -", *this).setColor(hc).setAlign(1));
	push(label[9].make(nextVertical() + vec4i(8, 8, 240, 20), "Shader", *this).setColor(tc));
	push(slider[9].make(nextVertical() + vec4i(8, 2, 240, 22), 3, *this, [](float pos, core::Control& c, core::Surface& f)->void {	
		int p = (int)(pos*2.0f);
		Controller& ct = Controller::get();
		if (ct.veryBusy)
			return;
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		if (ct.getShaderID() != p) {
			switch(p) {
				case 0: sb.label[9].setText("Shader: Realtime"); break;
				case 1: sb.label[9].setText("Shader: PBS"); break;
				case 2: sb.label[9].setText("Shader: PBR"); break;
				default: break;
			}
			sb.label[9].invalidate();
			ct.setShader(p);
			ct.clearSIMDImage();
			ct.invalidate();
			ct.frameCounter = 0;
			ct.wg->clearTasks();
		}
	}).setPos((float)0.0f));
	push(label[10].make(nextVertical() + vec4i(8, 8, 240, 20), "Mode", *this).setColor(tc));
	push(slider[10].make(nextVertical() + vec4i(8, 2, 240, 22), 3, *this, [](float pos, core::Control& c, core::Surface& f)->void {	
		int p = (int)(pos*2.0f);
		Controller& ct = Controller::get();
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		if (ct.getMode() != p) {
			switch(p) {
				case 0: sb.label[10].setText("Mode: Free"); break;
				case 1: sb.label[10].setText("Mode: Benchmark"); break;
				case 2: sb.label[10].setText("Mode: Turntable"); break;
				default: break;
			}
			sb.label[10].invalidate();
			ct.setMode(p);
			if (!ct.veryBusy) {
				ct.clearSIMDImage();
				ct.invalidate();
				ct.wg->clearTasks();
			}
			//ct.frameCounter = 0;
		}
	}).setPos((float)0.0f));


	push(label[0].make(nextVertical() + vec4i(8, 8, 240, 20), "Samples: 128", *this).setColor(tc));
	push(slider[0].make(nextVertical() + vec4i(8, 2, 240, 22), 0, *this, [](float pos, core::Control& c, core::Surface& f)->void {
		if (Controller::get().veryBusy)
			return;
		int samples = (int)(Settings::maxSampleCap * pos);
		if (samples < Storage::get().renderedSamples) {
			Controller::get().clearSIMDImage();
			Controller::get().frameCounter = 0;
		}
		Settings::maxSamples = samples;
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		char t[256];
		sprintf(t, "Samples: %d", samples);
		if (samples == 0)
			strcat(t, " (Infinite Render)");
		sb.label[0].setText(t).invalidate();
		sb.__invalidate();
		Controller::get().invalidate();		
	}).setPos((float)128.0f/Settings::maxSampleCap));

	sprintf(t, "Threads: %d", std::thread::hardware_concurrency());
	push(label[1].make(nextVertical() + vec4i(8, 4, 200, 16), t, *this).setColor(tc));
	push(slider[1].make(nextVertical() + vec4i(8, 2, 240, 22), std::thread::hardware_concurrency(), *this, [](float pos, core::Control& c, core::Surface& f)->void {
		if (Controller::get().veryBusy)
			return;
		int threads = (int)((float)(std::thread::hardware_concurrency()-1) * pos) + 1;
		if (threads == Controller::get().wg->workerCount())
			return;
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		char t[256];
		sprintf(t, "Threads: %d", threads);
		sb.label[1].setText(t).invalidate();
		sb.__invalidate();
		delete Controller::get().wg;
		Controller::get().wg = new core::WorkerGroup(threads);
	}).setPos(1.0f));

	sprintf(t, "Light Bounces: %d", std::thread::hardware_concurrency());
	push(label[8].make(nextVertical() + vec4i(8, 4, 200, 16), t, *this).setColor(tc));
	push(slider[8].make(nextVertical() + vec4i(8, 2, 240, 22), 7, *this, [](float pos, core::Control& c, core::Surface& f)->void {
		if (Controller::get().veryBusy)
			return;
		int bounces = (int)(7.0f * pos) + 1;
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		char t[256];
		sprintf(t, "Light Bounces: %d", bounces);
		sb.label[8].setText(t).invalidate();
		sb.__invalidate();
		Settings::maxBounces = bounces;
		Controller::get().clearSIMDImage();
		Controller::get().invalidate();
		Controller::get().frameCounter = 0;
	}).setPos(1.0f));


	push(header[1].make(nextVertical() + vec4i(8, 28, 240, 40), "- Material -", *this).setColor(hc).setAlign(1));
	push(label[2].make(nextVertical() + vec4i(8, 8, 240, 20), "Roughness", *this).setColor(tc));
	push(slider[2].make(nextVertical() + vec4i(8, 2, 240, 22), 0, *this, [](float pos, core::Control& c, core::Surface& f)->void {
		if (Controller::get().veryBusy)
			return;
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		char t[256];
		sprintf(t, "Roughness: %.4f", pos);
		sb.label[2].setText(t).invalidate();
		sb.__invalidate();
		Storage::get().material.roughness = pos;
		Controller::get().clearSIMDImage();
		Controller::get().invalidate();
		Controller::get().frameCounter = 0;
	}));

	push(label[3].make(nextVertical() + vec4i(8, 4, 240, 16), "Metallic", *this).setColor(tc));
	push(slider[3].make(nextVertical() + vec4i(8, 2, 240, 22), 0, *this, [](float pos, core::Control& c, core::Surface& f)->void {
		if (Controller::get().veryBusy)
			return;
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		char t[256];
		sprintf(t, "Metallic: %.4f", pos);
		sb.label[3].setText(t).invalidate();
		sb.__invalidate();
		Storage::get().material.metallic = pos;
		Controller::get().clearSIMDImage();
		Controller::get().invalidate();
		Controller::get().frameCounter = 0;
	}));

	push(header[2].make(nextVertical() + vec4i(8, 28, 240, 40), "- Color -", *this).setColor(hc).setAlign(1));
	push(label[4].make(nextVertical() + vec4i(8, 8, 240, 20), "Red:", *this).setColor(tc));
	push(slider[4].make(nextVertical() + vec4i(8, 2, 240, 22), 0, *this, [](float pos, core::Control& c, core::Surface& f)->void {
		if (Controller::get().veryBusy)
			return;
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		Storage& data = Storage::get();
		char t[256];
		sprintf(t, "Red: %.4f", pos);
		sb.label[4].setText(t).invalidate();
		sb.__invalidate();
		vec4 color;
		data.material.base.store(color);
		color.x = pos;
		data.material.base = color;
		Controller::get().clearSIMDImage();
		Controller::get().invalidate();
		Controller::get().frameCounter = 0;
	}));

	push(label[5].make(nextVertical() + vec4i(8, 4, 240, 16), "Green:", *this).setColor(tc));
	push(slider[5].make(nextVertical() + vec4i(8, 2, 240, 22), 0, *this, [](float pos, core::Control& c, core::Surface& f)->void {
		if (Controller::get().veryBusy)
			return;
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		Storage& data = Storage::get();
		char t[256];
		sprintf(t, "Green: %.4f", pos);
		sb.label[5].setText(t).invalidate();
		sb.__invalidate();
		vec4 color;
		data.material.base.store(color);
		color.y = pos;
		data.material.base = color;
		Controller::get().clearSIMDImage();
		Controller::get().invalidate();
		Controller::get().frameCounter = 0;
	}));

	push(label[6].make(nextVertical() + vec4i(8, 4, 240, 16), "Blue:", *this).setColor(tc));
	push(slider[6].make(nextVertical() + vec4i(8, 2, 240, 22), 0, *this, [](float pos, core::Control& c, core::Surface& f)->void {
		if (Controller::get().veryBusy)
			return;
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		Storage& data = Storage::get();
		char t[256];
		sprintf(t, "Blue: %.4f", pos);
		sb.label[6].setText(t).invalidate();
		sb.__invalidate();
		vec4 color;
		data.material.base.store(color);
		color.z = pos;
		data.material.base = color;
		Controller::get().clearSIMDImage();
		Controller::get().invalidate();
		Controller::get().frameCounter = 0;
	}));

	push(header[3].make(nextVertical() + vec4i(8, 28, 240, 40), "- Environment -", *this).setColor(hc).setAlign(1));
	push(label[7].make(nextVertical() + vec4i(8, 8, 240, 20), "Strength:", *this).setColor(tc));
	push(slider[7].make(nextVertical() + vec4i(8, 2, 240, 22), 0, *this, [](float pos, core::Control& c, core::Surface& f)->void {
		if (Controller::get().veryBusy)
			return;
		Sidebar& sb = dynamic_cast<Sidebar&>(f);
		Storage& data = Storage::get();
		float intensity = pos * Settings::maxEnvironmentStrenght;
		Settings::environmentStrength = intensity;
		char t[256];
		sprintf(t, "Strength: %.4f", intensity);
		sb.label[7].setText(t).invalidate();
		sb.__invalidate();
		Controller::get().clearSIMDImage();
		Controller::get().invalidate();
		Controller::get().frameCounter = 0;
	}));

	setControlColors();
}

int Sidebar::onLButtonDown(const core::eventInfo& e) {
	/*
	if (Controller::get().busy)
		return e;*/
	return Surface::onLButtonDown(e);
}

int Sidebar::onLButtonUp(const core::eventInfo& e) {
	/*
	if (Controller::get().busy)
		return e;*/
	return Surface::onLButtonUp(e);
}

int Sidebar::onResize(const core::eventInfo& e) {
	Surface::onResize(e);
	Rect fullRect = getSurfaceDim() + Rect(0, 24, 0, 0);
	return 0;
}

void Sidebar::updateUI() {
	Storage& data = Storage::get();
	Controller& c = Controller::get();
	char t[256]; //text
	vec4 v;
	memset(t, 0, sizeof(t));

	sprintf(t, "Samples: %d", Settings::maxSamples);
	label[0].setText(t);
	slider[0].setPos((float)Settings::maxSamples/Settings::maxSampleCap);

	sprintf(t, "Threads: %d", c.wg->workerCount());
	label[1].setText(t);
	slider[1].setPos((float)c.wg->workerCount() / std::thread::hardware_concurrency());

	sprintf(t, "Roughness: %.4f", data.material.roughness);
	label[2].setText(t);
	slider[2].setPos(data.material.roughness);

	sprintf(t, "Metallic: %.4f", data.material.metallic);
	label[3].setText(t);
	slider[3].setPos(data.material.metallic);

	data.material.base.store(v);
	sprintf(t, "Red: %.4f", v.x);
	label[4].setText(t);
	slider[4].setPos(v.x);
	sprintf(t, "Green: %.4f", v.y);
	label[5].setText(t);
	slider[5].setPos(v.y);
	sprintf(t, "Blue: %.4f", v.z);
	label[6].setText(t);
	slider[6].setPos(v.z);

	sprintf(t, "Strength: %.4f", Settings::environmentStrength);
	label[7].setText(t);
	slider[7].setPos(Settings::environmentStrength/Settings::maxEnvironmentStrenght);

	sprintf(t, "Light Bounces: %d", Settings::maxBounces);
	label[8].setText(t);
	slider[8].setPos((float)(Settings::maxBounces - 1)/7.0f);

	switch(Controller::get().getShaderID()) {
		case 0: label[9].setText("Shader: Realtime"); break;
		case 1: label[9].setText("Shader: PBS"); break;
		case 2: label[9].setText("Shader: PBR"); break;
		default: break;
	}
	slider[9].setPos((float)Controller::get().getShaderID()/2.0f);

	switch(Controller::get().getMode()) {
		case 0: label[10].setText("Mode: Free"); break;
		case 1: label[10].setText("Mode: Benchmark"); break;
		case 2: label[10].setText("Mode: Turntable"); break;
		default: break;
	}
	slider[10].setPos((float)Controller::get().getMode()/2.0f);

	for (int i=0;i<11;++i)
		label[i].invalidate();
	__invalidate();
}