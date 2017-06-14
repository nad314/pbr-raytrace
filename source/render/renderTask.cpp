#include <main> 

namespace core {
	void RenderTask::preprocess() {
		core::Renderer::clearImage(pview->img, vec4b((byte)0));
	}

	void RenderTask::execute(Worker* pWorker) {
		if (pWorker == NULL)
			return;	
		core::Renderer::quickTrace(*pbvh, pview, pWorker->threadNumber, pWorker->threadCount, Controller::get().getShader());
	}
}