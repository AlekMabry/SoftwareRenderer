#include "common.h"
#include "pipeline.h"
#include "string.h"

// Default vector sizes in SglCommandBuffer
#define SGL_STAGE_RENDERPASS_COUNT		4
#define SGL_STAGE_PIPELINE_COUNT		4
#define SGL_STAGE_DRAW_COUNT			16
#define SGL_STAGE_INSTANCE_COUNT		32

typedef struct SglInstanceStage
{
	void* pVertices;
	void* pIndices;
	uint32_t vertexCount;
	uint32_t indexCount;
	uint32_t uniformOffset;
} SglInstanceStage;

typedef struct SglDrawStage
{
	void* pUniforms;
	uint32_t indexTotalCount;
	uint32_t vertexTotalCount;
	uint32_t instanceIndices[2];
} SglDrawStage;

typedef struct SglPipelineStage
{
	SglPipeline* pPipeline;
	int32_t drawStart;
	uint32_t drawEnd;
} SglPipelineStage;

typedef struct SglRenderpassStage
{
	SglImage* pColorTarget;
	SglImage* pDepthTarget;
	bool bDepthCheck;
	bool bDepthWrite;
	int32_t pipelineStart;
	uint32_t pipelineCount;
} SglRenderpassStage;

typedef SGL_VECTOR_OF(SglInstanceStage) SglInstanceStageVector;
typedef SGL_VECTOR_OF(SglDrawStage) SglDrawStageVector;
typedef SGL_VECTOR_OF(SglPipelineStage) SglPipelineStageVector;
typedef SGL_VECTOR_OF(SglRenderpassStage) SglRenderpassStageVector;

/** Records state changes (bindings) and draw calls of a frame. */
typedef struct SglCommandBuffer
{
	SglType sType;
	SglRenderpassStageVector renderpasses;
	SglPipelineStageVector pipelines;
	SglDrawStageVector draws;
	SglInstanceStageVector instances;
} SglCommandBuffer;

SGL_EXPORT void sglInitCommandBuffer(SglCommandBuffer* pCommandBuffer);

SGL_EXPORT void sglFreeCommandBuffer(SglCommandBuffer* pCommandBuffer);

/** Command to begin recording in a command buffer. */
SGL_EXPORT void sglCmdBegin(SglCommandBuffer* pCommandBuffer);

/**	Command to begin a render pass. Must be called after sglCmdBegin. */
SGL_EXPORT void sglCmdBeginRenderpass(SglCommandBuffer* pCommandBuffer, SglImage* pColorTarget,
	SglImage* pDepthTarget, bool bDepthCheck, bool bDepthWrite);

SGL_EXPORT void sglCmdBindPipeline(SglCommandBuffer* pCommandBuffer, const SglPipeline* pPipeline);

SGL_EXPORT void sglCmdBindUniforms(SglCommandBuffer* pCommandBuffer, const void* pUniforms);

SGL_EXPORT void sglCmdDraw(SglCommandBuffer* pCommandBuffer, const void* pVertices,
	const void* pIndices, uint32_t vertexCount, uint32_t indexCount, uint32_t instanceCount);

SGL_EXPORT void sglSubmitCommandBuffer(SglCommandBuffer* pCommandBuffer,
	uint32_t threadCount, SglInstance* pInstance);