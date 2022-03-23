// Fill out your copyright notice in the Description page of Project Settings.

#include "NiflyMathHelper.h"

FVector NiflyMathHelper::Vec3ToFVector(const nifly::Vector3 inVec)
{
    FVector newVec = FVector();
    newVec.X = inVec.x;
    newVec.Y = inVec.y;
    newVec.Z = inVec.z;

    return newVec;
}

FVector NiflyMathHelper::TriToFVector(const nifly::Triangle inTri)
{
    FVector newVec = FVector();
    newVec.X = inTri.p1;
    newVec.Y = inTri.p2;
    newVec.Z = inTri.p3;

    return newVec;
}

FVector2D NiflyMathHelper::Vec2ToFVector2D(const nifly::Vector2 inVec)
{
    FVector2D newVec = FVector2D();
    newVec.X = inVec.u;
    newVec.Y = inVec.v;

    return newVec;
}

FVector4 NiflyMathHelper::Vec4ToFVector4(const nifly::Vector4 inVec)
{
    FVector4 newVec = FVector4();
    newVec.X = inVec.x;
    newVec.Y = inVec.y;
    newVec.Z = inVec.z;
    newVec.W = inVec.w;

    return newVec;
}

FVector NiflyMathHelper::Mat3ToFVector(const nifly::Matrix3 inMat)
{
    return Vec3ToFVector(nifly::RotMatToVec(inMat));
}

FQuat NiflyMathHelper::Mat3ToFQuat(const nifly::Matrix3 inMat)
{
    FQuat newQuat = FQuat();

    float x;
    float y;
    float z;
    inMat.ToEulerAngles(y, x, z);

    //nifly::Vector3 rot = nifly::RotMatToVec(inMat);
    newQuat.X = x;
    newQuat.Y = y;
    newQuat.Z = z;

    return newQuat;
}

FRotator NiflyMathHelper::Mat3ToFRot(const nifly::Matrix3 inMat)
{
    float p, y, r;
    inMat.ToEulerDegrees(y, p, r);

    FRotator newRot(p, y, r);

    return newRot;
}

FColor NiflyMathHelper::Col3ToFColor(const nifly::Color3 inCol)
{
    FColor newCol = FColor();
    newCol.R = inCol.r;
    newCol.G = inCol.g;
    newCol.B = inCol.b;

    return newCol;
}

FColor NiflyMathHelper::Col4ToFColor(const nifly::Color4 inCol)
{
    FColor newCol = FColor();
    newCol.R = inCol.r;
    newCol.G = inCol.g;
    newCol.B = inCol.b;
    newCol.A = inCol.a;

    return newCol;
}

FTransform NiflyMathHelper::MatTransToFTrans(const nifly::MatTransform inMatTrans)
{
    FTransform Trans = FTransform();
    Trans.SetLocation(Vec3ToFVector(inMatTrans.translation));
    Trans.SetRotation(Mat3ToFQuat(inMatTrans.rotation));
    Trans.SetScale3D(FVector(inMatTrans.scale));

    return Trans;
}
