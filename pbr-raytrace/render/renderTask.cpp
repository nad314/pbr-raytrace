#include <main> 

namespace core {
	void RenderTask::execute(Renderer::Worker* pWorker) {
		if (pWorker == NULL)
			return;
		core::Renderer::quickTrace(*pbvh, pview, pWorker->threadNumber, pWorker->threadCount, 1, 8, Storage::get().shader);
	}
}