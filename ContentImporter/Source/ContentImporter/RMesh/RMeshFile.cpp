#include "RMeshFile.h"

#include "../Utility/StreamHelper.h"

#include <iostream>

using namespace std;

int RMeshFile::Load(const std::filesystem::path& filePath)
{
	ifstream file(filePath, std::ios::in | std::ios::binary);

	return Load(file);
}

int RMeshFile::Load(std::ifstream& file)
{
	if (!file)
	{
		// failed to load the file
		cout << "Failed to load RMesh file\n";
		return 1;
	}

	char headerBuf[255];
	StreamHelper::ReadString(file, headerBuf);
	string headerStr = headerBuf;

	if (headerStr != "RoomMesh" && headerStr != "RoomMesh.HasTriggerBox")
	{
		cout << "Invalid Header\n";
		return 1;
	}

	int surfaceCount = StreamHelper::ReadInt(file);

	for (int i = 0; i < surfaceCount; i++)
	{
		wstring matName = L"drawnmesh" + to_wstring(i);

		Material mat;
		mat.Name = matName;
		mat.Textures.clear();
		for (int j = 0; j < 2; j++)
		{
			char blendType = StreamHelper::ReadByte(file);

			if (blendType != 0)
			{
				char texName[4096];
				StreamHelper::ReadString(file, texName);

				string texNameStr = string(texName);
				//wstring str2(texNameStr.length(), L' ');

				//copy(texNameStr.begin(), texNameStr.end(), str2.begin());

				if (texName[0] != 0)
				{
					mat.Textures.push_back(texNameStr);
				}
			}
		}

		Materials.push_back(mat);
		int vertCount = StreamHelper::ReadInt(file);

		for (int j = 0; j < vertCount; j++)
		{
			Vector3 vert;
			vert.X = StreamHelper::ReadFloat(file) * RoomScale;
			vert.Y = StreamHelper::ReadFloat(file) * RoomScale;
			vert.Z = -StreamHelper::ReadFloat(file) * RoomScale;

			Vector2 texCoord;
			texCoord.U = StreamHelper::ReadFloat(file);
			texCoord.V = StreamHelper::ReadFloat(file);

			StreamHelper::ReadFloat(file);
			StreamHelper::ReadFloat(file);

			char r, g, b;
			r = StreamHelper::ReadByte(file);
			g = StreamHelper::ReadByte(file);
			b = StreamHelper::ReadByte(file);

			Vertices.push_back(vert);
			UVs.push_back(texCoord);
		}

		int polyCount = StreamHelper::ReadInt(file);
		for (int j = 0; j < polyCount; j++)
		{
			Triangle tri;

			tri.p1 = StreamHelper::ReadInt(file) + Vertices.size() - vertCount;
			tri.p2 = StreamHelper::ReadInt(file) + Vertices.size() - vertCount;
			tri.p3 = StreamHelper::ReadInt(file) + Vertices.size() - vertCount;

			// Add the current surface ID as the texture ID
			tri.TextureIndex = i;

			Triangles.push_back(tri);
		}
	}

	return 0;
}
