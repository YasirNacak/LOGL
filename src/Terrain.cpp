#include "Terrain.h"

Terrain::Terrain(int size, std::string heightmap_path, std::string texturemap_path) {
    _texture0 = { Texture::Load(texturemap_path), "diffuse", texturemap_path };
    _size = size;
    _is_single_texture = true;
    _terrain_model = Generate(size, heightmap_path);
}

Terrain::Terrain(int size, std::string heightmap_path, std::string splatmap_path, std::string texture0_path, std::string texture1_path, std::string texture2_path) {
    _texture0 = { Texture::Load(texture0_path), "diffuse", texture0_path };
    _texture1 = { Texture::Load(texture1_path), "diffuse", texture1_path };
    _texture2 = { Texture::Load(texture2_path), "diffuse", texture2_path };
    _splatmap_texture = { Texture::Load(splatmap_path), "splat", splatmap_path };
    _size = size;
    _terrain_model = Generate(size, heightmap_path);
}

Model Terrain::GetModel() {
    return _terrain_model;
}

int Terrain::GetSize() {
    return _size;
}

Model Terrain::Generate(int size, std::string heightmap_path) {
    int SIZE = size;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

    int width, height, n_components;
    unsigned char* data = stbi_load(heightmap_path.c_str(), &width, &height, &n_components, STBI_rgb_alpha);

    int vertex_count = width;

    if (!data) {
        std::cout << "Texture failed to load at path: " << heightmap_path << std::endl;
        stbi_image_free(data);
    }

	int total_vertices = vertex_count * vertex_count;

    for (int i = 0; i < vertex_count; i++) {
        for (int j = 0; j < vertex_count; j++) {
            Vertex v;
            v.Position = glm::vec3((float)j / ((float)vertex_count - 1) * SIZE, GetHeight(j, i, data, width), (float)i / ((float)vertex_count - 1) * SIZE);
            glm::vec3 normal = GetNormal(j, i, data, width);
            v.Normal = glm::vec3(normal.x, normal.y, normal.z);
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

    textures.push_back(_texture0);

    if (!_is_single_texture) {
        textures.push_back(_texture1);
        textures.push_back(_texture2);
        textures.push_back(_splatmap_texture);
    }

    std::vector<Mesh> meshes;

    meshes.push_back(Mesh(vertices, indices, textures));

    stbi_image_free(data); 

    return Model(meshes);
}

float Terrain::GetHeight(int x, int z, unsigned char* heightmap, int heightmap_size) {
    if (x < 0 || x >= heightmap_size || z < 0 || z >= heightmap_size) {
        return 0.0f;
    }

    const float MAX_HEIGHT = 10;
    const float MAX_PIXEL_COLOR = 256;

    unsigned char r = heightmap[4 * (z * heightmap_size + x) + 0];

    // std::cout << "for point: " << x << "," << z << " R: " << r << " G: " << g << " B:" << b << std::endl;

    float height = 0;
    height = r / 255.0f;
    //height *= 2.5f;
    //height -= 2.0f;

    //std::cout << "height: " << height << std::endl;

    return height;
}

glm::vec3 Terrain::GetNormal(int x, int z, unsigned char* heightmap, int heightmap_size) {
    float heightL = GetHeight(x - 1, z, heightmap, heightmap_size);
    float heightR = GetHeight(x + 1, z, heightmap, heightmap_size);
    float heightD = GetHeight(x, z - 1, heightmap, heightmap_size);
    float heightU = GetHeight(x, z + 1, heightmap, heightmap_size);

    glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
    normal = glm::normalize(normal);
    return normal;
}
