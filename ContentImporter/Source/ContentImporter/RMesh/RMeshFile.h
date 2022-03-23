/*
	Credit goes to Juan from Undertow for this entire code.
	They created a fragmotion plugin and released the source code,
	without it I wouldn't have been able to write this at all.
	As such, this code contains a large portion of his code as well.

	todo: Migrate from wstring to string
	todo: add collision mesh
	todo: add mat data ids and face normals
*/

#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

struct Vector3
{
	float X, Y, Z;
};

struct Vector2
{
	float U, V;
};

struct Material
{
	std::wstring Name;
	std::vector<std::string> Textures;
};

struct Triangle
{
	float p1, p2, p3;
	int TextureIndex;
};

struct Model
{
	std::vector<Vector3>* GetVerts() { return &Vertices; }
	std::vector<Vector2>* GetUVs() { return &UVs; }
	std::vector<Triangle>* GetTris() { return &Triangles; }
	std::vector<Material>* GetMats() { return &Materials; }

private:
	std::vector<Vector3> Vertices;
	std::vector<Vector2> UVs;
	std::vector<Triangle> Triangles;
	std::vector<Material> Materials;
};

class RMeshFile
{
public:
	RMeshFile() = default;

	RMeshFile(const std::filesystem::path& filePath) { Load(filePath); }
	RMeshFile(std::ifstream& file) { Load(file); }

	int Load(const std::filesystem::path& filePath);
	int Load(std::ifstream& file);

	float RoomScale = 1.0f;

	std::vector<Vector3>* GetVerts() { return &Vertices; }
	std::vector<Vector2>* GetUVs() { return &UVs; }
	std::vector<Triangle>* GetTris() { return &Triangles; }
	std::vector<Material>* GetMats() { return &Materials; }

private:
	std::vector<Vector3> Vertices;
	std::vector<Vector2> UVs;
	std::vector<Triangle> Triangles;
	std::vector<Material> Materials;
};