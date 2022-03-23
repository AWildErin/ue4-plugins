// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"

#include "Serialization/MemoryReader.h"

#include "../RMesh/RMeshFile.h"

#include "RMeshFactory.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRMeshFactory, Log, All);

/**
 * 
 */
UCLASS()
class CONTENTIMPORTER_API URMeshFactory : public UFactory
{
	GENERATED_BODY()

public:
	
	URMeshFactory(const FObjectInitializer& ObjectInitializer);

	// Begin UFactory Interface
	//virtual bool DoesSupportClass(UClass* Class) override;

	//virtual UClass* ResolveSupportedClass() override;

	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& FileName,
		const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	// End UFactory Interface

private:
	UStaticMesh* CreateStaticMesh(UObject* InParent, FName InName, EObjectFlags, RMeshFile rmesh);
};
