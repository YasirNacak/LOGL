#include "Texture.h"

Texture::Texture()
{
	Id = 0;
	Type = "";
	Path = "";
}

Texture::Texture(unsigned int id, std::string type, std::string path) : Id(id), Type(type), Path(path){
}

unsigned int Texture::Load(std::string path) {
	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	int width, height, n_components;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &n_components, 0);
	if (data) {
		GLenum format;
		if (n_components == 1) {
			format = GL_RED;
		}
		else if (n_components == 3) {
			format = GL_RGB;
		}
		else if (n_components == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return texture_id;
}
