// Copyright Epic Games, Inc. All Rights Reserved.

#include "ToonShadingModelOutput.h"

#if WITH_EDITOR
#include "MaterialCompiler.h"
#endif

#define LOCTEXT_NAMESPACE "ToonShadingModelOutput"

UToonShadingModelOutput::UToonShadingModelOutput(const FObjectInitializer& ObjectInitializer)
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
int32 UToonShadingModelOutput::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	auto CompileExpressionInput = [&](FExpressionInput& ExpressionInput, float DefaultValue)
	{
		if (ExpressionInput.GetTracedInput().Expression)
		{
			return Compiler->CustomOutput(this, OutputIndex, ExpressionInput.Compile(Compiler));
		}
		return Compiler->CustomOutput(this, OutputIndex, Compiler->Constant(DefaultValue));
	};
	
	switch (EToonShadingModelOutput(OutputIndex))
	{
	case EToonShadingModelOutput::ShadowLightener:
		return CompileExpressionInput(ShadowLightener, 0.f);
	case EToonShadingModelOutput::Offset:
		return CompileExpressionInput(Offset, 0.f);
	case EToonShadingModelOutput::SpecularOffset:
		return CompileExpressionInput(SpecularOffset, 0.f);
	case EToonShadingModelOutput::SpecularRange:
		return CompileExpressionInput(SpecularRange, 0.f);
	}
	return CompilerError(Compiler, TEXT("Input missing"));
}

void UToonShadingModelOutput::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("ToonShading")));
}

uint32 UToonShadingModelOutput::GetInputType(int32 InputIndex)
{
	switch (EToonShadingModelOutput(InputIndex))
	{
	case EToonShadingModelOutput::ShadowLightener:
		return MCT_Float1;
	case EToonShadingModelOutput::Offset:
		return MCT_Float1;
	case EToonShadingModelOutput::SpecularOffset:
		return MCT_Float1;
	case EToonShadingModelOutput::SpecularRange:
		return MCT_Float1;
	}
	return MCT_Float1;
}

FExpressionInput* UToonShadingModelOutput::GetInput(int32 InputIndex)
{
	switch (EToonShadingModelOutput(InputIndex))
	{
	case EToonShadingModelOutput::ShadowLightener:
		return &ShadowLightener;
	case EToonShadingModelOutput::Offset:
		return &Offset;
	case EToonShadingModelOutput::SpecularOffset:
		return &SpecularOffset;
	case EToonShadingModelOutput::SpecularRange:
		return &SpecularRange;
	}
	return nullptr;
}
#endif

#undef LOCTEXT_NAMESPACE