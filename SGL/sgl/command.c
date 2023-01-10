#include "command.h"

void sglInitCommandBuffer(SglCommandBuffer* pCommandBuffer)
{
	pCommandBuffer->sType = SGL_TYPE_COMMAND_BUFFER;
	SGL_VECTOR_INIT(pCommandBuffer->renderpasses, SGL_STAGE_RENDERPASS_COUNT)
	SGL_VECTOR_INIT(pCommandBuffer->pipelines, SGL_STAGE_PIPELINE_COUNT)
	SGL_VECTOR_INIT(pCommandBuffer->draws, SGL_STAGE_DRAW_COUNT)
	SGL_VECTOR_INIT(pCommandBuffer->instances, SGL_STAGE_INSTANCE_COUNT)
}

void sglFreeCommandBuffer(SglCommandBuffer* pCommandBuffer)
{
	SGL_VECTOR_FREE(pCommandBuffer->renderpasses)
	SGL_VECTOR_FREE(pCommandBuffer->pipelines)
	SGL_VECTOR_FREE(pCommandBuffer->draws)
	SGL_VECTOR_FREE(pCommandBuffer->instances)
}

void sglCmdBegin(SglCommandBuffer* pCommandBuffer)
{
	SGL_VECTOR_CLEAR(pCommandBuffer->renderpasses)
	SGL_VECTOR_CLEAR(pCommandBuffer->pipelines)
	SGL_VECTOR_CLEAR(pCommandBuffer->draws)
	SGL_VECTOR_CLEAR(pCommandBuffer->instances)
}

void sglCmdBeginRenderpass(SglCommandBuffer* pCommandBuffer, SglImage* pColorTarget,
	SglImage* pDepthTarget, bool bDepthCheck, bool bDepthWrite)
{
	if (SGL_VECTOR_SIZE(pCommandBuffer->renderpasses) > 0)
	{
		if (SGL_VECTOR_LAST(pCommandBuffer->renderpasses).pipelineStart < 0)
		{
			SglObject* objects[] = { pCommandBuffer };
			sglValidationCallback(SGL_CALLBACK_SEVERITY_WARNING, objects, 1,
				"A new renderpass is being created before anything was drawn in the previous pass!");
		}
	}

	SglRenderpassStage renderpass =
	{
		pColorTarget,
		pDepthTarget,
		bDepthCheck,
		bDepthWrite,
		-1,
		0
	};

	SGL_VECTOR_PUSH_BACK(pCommandBuffer->renderpasses, renderpass);
}

void sglCmdBindPipeline(SglCommandBuffer* pCommandBuffer, const SglPipeline* pPipeline)
{
	// Verify that at least one renderpass has been created
	if (SGL_VECTOR_SIZE(pCommandBuffer->renderpasses) < 1)
	{
		SglObject* objects[] = { pCommandBuffer };
		sglValidationCallback(SGL_CALLBACK_SEVERITY_ERROR, objects, 1,
			"Unable to bind pipeline if a renderpass hasn't been created first!");
		return;
	}

	SglRenderpassStage* pRenderpass = &SGL_VECTOR_LAST(pCommandBuffer->renderpasses);
	if (pRenderpass->pipelineStart < 0)
	{
		pRenderpass->pipelineStart = SGL_VECTOR_SIZE(pCommandBuffer->pipelines);
		pRenderpass->pipelineCount = 1;
	}
	else
	{
		pRenderpass->pipelineCount += 1;
	}

	SglPipelineStage pipeline =
	{
		pPipeline,
		-1,
		0
	};

	SGL_VECTOR_PUSH_BACK(pCommandBuffer->pipelines, pipeline);
}

void sglCmdBindUniforms(SglCommandBuffer* pCommandBuffer, const void* pUniforms)
{
	pCommandBuffer->pBoundUniforms = pUniforms;
}

void sglCmdDraw(SglCommandBuffer* pCommandBuffer, const void* pVertices,
	const void* pIndices, uint32_t vertexCount, uint32_t indexCount, uint32_t instanceCount)
{
	SglRenderpassStage* pRenderpass =
		&pCommandBuffer->pRenderpasses[pCommandBuffer->renderpassCount - 1];
	SglPipelineStage* pPipeline = &pRenderpass->pPipelines[pRenderpass->pipelineCount - 1];
	SglDrawStage* pDraw = &pPipeline->pDraws[pPipeline->drawCount - 1];

	if (pDraw->instanceCount == pDraw->instanceSize)
	{
		pDraw->pInstances = realloc(pDraw->pInstances, )
		memset(&pPipeline->pDraws[pPipeline->])
	}
}