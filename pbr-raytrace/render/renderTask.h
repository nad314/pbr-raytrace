#pragma once
namespace core {
	struct RenderTask : public Renderer::Worker::Task {
		PBVH* pbvh;
		simdView* pview;
		~RenderTask() {}
		RenderTask(PBVH* pB, simdView* pW): pbvh(pB), pview(pW) {}
		virtual void execute(Renderer::Worker* pWorker) override;
	};
}
