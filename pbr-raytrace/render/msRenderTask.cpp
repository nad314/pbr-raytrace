#include <main> 

namespace core {
	void msRenderTask::execute(Renderer::Worker* pWorker) {
		if (pWorker == NULL)
			return;
		core::Renderer::quickTrace(*pbvh, pview, pWorker->threadNumber, pWorker->threadCount, samples, 8, Storage::get().shader);
	}
}
