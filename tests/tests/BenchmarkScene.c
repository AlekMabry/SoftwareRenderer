#include <sgl.h>

int main()
{
	SGLInstance context;
	sglCreateInstance(&context);

	/** Game thread one. */
	SGLCommandBuffer* cmdBuffer = malloc(sizeof(SGLCommandBuffer));

	SGLMeshInfo meshA;
	sglCreateMeshInfo(shader, uniforms, mesh, start, count, &meshA);

	sglCmdBindTarget(&m, 0 & queue);
	sglCmdDrawMesh(&meshA, 0, &queue);
	sglSubmitCmdBuffer
	/** Game thread killed. */
	return 0;
}