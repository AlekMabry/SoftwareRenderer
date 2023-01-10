#pragma once

#include "common.h"
#include "instance.h"
#include "command.h"

typedef struct SglThread
{
	uint32_t placeholder;
} SglThread;

void sglThreadExecute(SglInstance* pInstance, SglCommandBuffer* pCommandBuffer);