// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonAnisoShadingModelOutput.h"

#if WITH_EDITOR
#include "MaterialCompiler.h"
#endif

#define LOCTEXT_NAMESPACE "ToonAnisoShadingModelOutput"

UToonAnisoShadingModelOutput::UToonAnisoShadingModelOutput(const FObjectInitializer& ObjectInitializer)
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
int32 UToonAnisoShadingModelOutput::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	auto CompileExpressionInput = [&](FExpressionInput& ExpressionInput, float DefaultValue)
	{
		if (ExpressionInput.GetTracedInput().Expression)
		{
			return Compiler->CustomOutput(this, OutputIndex, ExpressionInput.Compile(Compiler));
		}
		return Compiler->CustomOutput(this, OutputIndex, Compiler->Constant(DefaultValue));
	};
	
	switch (EToonAnisoShadingModelOutput(OutputIndex))
	{
	case EToonAnisoShadingModelOutput::Anisotropy:
		return CompileExpressionInput(Anisotropy, 0.f);
	case EToonAnisoShadingModelOutput::AnisotropicRoughness:
		return CompileExpressionInput(AnisotropicRoughness, 0.f);
	case EToonAnisoShadingModelOutput::ShadowColor:
		return CompileExpressionInput(ShadowColor, 0.f);
	case EToonAnisoShadingModelOutput::TangentRotation:
		return CompileExpressionInput(TangentRotation, 0.f);
	}
	return CompilerError(Compiler, TEXT("Input missing"));
}

void UToonAnisoShadingModelOutput::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("ToonAnisoShading")));
}

uint32 UToonAnisoShadingModelOutput::GetInputType(int32 InputIndex)
{
	switch (EToonAnisoShadingModelOutput(InputIndex))
	{
	case EToonAnisoShadingModelOutput::Anisotropy:
		return MCT_Float1;
	case EToonAnisoShadingModelOutput::AnisotropicRoughness:
		return MCT_Float1;
	case EToonAnisoShadingModelOutput::ShadowColor:
		return MCT_Float3;
	case EToonAnisoShadingModelOutput::TangentRotation:
		return MCT_Float1;
	}
	return MCT_Float1;
}

FExpressionInput* UToonAnisoShadingModelOutput::GetInput(int32 InputIndex)
{
	switch (EToonAnisoShadingModelOutput(InputIndex))
	{
	case EToonAnisoShadingModelOutput::Anisotropy:
		return &Anisotropy;
	case EToonAnisoShadingModelOutput::AnisotropicRoughness:
		return &AnisotropicRoughness;
	case EToonAnisoShadingModelOutput::ShadowColor:
		return &ShadowColor;
	case EToonAnisoShadingModelOutput::TangentRotation:
		return &TangentRotation;
	}
	return nullptr;
}
#endif

#undef LOCTEXT_NAMESPACE