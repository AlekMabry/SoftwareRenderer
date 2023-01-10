#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>

//---------------- Compiler-specific utility. ----------------

#if defined (_MSC_VER)
#define SGL_INLINE __forceinline
#define SGL_EXPORT __declspec(dllexport)
#define SGL_ALIGN(X) __declspec(align(X))
#else
#define SGL_INLINE inline
#define SGL_EXPORT
#define SGL_ALIGN(X) __atribute((aligned(X)))
#endif

#define SGL_MALLOC(size)_aligned_malloc(size, 16)
#define SGL_DEPTH_FAR					1.0f

//---------------- Validation ----------------

#ifdef _DEBUG
#define SGL_VALIDATE
#endif

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
} SglObject;

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
	const SglObject** pObjects, uint32_t objectCount,
	const uint8_t* pMessage, ...);
#else
SGL_INLINE void sglValidationCallback(
	const SGLCallbackSeverityFlags callbackSeverity,
	const SglObject** pObjects, uint32_t objectCount,
	const uint8_t* pMessage, ...) {}
#endif

//---------------- STD template style vectors. ----------------

/** Macro to create a vector of a particular type. */
#define SGL_VECTOR_OF(TYPE) \
	struct { \
		TYPE *pData; \
		size_t size; \
		size_t capacity; \
	}

/**	Macro to initialize a vector. */
#define SGL_VECTOR_INIT(VECTOR, CAPACITY) \
	(VECTOR).pData = malloc((CAPACITY) * sizeof(*(VECTOR).pData)); \
	(VECTOR).size = 0; \
	(VECTOR).capacity = (CAPACITY);

/** Macro to free a vector. */
#define SGL_VECTOR_FREE(VECTOR) \
	free((VECTOR).pData); \
	(VECTOR).size = 0; \
	(VECTOR).capacity = 0; \

/** Macro to clear a vector. */
#define SGL_VECTOR_CLEAR(VECTOR) \
	(VECTOR).size = 0;

/**	Macro to get vector size (returns number of elements). */
#define SGL_VECTOR_SIZE(VECTOR)	\
	VECTOR.size

#define SGL_VECTOR_AT(VECTOR, INDEX) \
	(VECTOR).pData[(INDEX)]

#define SGL_VECTOR_LAST(VECTOR) \
	(VECTOR).pData[(VECTOR).size - 1]

/** Macro to push back a value into a vector. */
#define SGL_VECTOR_PUSH_BACK(VECTOR, VALUE) \
	do { \
		if (VECTOR.size == VECTOR.capacity) \
		{ \
			VECTOR.capacity *= 2; \
			VECTOR.pData = realloc(VECTOR.pData, sizeof(*(VECTOR).pData) * VECTOR.capacity); \
		} \
		VECTOR.pData[VECTOR.size] = VALUE; \
		VECTOR.size += 1; \
	} while (0)
	