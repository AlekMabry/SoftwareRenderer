#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>

#if defined (_MSC_VER)
#define SGL_INLINE __forceinline
#define SGL_EXPORT __declspec(dllexport)
#define SGL_ALIGN(X) __declspec(align(X))
#else
#define SGL_INLINE inline
#define SGL_EXPORT
#define SGL_ALIGN(X) __atribute((aligned(X)))
#endif

#ifdef _DEBUG
#define SGL_VALIDATE
#endif

#define SGL_MALLOC(size)_aligned_malloc(size, 16)
#define SGL_DEPTH_FAR					1.0f

typedef uint32_t SglType;

#define SGL_TYPE_INSTANCE				1
#define SGL_TYPE_CALLBACK				2
#define SGL_TYPE_COMMAND_BUFFER			3
#define SGL_TYPE_IMAGE_BGRA				4
#define SGL_TYPE_IMAGE_FLOAT			5
#define SGL_TYPE_SHADER					6
#define SGL_TYPE_MESH					7
#define SGL_TYPE_TARGET_INFO			8
#define SGL_TYPE_BIN					9

/** Prototype struct for an SGL object. All objects in SGL will contain a type
	member. */
typedef struct SglObject
{
	SglType sType;
} SGLObject;

typedef uint32_t SglCallbackSeverityFlags;

#define SGL_CALLBACK_SEVERITY_INFO		1	/** Informational message like resource creation. */
#define SGL_CALLBACK_SEVERITY_WARNING	2	/** Behaviour that is not necessarily an error */
#define SGL_CALLBACK_SEVERITY_ERROR		4	/** Invalid behavior, may cause crashes. */

#ifdef SGL_VALIDATE
/** A user defined validation callback function.
	@param[in]	callbackSeverity	Severity of callback, see
									SGL_MESSAGE_SEVERITY_ macros.
	@param[in]	pObject				Pointer array of objects.
	@param[in]	objects				Number of objects in pointer array.
	@param[in]	pMessage			Message format.
	@param[in]	...					Arguments for message format.
									callback. */
extern void sglValidationCallback(
	const SglCallbackSeverityFlags callbackSeverity,
	const SGLObject** pObjects, uint32_t objects,
	const uint8_t* pMessage, ...);
#else
SGL_INLINE void sglValidationCallback(
	const SGLCallbackSeverityFlags callbackSeverity,
	const SGLObject* pObject, uint32_t objects,
	const uint8_t* pMessage, ...) {}
#endif

