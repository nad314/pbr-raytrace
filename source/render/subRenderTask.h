#pragma once
namespace oven {
	struct subRenderTask : public Worker::Task {
		PBVH* pbvh;
		simdView* pview;
		~subRenderTask() {}
		subRenderTask(PBVH* pB, simdView* pW) : pbvh(pB), pview(pW) {}
		virtual void execute(Worker* pWorker) override;
	};
}
