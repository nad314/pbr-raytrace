#pragma once
namespace core {
	struct progRenderTask : public Worker::Task, public core::SIMD {
		PBVH* pbvh;
		simdView* pview;
		static std::atomic_int tc;
		~progRenderTask() {}
		progRenderTask(PBVH* pB, simdView* pW) : pbvh(pB), pview(pW) { tc = 0; }
		virtual void execute(Worker* pWorker) override;
	};
}
