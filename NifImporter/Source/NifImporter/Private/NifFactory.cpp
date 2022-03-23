// Fill out your copyright notice in the Description page of Project Settings.

#include "NifFactory.h"
#include "NiflyMathHelper.h"

#include <RawMesh.h>
#include <Engine/Polys.h>

DEFINE_LOG_CATEGORY(LogNifFactory);

UNifFactory::UNifFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bText = false;
	bEditorImport = true;
	Formats.Add(TEXT("nif;Bethesda Art File"));
	SupportedClass = UStaticMesh::StaticClass();
}

UObject* UNifFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename,
										const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	const FString Filepath = FPaths::ConvertRelativePathToFull(Filename);
	nifly::NifFile nif;
	nif.Load(TCHAR_TO_UTF8(*Filepath));

	UStaticMesh* staticMesh = CreateStaticMesh(InParent, InName, Flags, nif);

	return staticMesh;

	/*
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Parms);

	const FString Filepath = FPaths::ConvertRelativePathToFull(Filename);
	UE_LOG(LogNifFactory, Warning, TEXT("%s"), *Filepath);

	UStaticMesh* Object = nullptr;
	Object = NewObject<UStaticMesh>(InParent, InClass, InName, RF_Public | RF_Standalone);
	Object->AddToRoot();

	nifly::NifFile nif;
	nif.Load(TCHAR_TO_UTF8(*Filepath));

	UE_LOG(LogNifFactory, Warning, TEXT("%s"), UTF8_TO_TCHAR(nif.GetNodeName(0).c_str()));

	std::vector<std::string> shapeList = nif.GetShapeNames();
	for (size_t i = 0; i < shapeList.size(); i++)
	{
		const uint8_t MAX_TEXTURE_PATHS = 10;
		std::vector<std::string> texFiles(MAX_TEXTURE_PATHS);

		std::string shapeName = shapeList[i];
		auto shape = nif.FindBlockByName<nifly::NiShape>(shapeName);
		if (!shape)
		{
			UE_LOG(LogNifFactory, Error, TEXT("Couldn't find shape: %s"), UTF8_TO_TCHAR(nif.GetNodeName(0).c_str()));
			continue;
		}

		std::vector<nifly::Vector3> nifVerts;
		std::vector<nifly::Triangle> nifTris;

		nif.GetVertsForShape(shape, nifVerts);
		shape->GetTriangles(nifTris);

		const std::vector<nifly::Vector3>* nifNorms = nif.GetNormalsForShape(shape);
		const std::vector<nifly::Vector2>* nifUvs = nif.GetUvsForShape(shape);

		auto test = static_cast<nifly::NiTriStripsData*>(shape->GetGeomData());
		auto stripsInfo = test->stripsInfo;

		FRawMesh rawMesh;

		UE_LOG(LogNifFactory, Warning, TEXT("Adding mesh: %s"), UTF8_TO_TCHAR(shapeName.c_str()));

		// verts
		for (int j = 0; j < nifVerts.size(); j++)
		{
			FVector vertex = NiflyMathHelper::Vec3ToFVector((nifVerts)[j]);
			rawMesh.VertexPositions.Add(vertex);
			//UE_LOG(LogNifFactory, Warning, TEXT("Adding %f %f %f"), vertex.X, vertex.Y, vertex.Z);
		}

		// uvs
		// todo: this can have more than 1 uv set
		for (int j = 0; j < nifUvs->size(); j++)
		{
			auto uv = FVector2D();
			uv.X = (*nifUvs)[j].u;
			uv.Y = (*nifUvs)[j].v;

			rawMesh.WedgeTexCoords[0].Add(uv);
		}

		rawMesh.WedgeIndices.SetNumUninitialized(stripsInfo.points[0].size());
		for (int j = 0; j < stripsInfo.points[0].size(); j++)
		{
			//UE_LOG(LogNifFactory, Warning, TEXT("Adding %i"), stripsInfo.points[0][j]);
			rawMesh.WedgeIndices[j] = (stripsInfo.points[0][j]);
		}


		nifly::NiShader* shader = nif.GetShader(shape);
		if (shader)
		{
			for (int j = 0; j < MAX_TEXTURE_PATHS; j++)
			{
				nif.GetTextureSlot(shape, texFiles[j], j);
				FString texName = FPaths::GetBaseFilename(UTF8_TO_TCHAR(texFiles[j].c_str()));

				UE_LOG(LogNifFactory, Warning, TEXT("Texture: %s"), *texName);				
			}
		}

		FStaticMeshSourceModel& sourceModel = Object->AddSourceModel();

		sourceModel.RawMeshBulkData->SaveRawMesh(rawMesh);
		Object->CreateBodySetup();
		Object->Build();
	}

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);
	Object->PostEditChange();
	return Object;

	//UStaticMesh* NewAsset = NewObject<UStaticMesh>(InParent, InClass, InName, Flags | RF_Transactional);
	//return NewAsset;
	*/
}

UStaticMesh* UNifFactory::CreateStaticMesh(UObject* InParent, FName InName, EObjectFlags, nifly::NifFile nif)
{
	UStaticMesh* mesh = NewObject<UStaticMesh>(InParent, InName, RF_Public | RF_Standalone);
	mesh->AddToRoot();

	FRawMesh combinedMesh;

	std::vector<std::string> shapeList = nif.GetShapeNames();
	for (size_t shapeIndex = 0; shapeIndex < shapeList.size(); shapeIndex++)
	{
		std::string shapeName = shapeList[shapeIndex];
		auto shape = nif.FindBlockByName<nifly::NiShape>(shapeName);

		if (!shape)
		{
			UE_LOG(LogNifFactory, Error, TEXT("Couldn't find shape: %s"), UTF8_TO_TCHAR(shapeName.c_str()));
			return nullptr;
		}

		const uint8_t MAX_TEXTURE_PATHS = 10;
		TArray<FString> texFiles;
		nifly::NiShader* shader = nif.GetShader(shape);
		if (shader)
		{
			for (int i = 0; i < MAX_TEXTURE_PATHS; i++)
			{
				std::string texName;
				nif.GetTextureSlot(shape, texName, i);
				if (texName != "")
					texFiles.Add(FPaths::GetBaseFilename(UTF8_TO_TCHAR(texName.c_str())));
				else if (!shader->HasTextureSet())
				{
					// if the material doesn't have a texture set
					// set this to some sort of generic material
					// so it can be overridden in the editor
					texFiles.Add(TEXT("Null Material"));
				}
			}
		}

		std::vector<nifly::Vector3> nifVerts;
		std::vector<nifly::Vector3> nifBitangs;
		std::vector<nifly::Vector3> nifTangs;
		std::vector<nifly::Triangle> nifTris;

		nif.GetVertsForShape(shape, nifVerts);
		nif.GetBitangentsForShape(shape, nifBitangs);
		nif.GetTangentsForShape(shape, nifTangs);
		shape->GetTriangles(nifTris);

		//nifly::NiNode* parent = nif.GetParentNode(shape);
		//nifly::MatTransform shapeTransform = parent->GetTransformToParent().ComposeTransforms(shape->GetTransformToParent());

		const std::vector<nifly::Vector3>* nifNorms = nif.GetNormalsForShape(shape);
		const std::vector<nifly::Vector2>* nifUvs = nif.GetUvsForShape(shape);

		// Todo: maybe move this to a ParseShader method so it can be used in the static and skeletal meshes
		for (int i = 0; i < texFiles.Num(); i++)
		{
			FString name = texFiles[i];
			if (name.EndsWith("_N") || name.EndsWith("_SK") || name.EndsWith("_G")
				|| name.EndsWith("_E") || name.EndsWith("_M") || name.EndsWith("_EM")
				|| name.EndsWith("_I") || name.EndsWith("_S") || name.EndsWith("_P"))
				continue;

			FStaticMaterial mat = FStaticMaterial(nullptr, FName(*name));
			// Check if the material already exists
			if (mesh->StaticMaterials.Contains(mat))
				continue;
			else
			{
				// Add the material
				mesh->StaticMaterials.Add(mat);
			}
		}

		FRawMesh rMesh;
		rMesh.Empty();


		FVector TransVec;
		FRotator Rot;
		if (!shape->IsSkinned())
		{
			nifly::MatTransform ttg = shape->GetTransformToParent();
			nifly::NiNode* parent = nif.GetParentNode(shape);
			while (parent)
			{
				ttg = parent->GetTransformToParent().ComposeTransforms(ttg);
				parent = nif.GetParentNode(parent);
			}

			TransVec = NiflyMathHelper::Vec3ToFVector(ttg.translation);
			Rot = NiflyMathHelper::Mat3ToFRot(ttg.rotation);
		}
		else
		{
			nifly::MatTransform ttg;
			if (!nif.CalcShapeTransformGlobalToSkin(shape, ttg))
			{
				UE_LOG(LogNifFactory, Warning, TEXT("Failed to run CalcShapeTransformGlobalToSkin on: %s"), UTF8_TO_TCHAR(shapeName.c_str()));
			}

			TransVec = NiflyMathHelper::Vec3ToFVector(ttg.translation);
			Rot = NiflyMathHelper::Mat3ToFRot(ttg.rotation);
		}

		TransVec.X *= -1; // Nif requires this to be flipped... I have no idea why
		std::swap(Rot.Yaw, Rot.Roll); // These need to be swapped apparently
		Rot = Rot.GetInverse();

		FTransform Trans = FTransform();
		Trans.SetLocation(TransVec);

		// Add all verts to the raw mesh
		for (int i = 0; i < nifVerts.size(); i++)
		{
			FVector vertex = NiflyMathHelper::Vec3ToFVector((nifVerts)[i]);
			vertex.X *= -1;

			vertex = Rot.RotateVector(vertex);
			rMesh.VertexPositions.Add(Trans.TransformPosition(vertex));
		}

		FVector EmptyVector = FVector(0, 0, 0);
		for (auto i = 0; i < nifTris.size(); i++)
		{
			int ind1 = nifTris[i].p1;
			int ind2 = nifTris[i].p2;
			int ind3 = nifTris[i].p3;
			rMesh.WedgeIndices.Add(ind1);
			rMesh.WedgeIndices.Add(ind2);
			rMesh.WedgeIndices.Add(ind3);

			if (shape->HasUVs())
			{
				rMesh.WedgeTexCoords[0].Add(NiflyMathHelper::Vec2ToFVector2D((*nifUvs)[ind1]));
				rMesh.WedgeTexCoords[0].Add(NiflyMathHelper::Vec2ToFVector2D((*nifUvs)[ind2]));
				rMesh.WedgeTexCoords[0].Add(NiflyMathHelper::Vec2ToFVector2D((*nifUvs)[ind3]));
			}
			else
			{
				UE_LOG(LogNifFactory, Error, TEXT("Could not find UVs for %s"), UTF8_TO_TCHAR(shapeName.c_str()));
				rMesh.WedgeTexCoords[0].Add(FVector2D(0, 1));
				rMesh.WedgeTexCoords[0].Add(FVector2D(0, 1));
				rMesh.WedgeTexCoords[0].Add(FVector2D(0, 1));
			}

			rMesh.WedgeTangentX.Add(EmptyVector);
			rMesh.WedgeTangentX.Add(EmptyVector);
			rMesh.WedgeTangentX.Add(EmptyVector);

			rMesh.WedgeTangentY.Add(EmptyVector);
			rMesh.WedgeTangentY.Add(EmptyVector);
			rMesh.WedgeTangentY.Add(EmptyVector);

			FVector norm1 = NiflyMathHelper::Vec3ToFVector((*nifNorms)[ind1]);
			FVector norm2 = NiflyMathHelper::Vec3ToFVector((*nifNorms)[ind2]);
			FVector norm3 = NiflyMathHelper::Vec3ToFVector((*nifNorms)[ind3]);

			rMesh.WedgeTangentZ.Add(norm1);
			rMesh.WedgeTangentZ.Add(norm2);
			rMesh.WedgeTangentZ.Add(norm3);

			if (texFiles.Num() > 0)
			{
				rMesh.FaceMaterialIndices.Add(mesh->GetMaterialIndex(FName(*texFiles[0])));
			}
			else
			{
				rMesh.FaceMaterialIndices.Add(0);
			}

			rMesh.FaceSmoothingMasks.Add(1);
		}

		if (!MergeRawMesh(rMesh, combinedMesh))
		{
			UE_LOG(LogNifFactory, Error, TEXT("Failed the merge mesh: %s"), UTF8_TO_TCHAR(shapeName.c_str()));
			return nullptr;
		}
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
	sourceModel.RawMeshBulkData->SaveRawMesh(combinedMesh);

	// Lightmap stuff
	mesh->LightingGuid = FGuid::NewGuid();
	mesh->LightMapResolution = 128;
	mesh->LightMapCoordinateIndex = 1;

	mesh->CreateBodySetup();

	/*
	if (!CreateUCXMesh(mesh->BodySetup, nif))
	{
		//UE_LOG(LogNifFactory, Warning, TEXT("Failed to add collision mesh to %s"), mesh->GetFName());
	}
	*/

	TArray<FText> outErrors;
	mesh->Build(false, &outErrors);

	return mesh;
}

bool UNifFactory::MergeRawMesh(FRawMesh& InRawMesh, FRawMesh& OutRawMesh)
{
	if (InRawMesh.WedgeIndices.Num() <= 0 || InRawMesh.WedgeIndices.Num() % 3)
	{
		// The factory handles the error log here!
		return false;
	}

	OutRawMesh.FaceMaterialIndices.Append(InRawMesh.FaceMaterialIndices);
	OutRawMesh.FaceSmoothingMasks.Append(InRawMesh.FaceSmoothingMasks);

	int32 StartIndex = OutRawMesh.VertexPositions.Num();

	for (int32 WedgeIndex : InRawMesh.WedgeIndices)
	{
		OutRawMesh.WedgeIndices.Add(WedgeIndex + StartIndex);
	}

	OutRawMesh.VertexPositions.Append(InRawMesh.VertexPositions);

	OutRawMesh.WedgeTangentX.Append(InRawMesh.WedgeTangentX);
	OutRawMesh.WedgeTangentY.Append(InRawMesh.WedgeTangentY);
	OutRawMesh.WedgeTangentZ.Append(InRawMesh.WedgeTangentZ);

	for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
	{
		OutRawMesh.WedgeTexCoords[i].Append(InRawMesh.WedgeTexCoords[i]);
	}

	OutRawMesh.WedgeColors.Append(InRawMesh.WedgeColors);

	return true;
}

bool UNifFactory::CreateUCXMesh(UBodySetup* InBody, nifly::NifFile nif)
{
	nifly::NiNode* root = nif.GetRootNode();
	nifly::NiHeader hdr = nif.GetHeader();
	nifly::bhkCollisionObject* colObject = hdr.GetBlock<nifly::bhkCollisionObject>(root->collisionRef);

	if (colObject == nullptr)
	{
		return false;
	}

	TArray<FVector> colVerts;
	TArray<int32> colInds;

	// Get Collision shape
	nifly::bhkRigidBody* colBody = hdr.GetBlock<nifly::bhkRigidBody>(colObject->bodyRef);
	nifly::bhkShape* colShape = hdr.GetBlock<nifly::bhkShape>(colBody->shapeRef);
	
	// Check for the various types of collision shapes
	if (colShape->HasType<nifly::bhkMoppBvTreeShape>())
	{
		nifly::bhkMoppBvTreeShape* treeShape = dynamic_cast<nifly::bhkMoppBvTreeShape*>(colShape);
		nifly::bhkPackedNiTriStripsShape* treeStrips = hdr.GetBlock<nifly::bhkPackedNiTriStripsShape>(treeShape->shapeRef);
		nifly::hkPackedNiTriStripsData* treeData = hdr.GetBlock<nifly::hkPackedNiTriStripsData>(treeStrips->dataRef);
		
		for (int i = 0; i < treeData->triData.size(); i++)
		{
			nifly::Triangle tri = treeData->triData[i].tri;

			colInds.Add(tri.p1);
			colInds.Add(tri.p2);
			colInds.Add(tri.p3);
		}

		for (int i = 0; i < treeData->compressedVertData.size(); i++)
		{
			auto vert = treeData->compressedVertData[i];

			colVerts.Add(NiflyMathHelper::Vec3ToFVector(vert));
		}
	}
	else if (colShape->HasType<nifly::bhkConvexVerticesShape>())
	{
		nifly::bhkConvexVerticesShape* convexShape = dynamic_cast<nifly::bhkConvexVerticesShape*>(colShape);
		
	}

	if (colVerts.Num() == 0 || colInds.Num() == 0)
	{
		return false;
	}

	// Gets cleared up at somepoint via GC
	UModel* tempModel = NewObject<UModel>();
	tempModel->Initialize();

	for (int32 i = 0; colInds.Num(); i++)
	{
		FPoly* poly = new(tempModel->Polys->Element) FPoly();
		poly->Init();
		poly->iLink = i;

		new(poly->Vertices)FVector(colVerts[i]);

		poly->CalcNormal(true);
	}

	tempModel->BuildBound();
	
	InBody->CreateFromModel(tempModel, false);

	return true;
}