#include "instance.h"

void sglCreateInstance(SGLInstance* pInstance)
{
	pInstance->sType = SGL_TYPE_INSTANCE;
	pInstance->pCmdBuffers = malloc(sizeof(SGLCommandBuffer*) * 8);
	pInstance->cmdBuffers = 0;
	pInstance->cmdBufferSize = 8;
}

void sglDestroyInstance(SGLInstance* pInstance)
{
	
}

SGLCommandBuffer* sglCreateCommandBuffer(uint32_t size)
{
	SGLCommandBuffer* pCmdBuf = malloc(sizeof(SGLCommandBuffer));
	pCmdBuf->sType = SGL_TYPE_COMMAND_BUFFER;
	pCmdBuf->pCmds = malloc(sizeof(SGLCommand) * size);
	pCmdBuf->cmds = 0;
	pCmdBuf->cmdBufSize = size;
	return pCmdBuf;
}

void sglSubmitCommand(const SGLCommand* pCmd, SGLCommandBuffer* pCmdBuf)
{
	pCmdBuf->cmds++;
	if (pCmdBuf->cmds >= pCmdBuf->cmdsSize)
	{
		pCmdBuf->cmdsSize *= 2;
		pCmdBuf->pCmds = realloc(pCmdBuf->pCmds, pCmdBuf->cmdsSize);
		// Validate: Did it reallocate?
	}

	pCmdBuf->pCmds[pCmdBuf->cmds - 1] = pCmd;
}

