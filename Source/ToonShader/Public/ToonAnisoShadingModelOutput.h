// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpressionCustomOutput.h"
#include "ToonAnisoShadingModelOutput.generated.h"

/**
 * 
 */
UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UToonAnisoShadingModelOutput : public UMaterialExpressionCustomOutput
{
	GENERATED_BODY()
public:
	UToonAnisoShadingModelOutput(const FObjectInitializer& ObjectInitializer);

	enum class EToonAnisoShadingModelOutput
	{
		Anisotropy = 0,
		AnisotropicRoughness = 1,
		ShadowColor = 2,
		TangentRotation = 3,
	};
	
	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput Anisotropy;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput AnisotropicRoughness;

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
	int32 GetNumOutputs() const override { return 4; }
	FString GetFunctionName() const override { return TEXT("GetToonAnisoShading"); }
	FString GetDisplayName() const override { return TEXT("Toon Aniso Shading"); }
};
