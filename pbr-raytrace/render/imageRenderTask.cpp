#include <main>
namespace core {
#ifndef __WIN
template<>
#endif
	renderShowTask* Getter<renderShowTask>::getter = NULL;

	renderShowTask::renderShowTask(simdView* view, int nthreads) {
		std::lock_guard<std::mutex> lk(mutex);
		set(*this);
		img = view->img;
		threadSquare = new vec2i[nthreads];
		nt = nthreads;
	}


	void renderShowTask::onStartNode(simdView* view, vec2i cStep, vec2i nStep, int square) {
		{
			std::lock_guard<std::mutex> lk(mutex);
			Renderer::drawRect(vec4i(cStep.x*square, cStep.y*square, std::min((cStep.x + 1)*square, (int)img.width), std::min((cStep.y + 1)*square, (int)img.height)), vec4b(255, 255, 120, 255), img);
		}
		cv.notify_one();
	}

	void renderShowTask::onEndNode(simdView* view, vec2i cStep, vec2i nStep, int square) {
		{
			std::lock_guard<std::mutex> lk(mutex);
			const int x = cStep.x*square;
			for (int j = cStep.y*square; j < std::min(cStep.y*square + square, (int)img.height); ++j)
					memcpy(reinterpret_cast<int*>(img.data) + j*img.width + x, reinterpret_cast<int*>(view->img.data) + j*img.width + x, sizeof(int)*square);
		}
		cv.notify_one();
	}

	void imageRenderTask::execute(Renderer::Worker* pWorker) {
		if (pWorker == NULL)
			return;
		core::Renderer::quickTrace(*pbvh, pview, pWorker->threadNumber, pWorker->threadCount, samples, 32, &renderShowTask::get(), *shader);
	}
}
