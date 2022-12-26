#include "command.h"

void sglCmdBegin(SglCommandBuffer* pCommandBuffer)
{
	if (pRenderStages == NULL)
	{
		// Allocate command buffer
		pCommandBuffer->pRenderStages = calloc(sizeof(SglRenderpassStage) *
			SGL_STAGE_RENDERPASS_COUNT);
		pCommandBuffer->renderStagesCount = 0;
		pCommandBuffer->renderStagesSize = SGL_STAGE_RENDERPASS_COUNT;
	}
	else
	{
		// Clear command buffer
		for (uint32_t i = 0; i < pCommandBuffer->renderStageCount; i++)
		{
			pCommandBuffer->pRenderStages[i].p
		}
	}
	
	pCommandBuffer->renderStageCount = 0;
}

void sglCmdBeginRenderpass(SglCommandBuffer* pCommandBuffer, SglImage* pColorTarget,
	SglImage* pDepthTarget, bool bDepthCheck, bool bDepthWrite)
{
	if (pCommandBuffer->renderpassCount == pCommandBuffer->renderpassSize)
	{
		pCommandBuffer->pRenderStages = realloc(pCommandBuffer->pRenderStages,
			pCommandBuffer->renderpassSize * 2 * sizeof(SglRenderpassStage));
		memset(&pCommandBuffer->pRenderStages[pCommandBuffer->renderpassSize], 0,
			pCommandBuffer->renderpassSize * sizeof(SglRenderpassStage));
		pCommandBuffer->renderpassSize *= 2;
	}
	
	SglRenderpassStage* pRenderpass = &pCommandBuffer->pRenderpasses[pCommandBuffer->renderpassCount];
	pRenderpass->pColorTarget = pColorTarget;
	pRenderpass->pDepthTarget = pDepthTarget;
	pRenderpass->bDepthCheck = bDepthCheck;
	pRenderpass->bDepthWrite = bDepthWrite;
	if (pRenderpass->pPipelines == NULL)
	{
		pRenderpass->pPipelines = calloc(sizeof(SglPipelineStage) * SGL_STAGE_PIPELINE_COUNT);
		pRenderpass->pipelineSize = SGL_STAGE_PIPELINE_COUNT;
	}
	pRenderpass->pipelineCount = 0;

	pCommandBuffer->renderpassCount++;
}

void sglCmdBindPipeline(SglCommandBuffer* pCommandBuffer, const SglPipeline* pPipeline)
{
	SglRenderpassStage* pRenderpass =
		&pCommandBuffer->pRenderpasses[pCommandBuffer->renderpassCount - 1];
	
	if (pRenderpass->pipelineCount == pRenderpass->pipelineSize)
	{
		pRenderpass->pPipelines = realloc(pRenderpass->pipelineSize * 2 *
			sizeof(SglPipelineStage));
		memset(&pRenderpass->pPipelines[pRenderpass->piplineSize], 0, pRenderpass->pipelineSize *
			sizeof(SglPipelineStage));
		pRenderpass->pipelineSize *= 2;
	}

	SglPipelineStage* pPipelineStage = pRenderpass->pPipelines[pRenderpass->pipelineCount];
	pPipelineStage->pPipeline = pPipeline;
	if (pPipelineStage->pDraws == NULL)
	{
		pPipelineStage->pDraws = calloc(sizeof(SglDrawStage) * SGL_STAGE_DRAW_COUNT);
		pPipelineStage->drawSize = SGL_STAGE_DRAW_COUNT;
	}
	pPipelineStage->drawCount = 0;

	pRenderpass->pipelineCount++;
}

void sglCmdBindUniforms(SglCommandBuffer* pCommandBuffer, const void* pUniforms)
{
	pCommandBuffer->pBoundUniforms = pUniforms;
}

void sglCmdDraw(SglCommandBuffer* pCommandBuffer, const void* pVertices,
	const void* pIndices, uint32_t vertexCount, uint32_t indexCount, uint32_t instanceCount)
{
	SglRenderpassStage* pRenderpass =
		pCommandBuffer->pRenderpasses[pCommandBuffer->renderpassCount - 1];
	SglPipelineStage* pPipeline = pRenderpass->pPipelines[pRenderpass->pipelineCount - 1];
	
	if (pPipeline->drawCount == pPipeline->drawSize)
	{
		pPipeline->pDraws = realloc(pPipeline)
	}
}