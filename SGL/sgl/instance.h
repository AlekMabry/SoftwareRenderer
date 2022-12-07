#include "common.h"
#include "render.h"

typedef struct SGLCommandBindTarget
{
	SGLType sType;
	uint32_t order;
	SGLTex2D* pColorTex;
	SGLTex2D* pDepthTex;
} SGLCommandBindTarget;

typedef struct SGLCommandDrawMesh
{
	SGLType sType;
	uint32_t order;
	SGLMeshInfo* pMesh;
} SGLCommandDrawMesh;

typedef union SGLCommand
{
	struct info {
		SGLType sType;
		uint32_t order;
	};
	SGLCommandBindTarget bindTarget;
	SGLCommandDrawMesh drawMesh;
} SGLCommand;

typedef struct SGLCommandBuffer
{
	SGLType sType;
	SGLCommand* pCmds;		/** Array of commands. */
	uint32_t cmds;			/** Number of commands. */
	uint32_t cmdsSize;		/** Size of command array. */
} SGLCommandBuffer;

typedef struct SGLInstance
{
	SGLType sType;
	float* pVertCache;		/** Transformed vertex cache. */
	uint32_t vertCacheSize;	/** Vertex cache memory allocated size. */
	uint32_t vertCacheSP;	/** Vertex cache stack pointer. */
	SGLTargetInfo* pTarget;

	SGLCommandBuffer** pCmdBuffers;	/** Command buffer pointer array. */
	uint32_t cmdBuffers;			/** Number of command buffers. */
	uint32_t cmdBufferSize;			/** Size of command buffer pointer
										array. */
} SGLInstance;


SGL_EXPORT void sglCreateInstance(SGLInstance* pInstance);

SGL_EXPORT void sglDestroyInstance(SGLInstance* pInstance);

SGL_EXPORT SGLCommandBuffer* sglCreateCommandBuffer(uint32_t size);

SGL_EXPORT void sglCommandBindTarget(
	
	SGLCommandBuffer* pCmdBuf);

void sglSubmitCommand(const SGLCommand* pCmd, SGLCommandBuffer* pCmdBuf);

SGL_EXPORT void sglSubmitCommandBuffer();