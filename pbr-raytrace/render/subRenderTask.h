#pragma once
namespace core {
	struct subRenderTask : public Renderer::Worker::Task {
		PBVH* pbvh;
		simdView* pview;
		~subRenderTask() {}
		subRenderTask(PBVH* pB, simdView* pW) : pbvh(pB), pview(pW) {}
		virtual void execute(Renderer::Worker* pWorker) override;
	};
}
