#pragma once

#include <vector>

#include "Model.h"
#include "Mesh.h"
#include "Vertex.h"

class Terrain {
public: 
	static Model Generate(std::string texture_path);
};