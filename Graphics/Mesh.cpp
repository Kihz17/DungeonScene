#include "Mesh.h"

#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/postprocess.h>

#include <iostream>

static const uint32_t assimpFlags =
aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
aiProcess_Triangulate |             // Make sure we're triangles
aiProcess_SortByPType |             // Split meshes by primitive type
aiProcess_GenNormals |              // Make sure we have legit normals
aiProcess_GenUVCoords |             // Convert UVs if required 
aiProcess_OptimizeMeshes |          // Batch draws where possible
aiProcess_JoinIdenticalVertices |	// Join up identical vertices
aiProcess_ValidateDataStructure;    // Validation

static glm::mat4 ConvertToGLMMat4(const aiMatrix4x4& matrix)
{
	glm::mat4 glmMat;
	glmMat[0][0] = matrix.a1;
	glmMat[1][0] = matrix.a2;
	glmMat[2][0] = matrix.a3;
	glmMat[3][0] = matrix.a4;
	glmMat[0][1] = matrix.b1;
	glmMat[1][1] = matrix.b2;
	glmMat[2][1] = matrix.b3;
	glmMat[3][1] = matrix.b4;
	glmMat[0][2] = matrix.c1;
	glmMat[1][2] = matrix.c2;
	glmMat[2][2] = matrix.c3;
	glmMat[3][2] = matrix.c4;
	glmMat[0][3] = matrix.d1;
	glmMat[1][3] = matrix.d2;
	glmMat[2][3] = matrix.d3;
	glmMat[3][3] = matrix.d4;
	return glmMat;
}

static float* ConvertVerticesToArray(const std::vector<Vertex>& vertices)
{
	uint32_t vertexBufferSize = (uint32_t)(vertices.size() * sizeof(Vertex));
	float* vertexBuffer = new float[vertexBufferSize];
	uint32_t bufferIndex = 0;
	for (int i = 0; i < vertices.size(); i++) // Convert data into contiguous float array 
	{
		const Vertex& vertex = vertices[i];
		vertexBuffer[bufferIndex] = vertex.position.x;
		vertexBuffer[bufferIndex + 1] = vertex.position.y;
		vertexBuffer[bufferIndex + 2] = vertex.position.z;

		vertexBuffer[bufferIndex + 3] = vertex.normal.x;
		vertexBuffer[bufferIndex + 4] = vertex.normal.y;
		vertexBuffer[bufferIndex + 5] = vertex.normal.z;

		vertexBuffer[bufferIndex + 6] = vertex.textureCoord.x;
		vertexBuffer[bufferIndex + 7] = vertex.textureCoord.y;

		vertexBuffer[bufferIndex + 8] = vertex.binormal.x;
		vertexBuffer[bufferIndex + 9] = vertex.binormal.y;
		vertexBuffer[bufferIndex + 10] = vertex.binormal.z;

		vertexBuffer[bufferIndex + 11] = vertex.tangent.x;
		vertexBuffer[bufferIndex + 12] = vertex.tangent.y;
		vertexBuffer[bufferIndex + 13] = vertex.tangent.z;
		 
		bufferIndex += 14;
	}

	return vertexBuffer;
}

static uint32_t* ConvertIndicesToArray(const std::vector<Face>& faces)
{
	uint32_t indexBufferSize = (uint32_t)(faces.size() * sizeof(Face));
	uint32_t* indexBuffer = new uint32_t[indexBufferSize];
	uint32_t bufferIndex = 0;
	for (int i = 0; i < faces.size(); i++) // Convert data into contiguous int array 
	{
		const Face& face = faces[i];
		indexBuffer[bufferIndex] = face.v1;
		indexBuffer[bufferIndex + 1] = face.v2;
		indexBuffer[bufferIndex + 2] = face.v3;

		bufferIndex += 3;
	}

	return indexBuffer;
}

struct AssimpLogger : public Assimp::LogStream
{
	static void Initialize()
	{
		if (Assimp::DefaultLogger::isNullLogger())
		{
			Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
			Assimp::DefaultLogger::get()->attachStream(new AssimpLogger, Assimp::Logger::Err | Assimp::Logger::Warn);
		}
	}

	virtual void write(const char* message) override
	{
		std::cout << "[ASSIMP ERROR]: " << message;
	}
};

Mesh::Mesh(const std::string& filePath)
	: filePath(filePath), boundingBox(glm::vec3(0.0f), glm::vec3(0.0f))
{
	AssimpLogger::Initialize();

	std::cout << "Loading mesh " << filePath << "..." << std::endl;;
	this->importer = CreateScope<Assimp::Importer>();

	const aiScene* scene = this->importer->ReadFile(filePath, assimpFlags);
	if (!scene || !scene->HasMeshes())
	{
		std::cout << "Failed to load mesh file: " << filePath << std::endl;
		return;
	}

	this->assimpScene = scene;
	this->inverseTransform = glm::inverse(ConvertToGLMMat4(scene->mRootNode->mTransformation));
	this->submeshes.reserve(scene->mNumMeshes);

	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* assimpMesh = scene->mMeshes[i];

		Submesh submesh;
		submesh.baseVertex = vertexCount;
		submesh.baseIndex = indexCount;
		submesh.materialIndex = assimpMesh->mMaterialIndex;
		submesh.vertexCount = assimpMesh->mNumVertices;
		submesh.indexCount = assimpMesh->mNumFaces * 3;
		submesh.meshName = assimpMesh->mName.C_Str();
		this->submeshes.push_back(submesh);

		vertexCount += assimpMesh->mNumVertices;
		indexCount += submesh.indexCount;

		if (!assimpMesh->HasPositions())
		{
			std::cout << "Mesh does not have position!" << std::endl;
			return;
		}

		if (!assimpMesh->HasNormals())
		{
			std::cout << "Mesh does not have normals!" << std::endl;
			return;
		}

		AABB& aabb = submesh.boundingBox;
		aabb.min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		aabb.max = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (unsigned int j = 0; j < assimpMesh->mNumVertices; j++)
		{
			Vertex vertex;
			vertex.position = glm::vec3(assimpMesh->mVertices[j].x, assimpMesh->mVertices[j].y, assimpMesh->mVertices[j].z);
			vertex.normal = glm::vec3(assimpMesh->mNormals[j].x, assimpMesh->mNormals[j].y, assimpMesh->mNormals[j].z);
			aabb.min.x = glm::min(vertex.position.x, aabb.min.x);
			aabb.min.y = glm::min(vertex.position.y, aabb.min.y);
			aabb.min.z = glm::min(vertex.position.z, aabb.min.z);
			aabb.max.x = glm::max(vertex.position.x, aabb.max.x);
			aabb.max.y = glm::max(vertex.position.y, aabb.max.y);
			aabb.max.z = glm::max(vertex.position.z, aabb.max.z);

			if (assimpMesh->HasTextureCoords(0))
			{
				vertex.textureCoord = glm::vec2(assimpMesh->mTextureCoords[0][j].x, assimpMesh->mTextureCoords[0][j].y);
			}


			if (assimpMesh->HasTangentsAndBitangents())
			{
				vertex.tangent = glm::vec3(assimpMesh->mTangents[j].x, assimpMesh->mTangents[j].y, assimpMesh->mTangents[j].z);
				vertex.binormal = glm::vec3(assimpMesh->mBitangents[j].x, assimpMesh->mBitangents[j].y, assimpMesh->mBitangents[j].z);
			}

			this->vertices.push_back(vertex);
		}

		for (unsigned int j = 0; j < assimpMesh->mNumFaces; j++)
		{
			if (assimpMesh->mFaces[j].mNumIndices != 3)
			{
				std::cout << "Face must be a triangle!" << std::endl;
				return;
			}

			Face face;
			face.v1 = assimpMesh->mFaces[j].mIndices[0];
			face.v2 = assimpMesh->mFaces[j].mIndices[1];
			face.v3 = assimpMesh->mFaces[j].mIndices[2];
			this->faces.push_back(face);
		}
	}

	LoadNodes(scene->mRootNode); // Load all the submeshes

	// Configure parent's bounding box based off of the submeshes we just added
	for (Submesh& submesh : this->submeshes)
	{
		AABB submeshAABB = submesh.boundingBox;
		glm::vec3 min = glm::vec3(submesh.transform * glm::vec4(submeshAABB.min, 1.0f));
		glm::vec3 max = glm::vec3(submesh.transform * glm::vec4(submeshAABB.max, 1.0f));

		this->boundingBox.min.x = glm::min(this->boundingBox.min.x, min.x);
		this->boundingBox.min.y = glm::min(this->boundingBox.min.y, min.y);
		this->boundingBox.min.z = glm::min(this->boundingBox.min.z, min.z);
		this->boundingBox.max.x = glm::max(this->boundingBox.max.x, max.x);
		this->boundingBox.max.y = glm::max(this->boundingBox.max.y, max.y);
		this->boundingBox.max.z = glm::max(this->boundingBox.max.z, max.z);
	}

	if (scene->HasMaterials())
	{
		SetupMaterials();
	}

	BufferLayout bufferLayout = {
		{ ShaderDataType::Float3, "vPosition" },
		{ ShaderDataType::Float3, "vNormal" },
		{ ShaderDataType::Float2, "vTextureCoordinates" },
		{ ShaderDataType::Float3, "vBiNormal" },
		{ ShaderDataType::Float3, "vTangent" }
	};

	this->vertexArray = CreateRef<VertexArrayObject>();

	uint32_t vertexBufferSize = (uint32_t)(this->vertices.size() * sizeof(Vertex));
	float* vertexBuffer = ConvertVerticesToArray(this->vertices);

	this->vertexBuffer = CreateRef<VertexBuffer>(vertexBuffer, vertexBufferSize);
	this->vertexBuffer->SetLayout(bufferLayout);

	uint32_t indexBufferSize = (uint32_t)(this->faces.size() * sizeof(Face));
	uint32_t* indexBuffer = ConvertIndicesToArray(this->faces);

	this->vertexArray->Bind();
	this->indexBuffer = CreateRef<IndexBuffer>(indexBuffer, indexBufferSize);

	this->vertexArray->AddVertexBuffer(this->vertexBuffer);
	this->vertexArray->SetIndexBuffer(this->indexBuffer);

	delete[] vertexBuffer;
	delete[] indexBuffer;
}

Mesh::Mesh(const Ref<Mesh> mesh)
	: submeshes(mesh->submeshes),
	inverseTransform(mesh->inverseTransform),
	vertexArray(mesh->vertexArray),
	vertexBuffer(mesh->vertexBuffer),
	indexBuffer(mesh->indexBuffer),
	vertices(mesh->vertices),
	faces(mesh->faces),
	nodeMap(mesh->nodeMap),
	assimpScene(mesh->assimpScene),
	textures(mesh->textures),
	boundingBox(mesh->boundingBox),
	filePath(mesh->filePath)
{

}

Mesh::~Mesh()
{

}

void Mesh::LoadNodes(aiNode* node, const glm::mat4& parentTransform)
{
	glm::mat4 transform = parentTransform * ConvertToGLMMat4(node->mTransformation);
	this->nodeMap[node].resize(node->mNumMeshes);
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		unsigned int meshIndex = node->mMeshes[i];
		Submesh& submesh = this->submeshes[meshIndex];
		submesh.nodeName = node->mName.C_Str();
		submesh.transform = transform;
		this->nodeMap[node][i] = meshIndex;
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadNodes(node->mChildren[i], transform);
	}
}

void Mesh::SetupMaterials()
{
	// TODO: Setup materials
}