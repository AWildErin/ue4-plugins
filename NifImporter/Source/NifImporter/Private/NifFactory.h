// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Editor/UnrealEd/Public/EditorReimportHandler.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Factories/Factory.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "PhysicsEngine/PhysicsAsset.h"

#include <NifFile.hpp>
#include <bhk.hpp>

#include "NifFactory.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNifFactory, Log, All);

/**
 * 
 * @brief Handles importing `.nif` files into UE4 with the correct collision, animations and materials
 */
UCLASS()
class UNifFactory : public UFactory//, public FReimportHandler
{
	GENERATED_UCLASS_BODY()
public:
	//UNifFactory(const FObjectInitializer& ObjectInitializer);

	// Begin UFactory Interface

	//virtual bool DoesSupportClass(UClass* Class) override;

	//virtual UClass* ResolveSupportedClass() override;

	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename,
										const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

	// End UFactory Interface

	// Begin FReimportHandler Interface
	//virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;

	//virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;

	//virtual EReimportResult::Type Reimport(UObject* Obj) override;
	// End FReimportHandler Interface

private:
	//! @todo Add submeshes along with their location and rotation data
	//! @todo Add collision data to the mesh
	//! @todo Add material data [DONE]
	UStaticMesh* CreateStaticMesh(UObject* InParent, FName InName, EObjectFlags, nifly::NifFile nif);

	//USkeletalMesh* CreateSkeletalMesh(UObject* InParent, FName InName, EObjectFlags, const nifly::NifFile* nif);

	//! Merges `InRawMesh` into the specified `OutRawMesh`, returns `false` on failure to do so
	bool MergeRawMesh(FRawMesh& InRawMesh, FRawMesh& OutRawMesh);

	
	bool CreateUCXMesh(UBodySetup* InBody, nifly::NifFile nif);
	//bool ParseMaterials(UStaticMesh& mesh, nifly::NifFile& nif, nifly::NiShape& shape);
};
