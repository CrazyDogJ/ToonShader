// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ToonFunctionLibrary.generated.h"

UCLASS()
class TOONSHADER_API UToonFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Toon Shading")
	static void SetMaterialSlotsOverlayMaterial(USkeletalMeshComponent* MeshComponent, const TArray<UMaterialInterface*>& Materials);

	static UMaterialInstanceDynamic* CreateAndSetOverlayMaterialInstanceDynamic(USkeletalMeshComponent* MeshComponent, int32 ElementIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Toon Shading")
	static void SetScalarParameterValueOnOverlayMaterials(USkeletalMeshComponent* MeshComponent, const FName ParameterName, const float ParameterValue);
};
