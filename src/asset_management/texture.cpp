#include "texture.hpp"
#include <glad/glad.h>

void Texture::BindTexture2D()
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::BindTextureCubemap()
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}