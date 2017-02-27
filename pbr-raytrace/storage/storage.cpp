#include <main>
Storage* core::Getter<Storage>::getter = NULL;

void Storage::dispose() {
	cloud.dispose();
	cloudTree.dispose();
	pbvh.dispose();
}

int Storage::load(const char* path) {
	float rad = -1.0f;
	dispose();
	std::string ext = core::Path::getExt(path);
	for (auto& i : ext)
		i = toupper(i);
	if (ext == "CLOUD") {
		if (!cloud.loadAtomic(path, rad)) {
			cloud.dispose();
			return 1;
		}
	}
	else if (ext == "OBJ") {
		if (!cloud.loadObj(path)) {
			cloud.dispose();
			return 1;
		}
		cloud.normalize();
	}
	else {
		dispose();
		return 1;
	}
	//cloud.normalize();
	if (rad > 0.0f)
		cloudTree.radius = rad;
	else cloudTree.radius = -1.0f;
	cloudTree.build(cloud);
	pbvh.build(cloudTree);
	if (rad < 0.0f) {
		rad = pbvh.estimateRadius();
		pbvh.setRadius(rad);
	} else pbvh.radiusSquared = rad*rad;
	cloudTree.dispose();

	char title[256];
	sprintf(title, "Core Renderer - %c%s%c", 39, core::Path::getFileName(path).c_str(), 39);
	MainWindow::get().setFormTitle(title);

	Controller& controller = Controller::get();
	//controller.view->home();
	//controller.view->updateMatrix();
	controller.home();
	controller.invalidate();
	return 0;
}
