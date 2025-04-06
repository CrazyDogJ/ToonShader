// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonSkinShadingModelOutput.h"

#if WITH_EDITOR
#include "MaterialCompiler.h"
#endif

#define LOCTEXT_NAMESPACE "ToonSkinShadingModelOutput"

UToonSkinShadingModelOutput::UToonSkinShadingModelOutput(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	static FText NAME_Category = LOCTEXT("Toon", "Toon");
	MenuCategories.Add(NAME_Category);

	bCollapsed = true;

	// No outputs
	Outputs.Reset();
#endif
}

#if WITH_EDITOR
int32 UToonSkinShadingModelOutput::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	auto CompileExpressionInput = [&](FExpressionInput& ExpressionInput, float DefaultValue)
	{
		if (ExpressionInput.GetTracedInput().Expression)
		{
			return Compiler->CustomOutput(this, OutputIndex, ExpressionInput.Compile(Compiler));
		}
		return Compiler->CustomOutput(this, OutputIndex, Compiler->Constant(DefaultValue));
	};
	
	switch (EToonSkinShadingModelOutput(OutputIndex))
	{
	case EToonSkinShadingModelOutput::SSSSwitch:
		return CompileExpressionInput(SSSSwitch, 0.f);
	case EToonSkinShadingModelOutput::offset:
		return CompileExpressionInput(offset, 0.f);
	case EToonSkinShadingModelOutput::ShadowColor:
		return CompileExpressionInput(ShadowColor, 0.f);
	case EToonSkinShadingModelOutput::SpecularOffset:
		return CompileExpressionInput(SpecularOffset, 0.f);
	case EToonSkinShadingModelOutput::SpecularRange:
		return CompileExpressionInput(SpecularRange, 0.f);
	}
	return CompilerError(Compiler, TEXT("Input missing"));
}

void UToonSkinShadingModelOutput::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("ToonSkinShading")));
}

uint32 UToonSkinShadingModelOutput::GetInputType(int32 InputIndex)
{
	switch (EToonSkinShadingModelOutput(InputIndex))
	{
	case EToonSkinShadingModelOutput::SSSSwitch:
		return MCT_Float1;
	case EToonSkinShadingModelOutput::offset:
		return MCT_Float1;
	case EToonSkinShadingModelOutput::ShadowColor:
		return MCT_Float3;
	case EToonSkinShadingModelOutput::SpecularOffset:
		return MCT_Float1;
	case EToonSkinShadingModelOutput::SpecularRange:
		return MCT_Float1;
	}
	return MCT_Float1;
}

FExpressionInput* UToonSkinShadingModelOutput::GetInput(int32 InputIndex)
{
	switch (EToonSkinShadingModelOutput(InputIndex))
	{
	case EToonSkinShadingModelOutput::SSSSwitch:
		return &SSSSwitch;
	case EToonSkinShadingModelOutput::offset:
		return &offset;
	case EToonSkinShadingModelOutput::ShadowColor:
		return &ShadowColor;
	case EToonSkinShadingModelOutput::SpecularOffset:
		return &SpecularOffset;
	case EToonSkinShadingModelOutput::SpecularRange:
		return &SpecularRange;
	}
	return nullptr;
}
#endif

#undef LOCTEXT_NAMESPACE