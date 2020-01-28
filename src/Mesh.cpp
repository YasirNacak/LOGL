#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->Vertices = vertices;
	this->Indices = indices;
	this->Textures = textures;

	Setup();
}

void Mesh::Draw(Shader shader) {
    unsigned int diffuse_index = 0;
    unsigned int specular_index = 0;
    for (unsigned int i = 0; i < Textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        
        std::string number;
        std::string type = Textures[i].Type;

        if (type == "diffuse") {
            number = std::to_string(diffuse_index++);
            shader.SetFloat(("material.texture_diffuse" + number).c_str(), i);
        }
        else if (type == "specular") {
            number = std::to_string(specular_index++);
            shader.SetFloat(("material.texture_specular" + number).c_str(), i);
        }

        glBindTexture(GL_TEXTURE_2D, Textures[i].Id);
    }

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::Setup() {
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoordinates));

    glBindVertexArray(0);
}