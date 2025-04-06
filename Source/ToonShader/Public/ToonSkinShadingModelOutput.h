// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Materials/MaterialExpressionCustomOutput.h"
#include "ToonSkinShadingModelOutput.generated.h"

/* 
*
*/
UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UToonSkinShadingModelOutput : public UMaterialExpressionCustomOutput
{
	GENERATED_BODY()
public:
	UToonSkinShadingModelOutput(const FObjectInitializer& ObjectInitializer);

	enum class EToonSkinShadingModelOutput
	{
		SSSSwitch = 0,
		offset = 1,
		ShadowColor = 2,
		SpecularOffset = 3,
		SpecularRange = 4,
	};
	
	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput SSSSwitch;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput offset;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput ShadowColor;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput SpecularOffset;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput SpecularRange;
	
#if WITH_EDITOR
	int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	void GetCaption(TArray<FString>& OutCaptions) const override;
	uint32 GetInputType(int32 InputIndex) override;
	FExpressionInput* GetInput(int32 InputIndex) override;
#endif
	int32 GetNumOutputs() const override { return 5; }
	FString GetFunctionName() const override { return TEXT("GetToonSkinShading"); }
	FString GetDisplayName() const override { return TEXT("Toon Skin Shading"); }
};
