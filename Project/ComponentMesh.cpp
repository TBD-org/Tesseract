#include "ComponentMesh.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModulePrograms.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ComponentMaterial.h"

#include "assimp/mesh.h"
#include "GL/glew.h"

#include "Leaks.h"

ComponentMesh::ComponentMesh(GameObject& owner) : Component(static_type, owner) {}

void ComponentMesh::Load(const aiMesh* mesh)
{
	num_vertices = mesh->mNumVertices;
	num_indices = mesh->mNumFaces * 3;
	material_index = mesh->mMaterialIndex;

	LOG("Loading %i vertices...", num_vertices);

	unsigned position_size = sizeof(float) * 3;
	unsigned normal_size = sizeof(float) * 3;
	unsigned uv_size = sizeof(float) * 2;
	unsigned index_size = sizeof(unsigned);

	unsigned vertex_size = position_size + normal_size + uv_size;
	unsigned vertex_buffer_size = vertex_size * num_vertices;
	unsigned index_buffer_size = index_size * num_indices;

	// Create VAO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// Load VBO
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, nullptr, GL_STATIC_DRAW);
	float* vertex_buffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, vertex_buffer_size, GL_MAP_WRITE_BIT);
	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		aiVector3D& vertex = mesh->mVertices[i];
		aiVector3D& normal = mesh->mNormals[i];
		aiVector3D* texture_coords = mesh->mTextureCoords[0];

		*(vertex_buffer++) = vertex.x;
		*(vertex_buffer++) = vertex.y;
		*(vertex_buffer++) = vertex.z;
		*(vertex_buffer++) = normal.x;
		*(vertex_buffer++) = normal.y;
		*(vertex_buffer++) = normal.z;
		*(vertex_buffer++) = texture_coords != nullptr ? texture_coords[i].x : 0;
		*(vertex_buffer++) = texture_coords != nullptr ? texture_coords[i].y : 0;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Load EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, nullptr, GL_STATIC_DRAW);
	unsigned* index_buffer = (unsigned*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, index_buffer_size, GL_MAP_WRITE_BIT);
	for (unsigned i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace& face = mesh->mFaces[i];

		// Assume triangles = 3 indices per face
		if (face.mNumIndices != 3)
		{
			LOG("Found a face with %i vertices. Discarded.", face.mNumIndices);
			continue;
		}

		*(index_buffer++) = face.mIndices[0];
		*(index_buffer++) = face.mIndices[1];
		*(index_buffer++) = face.mIndices[2];
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// Load vertex attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)position_size);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)(position_size  + normal_size));

	// Unbind VAO
	glBindVertexArray(0);
}

void ComponentMesh::Release()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void ComponentMesh::Draw(const std::vector<ComponentMaterial*>& materials, const float4x4& model_matrix) const
{
	unsigned program = App->programs->default_program;

	float4x4 view_matrix = App->camera->GetViewMatrix();
	float4x4 proj_matrix = App->camera->GetProjectionMatrix();
	unsigned texture = materials.size() > material_index ? materials[material_index]->texture : 0;

	// TODO : Move to Component Light
	float3 light_color = float3(1.0f, 1.0f, 1.0f);
	float3 light_direction = float3(1.0f, -1.0f, 1.0f);
	float3 ambient_color = float3(1.0f, 0.0f, 0.0f);

	if (materials[material_index]->material_type == ShaderType::PHONG) 
	{
		program = App->programs->phong_program;
		glUseProgram(program);

		glUniform1f(glGetUniformLocation(program, "Kd"), materials[material_index]->Kd);
		glUniform1f(glGetUniformLocation(program, "Ks"), materials[material_index]->Ks);
		glUniform1i(glGetUniformLocation(program, "n"), materials[material_index]->n);
		glUniform3fv(glGetUniformLocation(program, "ambient_color"), 1, App->renderer->ambient_color.ptr());
		glUniform3fv(glGetUniformLocation(program, "light_direction"), 1, light_direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light_color"), 1, light_color.ptr());
		glUniform3fv(glGetUniformLocation(program, "camera_direction"), 1, App->camera->GetFront().ptr());
	}
	else 
	{
		glUseProgram(program);
	}

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_matrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view_matrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj_matrix.ptr());
	glUniform1i(glGetUniformLocation(program, "diffuse_texture"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}