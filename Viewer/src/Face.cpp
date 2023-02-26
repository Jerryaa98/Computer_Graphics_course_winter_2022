#pragma once
#include <istream>
#include "Face.h"
/*
Face::Face(std::istream& issLine)
{
	vertexIndices = { 0,0,0 };
	normalIndices = { 0,0,0 };
	textureIndices = { 0,0,0 };

	char c;
	for (int i = 0; i < 3; i++)
	{
		issLine >> std::ws >> vertexIndices.at(i) >> std::ws;
		if (issLine.peek() != '/')
		{
			continue;
		}
		
		issLine >> c >> std::ws;
		if (issLine.peek() == '/')
		{
			issLine >> c >> std::ws >> normalIndices.at(i);
			continue;
		}

		issLine >> textureIndices.at(i);
		if (issLine.peek() != '/')
		{
			continue;
		}
		
		issLine >> c >> normalIndices.at(i);
	}
}


const int Face::GetVertexIndex(int internal_index) {
	return vertexIndices[internal_index];
}

const int Face::GetNormalIndex(int internal_index) {
	return normalIndices[internal_index];
}

const int Face::GetTextureIndex(int internal_index) {
	return textureIndices[internal_index];
}



*/
