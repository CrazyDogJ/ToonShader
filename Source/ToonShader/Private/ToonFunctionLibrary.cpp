// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonFunctionLibrary.h"

#include "Components/CapsuleComponent.h"

void UToonFunctionLibrary::SetMaterialSlotsOverlayMaterial(USkeletalMeshComponent* MeshComponent,
                                                           const TArray<UMaterialInterface*>& Materials)
{
	if (MeshComponent == nullptr)
	{
		return;
	}
	MeshComponent->MaterialSlotsOverlayMaterial = Materials;
	MeshComponent->PrecachePSOs();
	MeshComponent->MarkRenderStateDirty();
}

UMaterialInstanceDynamic* UToonFunctionLibrary::CreateAndSetOverlayMaterialInstanceDynamic(USkeletalMeshComponent* MeshComponent, int32 ElementIndex)
{
	TArray<TObjectPtr<class UMaterialInterface>> MaterialInterfaces;
	MeshComponent->GetMaterialSlotsOverlayMaterial(MaterialInterfaces);
	TObjectPtr<UMaterialInterface> MaterialInstance;
	if (MaterialInterfaces.IsValidIndex(ElementIndex))
	{
		MaterialInstance = MaterialInterfaces[ElementIndex];
	}
	
	UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MaterialInstance);

	if (MaterialInstance && !MID)
	{
		// Create and set the dynamic material instance.
		MID = UMaterialInstanceDynamic::Create(MaterialInstance, MeshComponent);
		if (MeshComponent->MaterialSlotsOverlayMaterial.IsValidIndex(ElementIndex))
		{
			MeshComponent->MaterialSlotsOverlayMaterial[ElementIndex] = MID;
		}
	}

	return MID;
}

void UToonFunctionLibrary::SetScalarParameterValueOnOverlayMaterials(USkeletalMeshComponent* MeshComponent,
                                                                     const FName ParameterName, const float ParameterValue)
{
	TArray<TObjectPtr<class UMaterialInterface>> MaterialInterfaces;
	MeshComponent->GetMaterialSlotsOverlayMaterial(MaterialInterfaces);
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialInterfaces.Num(); ++MaterialIndex)
	{
		if (UMaterialInterface* MaterialInterface = MaterialInterfaces[MaterialIndex])
		{
			UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MaterialInterface);
			if (!DynamicMaterial)
			{
				DynamicMaterial = CreateAndSetOverlayMaterialInstanceDynamic(MeshComponent, MaterialIndex);
			}
			DynamicMaterial->SetScalarParameterValue(ParameterName, ParameterValue);
		}
	}
}

FVector UToonFunctionLibrary::FindClosestPointOnCapsule(const FVector& Point, const UCapsuleComponent* Capsule, bool& bIsInCapsule)
{
	// 胶囊体的上半球中心和下半球中心
	FVector CapsuleLocation = Capsule->GetComponentLocation();
	FVector CapsuleTop = CapsuleLocation + Capsule->GetScaledCapsuleHalfHeight() * Capsule->GetUpVector();
	FVector CapsuleBottom = CapsuleLocation - Capsule->GetScaledCapsuleHalfHeight() * Capsule->GetUpVector();

	// 将点投影到胶囊体轴上
	FVector PointProjection = FMath::ClosestPointOnSegment(Point, CapsuleBottom, CapsuleTop);

	// 计算从投影点到输入点的向量
	FVector ToPoint = Point - PointProjection;

	bIsInCapsule = ToPoint.Length() <= Capsule->GetScaledCapsuleRadius();
	ToPoint = ToPoint.GetSafeNormal(); // 归一化向量

	// 返回最近点
	return PointProjection + ToPoint * Capsule->GetScaledCapsuleRadius();
}

float UToonFunctionLibrary::GetDistanceToCapsule(const FVector& Location, const UCapsuleComponent* Capsule)
{
	if (!Capsule)
	{
		return -1.0f;
	}

	// 计算胶囊体上最近的点
	bool bIsInCapsule;
	FVector ClosestPointOnCapsule = FindClosestPointOnCapsule(Location, Capsule, bIsInCapsule);

	// 计算距离
	float Distance = FVector::Dist(Location, ClosestPointOnCapsule);
	Distance *= bIsInCapsule ? -1 : 1;

	return Distance;
}