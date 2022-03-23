// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <NifFile.hpp>

/**
 * Handles the conversion from Nifly types to UE4 types
 */
class NiflyMathHelper
{
public:
	// Vectors
	//! Converts Nifly::Vector3 to FVector
	static FVector Vec3ToFVector(const nifly::Vector3 inVec);
	//! Converts Nifly::Triangle to FVector
	static FVector TriToFVector(const nifly::Triangle inTri);
	//! Converts Nifly::Vector2 to FVector2D
	static FVector2D Vec2ToFVector2D(const nifly::Vector2 inVec);
	//! Converts Nifly::Vector4 to FVector4
	static FVector4 Vec4ToFVector4(const nifly::Vector4 inVec);

	// Matrix
	//! Converts Nifly::Matrix3 to FVector
	static FVector Mat3ToFVector(const nifly::Matrix3 inMat);
	//! Converts Nifly::Matrix3 to FQuart
	static FQuat Mat3ToFQuat(const nifly::Matrix3 inMat);
	//! Converts Nifly::Matrix3 to FRotator
	static FRotator Mat3ToFRot(const nifly::Matrix3 inMat);

	// Color
	//! Converts Nifly::Color3 to FColor
	static FColor Col3ToFColor(const nifly::Color3 inCol);
	//! Converts Nifly::Color4 to FColor
	static FColor Col4ToFColor(const nifly::Color4 inCol);

	// Transform
	static FTransform MatTransToFTrans(const nifly::MatTransform inMatTrans);
};
