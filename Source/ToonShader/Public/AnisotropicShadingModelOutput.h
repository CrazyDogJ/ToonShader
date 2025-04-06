// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpressionCustomOutput.h"
#include "AnisotropicShadingModelOutput.generated.h"

/**
 * 
 */
UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UAnisotropicShadingModelOutput : public UMaterialExpressionCustomOutput
{
	GENERATED_BODY()
public:
	UAnisotropicShadingModelOutput(const FObjectInitializer& ObjectInitializer);

	enum class EAnisotropicShadingModelOutput
	{
		Anisotropy = 0,
		ShadowColor = 1,
		TangentRotation = 2,
	};
	
	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput Anisotropy;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput ShadowColor;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput TangentRotation;
	
#if WITH_EDITOR
	int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	void GetCaption(TArray<FString>& OutCaptions) const override;
	uint32 GetInputType(int32 InputIndex) override;
	FExpressionInput* GetInput(int32 InputIndex) override;
#endif
	int32 GetNumOutputs() const override { return 3; }
	FString GetFunctionName() const override { return TEXT("GetAnisotropicShading"); }
	FString GetDisplayName() const override { return TEXT("Anisotropic Shading"); }
};
