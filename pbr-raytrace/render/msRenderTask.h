#pragma once
namespace core {
	struct msRenderTask : public Worker::Task {
		PBVH* pbvh;
		simdView* pview;
		int samples;
		~msRenderTask() {}
		msRenderTask(PBVH* pB, simdView* pW, const int samp) : pbvh(pB), pview(pW), samples(samp) {}
		virtual void execute(Worker* pWorker) override;
	};
}
