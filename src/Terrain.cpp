#include "Terrain.h"

Model Terrain::Generate(std::string texture_path) {
    int SIZE = 60;
    int vertex_count = 128;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	int total_vertices = vertex_count * vertex_count;

    for (int i = 0; i < vertex_count; i++) {
        for (int j = 0; j < vertex_count; j++) {
            Vertex v;
            v.Position = glm::vec3((float)j / ((float)vertex_count - 1) * SIZE, 0, (float)i / ((float)vertex_count - 1) * SIZE);
            v.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            v.TextureCoordinates = glm::vec2((float)j / ((float)vertex_count - 1), (float)i / ((float)vertex_count - 1));
            vertices.push_back(v);
        }
    }

    int index_loc = 0;
    for (int gz = 0; gz < vertex_count - 1; gz++) {
        for (int gx = 0; gx < vertex_count - 1; gx++) {
            int top_left = (gz * vertex_count) + gx;
            int top_right = top_left + 1;
            int bottom_left = ((gz + 1) * vertex_count) + gx;
            int bottom_right = bottom_left + 1;
            indices.push_back(top_left);
            indices.push_back(bottom_left);
            indices.push_back(top_right);
            indices.push_back(top_right);
            indices.push_back(bottom_left);
            indices.push_back(bottom_right);
        }
    }

    Texture terrain_texture; 
    terrain_texture.Id = Texture::Load(texture_path);
    terrain_texture.Path = texture_path;
    terrain_texture.Type = "diffuse";

    textures.push_back(terrain_texture);

    std::vector<Mesh> meshes;

    meshes.push_back(Mesh(vertices, indices, textures));

    return Model(meshes);
}
