#pragma once
namespace core {
	struct renderShowTask final: public Renderer::imageShowTask, public Getter<renderShowTask> {
		Image img;
		Image simg;
		vec2i *threadSquare;
		int nt;
		renderShowTask(simdView* view, int nthreads);
		renderShowTask() { delete[] threadSquare; }
		void onStartNode(simdView* view, vec2i cStep, vec2i nStep, int square) override;
		void onEndNode(simdView* view, vec2i cStep, vec2i nStep, int square) override;
	};

	struct imageRenderTask : public Renderer::Worker::Task {
		PBVH* pbvh;
		simdView* pview;
		Renderer::PixelShader* shader;
		int samples;
		~imageRenderTask() {}
		imageRenderTask(PBVH* pB, simdView* pW, const int samp, Renderer::PixelShader* sh) : pbvh(pB), pview(pW), samples(samp) { shader = sh; }
		virtual void execute(Renderer::Worker* pWorker) override;
	};
}
