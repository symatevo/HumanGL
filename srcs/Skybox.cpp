#include "Skybox.hpp"
#include "lib/stb_image.h"
#include "humanGL.hpp"

const float Skybox::_vertices[] = {
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

Skybox::Skybox(Shader &sh) :
_shader(sh) {
	std::vector<std::string> skyboxFaces = {
		std::string(SKYBOX_START) + SKYBOX_NAME_RIGHT + SKYBOX_EXT,  // right
		std::string(SKYBOX_START) + SKYBOX_NAME_LEFT + SKYBOX_EXT,  // left
		std::string(SKYBOX_START) + SKYBOX_NAME_TOP + SKYBOX_EXT,  // up
		std::string(SKYBOX_START) + SKYBOX_NAME_BOTTOM + SKYBOX_EXT,  // down
		std::string(SKYBOX_START) + SKYBOX_NAME_FRONT + SKYBOX_EXT,  // front
		std::string(SKYBOX_START) + SKYBOX_NAME_BACK + SKYBOX_EXT,  // back
	};
	load(skyboxFaces);

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), &_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}

Skybox::Skybox(Skybox const &src) :
_shader(src.getShader()) {
	*this = src;
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
}

Skybox &Skybox::operator=(Skybox const &rhs) {
	if (this != &rhs) {
		_textureID = getTextureID();
		_vao = getVao();
		_vbo = getVbo();
	}
	return *this;
}

void Skybox::load(std::vector<std::string> &faces) {
    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

    int width;
	int height;
	int nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {

			if ((width & (width - 1)) != 0 || (height & (height - 1)) != 0) {
				std::cout << "WARNING: image " << faces[i] << " is not power-of-2 dimensions" << std::endl;
			}
			GLenum format = GL_RGB;
			if (nrChannels == 1) {
				format = GL_RED;
			}
			else if (nrChannels == 3) {
				format = GL_RGB;
			}
			else if (nrChannels == 4) {
				format = GL_RGBA;
			}
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
}

void Skybox::draw() {
	_shader.use();
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(_vertices) / sizeof(_vertices[0]));
	glDepthFunc(GL_LESS);
	glBindVertexArray(0);
}

Shader		&Skybox::getShader() { return _shader; }
Shader		&Skybox::getShader() const { return _shader; }
uint32_t	Skybox::getTextureID() const { return _textureID; }
uint32_t	Skybox::getVao() const { return _vao; }
uint32_t	Skybox::getVbo() const { return _vbo; }
