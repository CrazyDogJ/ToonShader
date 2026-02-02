// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonFunctionLibrary.h"

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
