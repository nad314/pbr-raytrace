#pragma once
namespace core {
	struct imageRenderTask : public Worker::Task {
		static int squareSize;
		static vec2i squares;
		static vec2i current;
		PBVH* pbvh;
		simdView* pview;
		~imageRenderTask() {}
		imageRenderTask(PBVH* pB, simdView* pW) : pbvh(pB), pview(pW) {
			current = vec2i(0, 0);
			squares = vec2i(std::ceil((float)pW->img.width/squareSize), std::ceil((float)pW->img.height/squareSize));
		}
		virtual void execute(Worker* pWorker) override;
		bool getNextRect(vec4i& rect);
	};
}
