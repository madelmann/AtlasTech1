
#include "ObjectManager.h"

#include "Main.h"


CObjectManager::CObjectManager()
: mActive(0)
{
}

CObjectManager::~CObjectManager()
{
	clear();
}

void CObjectManager::activate(unsigned int activeId)
{
	mActive = activeId;
}

void CObjectManager::clear()
{
	mObjects.clear();
}

void CObjectManager::setMaterial(string material)
{
	mObjects.back().Material = material;
}

void CObjectManager::setMesh(string mesh)
{
	mObjects.back().Mesh = mesh;
}

void CObjectManager::setMeshType(string meshType)
{
	mObjects.back().MeshType = meshType;
}

void CObjectManager::setNormalMap(string normalMap)
{
	mObjects.back().NormalMap = normalMap;
}

void CObjectManager::setShader(string shader)
{
	mObjects.back().Shader = shader;
}

void CObjectManager::setTexture(string texture)
{
	mObjects.back().Texture = texture;
}

void CObjectManager::spawn(string filename)
{
	if ( !filename.length() ) {
		return;
	}

	GameObject_t object;

	mObjects.push_back(object);
	mActive = mObjects.size();

	process(filename);
}

void CObjectManager::process(string filename)
{
	FILE *pFile;
	char fname[255] = "";
	char buffer[255] = "";

	sprintf(fname, "%sMaps\\%s\\Objects\\%s.atlasobject", g_sGameName, Game.sMapName, filename.c_str());
	
	if(fopen_s(&pFile, fname, "rt") != NULL)
	{
		sprintf(fname, "%sObjects\\%s.atlasobject", g_sGameName, filename.c_str());

		if(fopen_s(&pFile, fname, "rt") != NULL)
		{
			sprintf(buffer, "ERROR[ObjectManager::Spawn]: '%s' not found!", fname);
			Console->Output(buffer);
			return;
		}
	}

	char line[255] = "";
	char command[255] = "";

	do
	{
		sprintf(line, "");
		readstr(pFile, line);
		sprintf(command, "OBJECT.%s", line);

		ProcessCommand(command);

	} while ( strcmp(line, "") );

	fclose(pFile);


	char *mesh = const_cast<char*>(mObjects.back().Mesh.c_str());
	char *ext = const_cast<char*>(mObjects.back().MeshType.c_str());

	int id = -1;
	CGraphicObject *go = NULL;

	if ( strcmpi(ext, "3ds") == 0 ) {
		id = g_Manager_3DSObject.Add(mesh);

		if ( id != -1 ) {
			go = g_Manager_3DSObject.Mesh[id];
		}
	}
	else if ( strcmpi(ext, "md2") == 0 ) {
		id = g_Manager_Entity.Add(mesh, mesh);

		if ( id != -1 ) {
			go = &g_Manager_Entity.Object[id];
		}
	}
	else if ( strcmpi(ext, "static") == 0 ) {
		id = g_Manager_StaticObject.Add(mesh);

		if ( id != -1 ) {
			go = &g_Manager_StaticObject.Object[id];
		}
	}

	if ( !go ) {
		return;
	}

	go->setMaterial(g_Manager_Material.Add(const_cast<char*>(mObjects.back().Material.c_str())));
	go->setNormalMap(g_Manager_Texture->CreateTexture(const_cast<char*>(mObjects.back().NormalMap.c_str())));
	go->setShader(Shader->Add(const_cast<char*>(mObjects.back().Shader.c_str())));
	go->setTexture(g_Manager_Texture->CreateTexture(const_cast<char*>(mObjects.back().Texture.c_str())));
	go->PreProcess();

	Scene.Add(go);
}