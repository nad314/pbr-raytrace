#pragma once
namespace core {
	struct RenderTask : public Worker::Task {
		PBVH* pbvh;
		simdView* pview;
		~RenderTask() {}
		RenderTask(PBVH* pB, simdView* pW): pbvh(pB), pview(pW) {}
		virtual void preprocess() override;
		virtual void execute(Worker* pWorker) override;
	};
}
