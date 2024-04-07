
#pragma once

// Library includes
#include <string>
#include <vector>

// Project includes


using namespace std;


typedef struct GameObject {
	float Scale;
	string Material;
	string Mesh;
	string MeshType;
	string NormalMap;
	string Shader;
	string Texture;
} GameObject_t;


class CObjectManager
{
public:
	CObjectManager();
	~CObjectManager();

	void activate(unsigned int activeId);
	void clear();
	void setMaterial(string material);
	void setMesh(string mesh);
	void setMeshType(string meshType);
	void setNormalMap(string normalMap);
	void setShader(string shader);
	void setTexture(string texture);
	void spawn(string filename);

protected:

private:
	void process(string filename);
	
	unsigned int mActive;
	vector<GameObject_t> mObjects;
};
