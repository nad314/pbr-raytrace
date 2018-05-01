#pragma once
namespace oven {
	struct imageRenderTask : public Worker::Task {
		static int squareSize;
		static vec2i squares;
		static vec2i current;
		PBVH* pbvh;
		simdView* pview;
		~imageRenderTask() {}
		imageRenderTask(PBVH* pB, simdView* pW);
		virtual void execute(Worker* pWorker) override;
		bool getNextRect(vec4i& rect);
	};
}
