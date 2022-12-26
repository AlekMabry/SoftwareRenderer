#include "common.h"
#include "pipeline.h"
#include "string.h"

// Default vector sizes in SglCommandBuffer
#define SGL_STAGE_RENDERPASS_COUNT		2
#define SGL_STAGE_PIPELINE_COUNT		2
#define SGL_STAGE_DRAW_COUNT			8
#define SGL_STAGE_DRAW_MESH_COUNT		4

typedef struct SglMeshStage
{
	void* pVertices;
	void* pIndices;
	uint32_t vertexCount;
	uint32_t indexCount;
	uint32_t uniformOffset;
} SglMeshStage;

typedef struct SglDrawStage
{
	void* pUniforms;
	uint32_t indexTotalCount;
	uint32_t vertexTotalCount;
	SglMeshStage* pInstances;
	uint32_t instanceCount;
	uint32_t instanceSize;
} SglDrawStage;

typedef struct SglPipelineStage
{
	SglPipeline* pPipeline;
	SglDrawStage* pDraws;
	uint32_t drawCount;
	uint32_t drawSize;
} SglPipelineStage;

typedef struct SglRenderpassStage
{
	SglImage* pColorTarget;
	SglImage* pDepthTarget;
	bool bDepthCheck;
	bool bDepthWrite;
	SglPipelineStage* pPipelines;
	uint32_t pipelineCount;
	uint32_t pipelineSize;
} SglRenderpassStage;

typedef struct SglCommandBuffer
{
	SglRenderpassStage* pRenderpasses;
	uint32_t renderpassCount;
	uint32_t renderpassSize;
	void* pBoundUniforms;
} SglCommandBuffer;

SGL_EXPORT void sglCmdBegin(SglCommandBuffer* pCommandBuffer);

SGL_EXPORT void sglCmdBeginRenderpass(SglCommandBuffer* pCommandBuffer, SglImage* pColorTarget,
	SglImage* pDepthTarget, bool bDepthCheck, bool bDepthWrite);

SGL_EXPORT void sglCmdBindPipeline(SglCommandBuffer* pCommandBuffer, const SglPipeline* pPipeline);

SGL_EXPORT void sglCmdBindUniforms(SglCommandBuffer* pCommandBuffer, const void* pUniforms);

SGL_EXPORT void sglCmdDraw(SglCommandBuffer* pCommandBuffer, const void* pVertices,
	const void* pIndices, uint32_t vertexCount, uint32_t indexCount, uint32_t instanceCount);

SGL_EXPORT void sglSubmitCommandBuffer(SglCommandBuffer* pCommandBuffer,
	uint32_t threadCount, SglInstance* pInstance);