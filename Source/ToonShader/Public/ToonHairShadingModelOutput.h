// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpressionCustomOutput.h"
#include "ToonHairShadingModelOutput.generated.h"

/**
 * 
 */
UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UToonHairShadingModelOutput : public UMaterialExpressionCustomOutput
{
	GENERATED_BODY()
public:
	UToonHairShadingModelOutput(const FObjectInitializer& ObjectInitializer);

	enum class EToonHairShadingModelOutput
	{
		TightenSpecular = 0,
		Scatter = 1,
		SpecularOffset = 2,
	};
	
	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput TightenSpecular;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput Scatter;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput SpecularOffset;
	
#if WITH_EDITOR
	int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	void GetCaption(TArray<FString>& OutCaptions) const override;
	uint32 GetInputType(int32 InputIndex) override;
	FExpressionInput* GetInput(int32 InputIndex) override;
#endif
	int32 GetNumOutputs() const override { return 3; }
	FString GetFunctionName() const override { return TEXT("GetToonHairShading"); }
	FString GetDisplayName() const override { return TEXT("Toon Hair Shading"); }
};
