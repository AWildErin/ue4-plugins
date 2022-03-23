// Fill out your copyright notice in the Description page of Project Settings.

#include "RMeshFactory.h"

#include "Misc/FileHelper.h"
#include "EditorFramework/AssetImportData.h"

#include <RawMesh.h>
#include <Engine/Polys.h>

URMeshFactory::URMeshFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bText = false;
	bEditorImport = true;
	Formats.Add(TEXT("rmesh;Room Mesh File"));
	SupportedClass = UStaticMesh::StaticClass();
}

UObject* URMeshFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& FileName,
	const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Parms);

	const FString FilePath = FPaths::ConvertRelativePathToFull(FileName);
	
	RMeshFile file;
	file.RoomScale = 0.75f;
	file.Load(TCHAR_TO_UTF8(*FilePath));

	UStaticMesh* StaticMesh = CreateStaticMesh(InParent, InName, Flags, file);
	StaticMesh->MarkPackageDirty();
	StaticMesh->PostEditChange();

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, StaticMesh);

	return StaticMesh;
}

UStaticMesh* URMeshFactory::CreateStaticMesh(UObject* InParent, FName InName, EObjectFlags, RMeshFile rmesh)
{
	std::vector<Vector3>* verts = rmesh.GetVerts();
	std::vector<Triangle>* tris = rmesh.GetTris();
	std::vector<Vector2>* uvs = rmesh.GetUVs();
	std::vector<Material>* mats = rmesh.GetMats();

	UStaticMesh* mesh = NewObject<UStaticMesh>(InParent, InName, RF_Public | RF_Standalone);
	mesh->AddToRoot();

	// Add material groups
	for (int i = 0; i < mats->size(); i++)
	{
		Material mat = mats->at(i);

		FString texName;
		for (int j = 0; j < mat.Textures.size(); j++)
		{
			texName = FPaths::GetBaseFilename(UTF8_TO_TCHAR(mat.Textures.at(j).c_str()));

			// We don't count lightmaps here because they are not needed in UE4
			if (texName.Contains("_lm"))
				continue;
			else
				break;
		}

		FStaticMaterial staticMat = FStaticMaterial(nullptr, FName(*texName));

		//if (mesh->StaticMaterials.Contains(staticMat))
		//	continue;
		//else
			mesh->StaticMaterials.Add(staticMat);
	}

	FRawMesh rawMesh;
	rawMesh.Empty();

	// Add verts
	for (int i = 0; i < verts->size(); i++)
	{
		//FTransform trans;

		FVector vert;
		Vector3 rmeshVert = verts->at(i);
		
		// Make a transform with a flipped X axis
		//trans.SetScale3D(FVector(-1, 1, 1));

		vert.X = rmeshVert.X;
		vert.Y = rmeshVert.Y;
		vert.Z = rmeshVert.Z;

		FRotator rot = FRotator(0, 0, -90);

		// Rooms are rolled by 90d, so we correct that here.
		vert = rot.RotateVector(vert);

		// Transform the vert with the scaled transform
		//rawMesh.VertexPositions.Add(trans.TransformPosition(vert));
		rawMesh.VertexPositions.Add(vert);
	}

	// Add tris and all that other stuff
	FVector emptyVector = FVector(0, 0, 0);
	for (int i = 0; i < tris->size(); i++)
	{
		Triangle tri = tris->at(i);
		rawMesh.WedgeIndices.Add(tri.p1);
		rawMesh.WedgeIndices.Add(tri.p2);
		rawMesh.WedgeIndices.Add(tri.p3);

		Vector2 rawUV1 = uvs->at(tri.p1);
		Vector2 rawUV2 = uvs->at(tri.p2);
		Vector2 rawUV3 = uvs->at(tri.p3);

		FVector2D uv1 = FVector2D(rawUV1.U, rawUV1.V);
		FVector2D uv2 = FVector2D(rawUV2.U, rawUV2.V);
		FVector2D uv3 = FVector2D(rawUV3.U, rawUV3.V);

		rawMesh.WedgeTexCoords[0].Add(uv1);
		rawMesh.WedgeTexCoords[0].Add(uv2);
		rawMesh.WedgeTexCoords[0].Add(uv3);

		rawMesh.WedgeTangentX.Add(emptyVector);
		rawMesh.WedgeTangentX.Add(emptyVector);
		rawMesh.WedgeTangentX.Add(emptyVector);

		rawMesh.WedgeTangentY.Add(emptyVector);
		rawMesh.WedgeTangentY.Add(emptyVector);
		rawMesh.WedgeTangentY.Add(emptyVector);

		rawMesh.WedgeTangentZ.Add(emptyVector);
		rawMesh.WedgeTangentZ.Add(emptyVector);
		rawMesh.WedgeTangentZ.Add(emptyVector);

		rawMesh.FaceMaterialIndices.Add(tri.TextureIndex);
		rawMesh.FaceSmoothingMasks.Add(0);
	}

	FStaticMeshSourceModel& sourceModel = mesh->AddSourceModel();
	sourceModel.BuildSettings.bRecomputeNormals = true;
	sourceModel.BuildSettings.bRecomputeTangents = true;
	sourceModel.BuildSettings.bUseMikkTSpace = false;
	sourceModel.BuildSettings.bGenerateLightmapUVs = true;
	sourceModel.BuildSettings.bBuildAdjacencyBuffer = false;
	sourceModel.BuildSettings.bBuildReversedIndexBuffer = false;
	sourceModel.BuildSettings.bUseFullPrecisionUVs = true;
	sourceModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
	sourceModel.RawMeshBulkData->SaveRawMesh(rawMesh);

	mesh->LightingGuid = FGuid::NewGuid();
	mesh->LightMapResolution = 512;
	mesh->LightMapCoordinateIndex = 1;

	mesh->CreateBodySetup();

	TArray<FText> outErrors;
	mesh->Build(false, &outErrors);

	return mesh;
}
