#pragma once

struct Texture
{
	unsigned int ID;
	int index;
	int width, height, channels;

	static int numTexturesLoaded;

	void BindTexture2D();
	void BindTextureCubemap();
};