#include "AABB.h"

#include "GLCommon.h"

AABB::AABB(const glm::vec3& min, const glm::vec3 max) : min(min), max(max)
{

}

AABB::~AABB()
{

}

void AABB::Draw(const glm::vec3& position, const glm::vec3& scale) const
{
	float* vertexData = GetVertices(position, scale);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertexData);
	glDrawArrays(GL_LINES, 0, 24);
	glDisableClientState(GL_VERTEX_ARRAY);
	delete[] vertexData;
}

float* AABB::GetVertices(const glm::vec3& position, const glm::vec3& scale) const
{
	float* vertexData = new float[72];

	// Draw bottom square
	vertexData[0] = (min.x * scale.x)+ position.x;
	vertexData[1] = (min.y * scale.y)+ position.y;
	vertexData[2] = (min.z * scale.z)+ position.z;
	vertexData[3] = (min.x * scale.x)+ position.x;
	vertexData[4] = (min.y * scale.y)+ position.y;
	vertexData[5] = (max.z * scale.z)+ position.z;

	vertexData[6] = (min.x * scale.x)+ position.x;
	vertexData[7] = (min.y * scale.y)+ position.y;
	vertexData[8] = (min.z * scale.z)+ position.z;
	vertexData[9] = (max.x * scale.x)+ position.x;
	vertexData[10] = (min.y * scale.y)+ position.y;
	vertexData[11] = (min.z * scale.z)+ position.z;

	vertexData[12] = (max.x * scale.x)+ position.x;
	vertexData[13] = (min.y * scale.y)+ position.y;
	vertexData[14] = (min.z * scale.z)+ position.z;
	vertexData[15] = (max.x * scale.x)+ position.x;
	vertexData[16] = (min.y * scale.y)+ position.y;
	vertexData[17] = (max.z * scale.z)+ position.z;

	vertexData[18] = (min.x * scale.x)+ position.x;
	vertexData[19] = (min.y * scale.y)+ position.y;
	vertexData[20] = (max.z * scale.z)+ position.z;
	vertexData[21] = (max.x * scale.x)+ position.x;
	vertexData[22] = (min.y * scale.y)+ position.y;
	vertexData[23] = (max.z * scale.z)+ position.z;

	// Draw connecting lines up
	vertexData[24] = (min.x * scale.x)+ position.x;
	vertexData[25] = (min.y * scale.y)+ position.y;
	vertexData[26] = (min.z * scale.z)+ position.z;
	vertexData[27] = (min.x * scale.x)+ position.x;
	vertexData[28] = (max.y * scale.y)+ position.y;
	vertexData[29] = (min.z * scale.z)+ position.z;

	vertexData[30] = (max.x * scale.x)+ position.x;
	vertexData[31] = (min.y * scale.y)+ position.y;
	vertexData[32] = (min.z * scale.z)+ position.z;
	vertexData[33] = (max.x * scale.x)+ position.x;
	vertexData[34] = (max.y * scale.y)+ position.y;
	vertexData[35] = (min.z * scale.z)+ position.z;

	vertexData[36] = (max.x * scale.x)+ position.x;
	vertexData[37] = (min.y * scale.y)+ position.y;
	vertexData[38] = (max.z * scale.z)+ position.z;
	vertexData[39] = (max.x * scale.x)+ position.x;
	vertexData[40] = (max.y * scale.y)+ position.y;
	vertexData[41] = (max.z * scale.z)+ position.z;

	vertexData[42] = (min.x * scale.x)+ position.x;
	vertexData[43] = (min.y * scale.y)+ position.y;
	vertexData[44] = (max.z * scale.z)+ position.z;
	vertexData[45] = (min.x * scale.x)+ position.x;
	vertexData[46] = (max.y * scale.y)+ position.y;
	vertexData[47] = (max.z * scale.z)+ position.z;

	// Draw top square
	vertexData[48] = (min.x * scale.x)+ position.x;
	vertexData[49] = (max.y * scale.y)+ position.y;
	vertexData[50] = (min.z * scale.z)+ position.z;
	vertexData[51] = (min.x * scale.x)+ position.x;
	vertexData[52] = (max.y * scale.y)+ position.y;
	vertexData[53] = (max.z * scale.z)+ position.z;

	vertexData[54] = (min.x * scale.x)+ position.x;
	vertexData[55] = (max.y * scale.y)+ position.y;
	vertexData[56] = (min.z * scale.z)+ position.z;
	vertexData[57] = (max.x * scale.x)+ position.x;
	vertexData[58] = (max.y * scale.y)+ position.y;
	vertexData[59] = (min.z * scale.z)+ position.z;

	vertexData[60] = (max.x * scale.x)+ position.x;
	vertexData[61] = (max.y * scale.y)+ position.y;
	vertexData[62] = (min.z * scale.z)+ position.z;
	vertexData[63] = (max.x * scale.x)+ position.x;
	vertexData[64] = (max.y * scale.y)+ position.y;
	vertexData[65] = (max.z * scale.z)+ position.z;

	vertexData[66] = (min.x * scale.x)+ position.x;
	vertexData[67] = (max.y * scale.y)+ position.y;
	vertexData[68] = (max.z * scale.z)+ position.z;
	vertexData[69] = (max.x * scale.x)+ position.x;
	vertexData[70] = (max.y * scale.y)+ position.y;
	vertexData[71] = (max.z * scale.z)+ position.z;
	return vertexData;
}