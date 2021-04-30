#include "texture.hpp"
#include <glad/glad.h>

void BindTexture(Texture *tex)
{
	glActiveTexture(GL_TEXTURE0 + tex->index);
	glBindTexture(GL_TEXTURE_2D, tex->ID);
}