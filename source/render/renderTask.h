#pragma once
namespace oven {
	struct RenderTask : public Worker::Task {
		PBVH* pbvh;
		simdView* pview;
		~RenderTask() {}
		RenderTask(PBVH* pB, simdView* pW): pbvh(pB), pview(pW) {}
		virtual void execute(Worker* pWorker) override;
	};
}
