// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Materials/MaterialExpressionCustomOutput.h"
#include "ToonShadingModelOutput.generated.h"

/* 
*
*/
UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UToonShadingModelOutput : public UMaterialExpressionCustomOutput
{
	GENERATED_BODY()
public:
	UToonShadingModelOutput(const FObjectInitializer& ObjectInitializer);

	enum class EToonShadingModelOutput
	{
		ShadowLightener = 0,
		Offset = 1,
		SpecularOffset = 2,
		SpecularRange = 3,
	};
	
	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput ShadowLightener;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput Offset;

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
	int32 GetNumOutputs() const override { return 4; }
	FString GetFunctionName() const override { return TEXT("GetToonShading"); }
	FString GetDisplayName() const override { return TEXT("Toon Shading"); }
};
