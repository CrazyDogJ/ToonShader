// Fill out your copyright notice in the Description page of Project Settings.


#include "AnisotropicShadingModelOutput.h"

#if WITH_EDITOR
#include "MaterialCompiler.h"
#endif

#define LOCTEXT_NAMESPACE "AnisotropicShadingModelOutput"

UAnisotropicShadingModelOutput::UAnisotropicShadingModelOutput(const FObjectInitializer& ObjectInitializer)
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
int32 UAnisotropicShadingModelOutput::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	auto CompileExpressionInput = [&](FExpressionInput& ExpressionInput, float DefaultValue)
	{
		if (ExpressionInput.GetTracedInput().Expression)
		{
			return Compiler->CustomOutput(this, OutputIndex, ExpressionInput.Compile(Compiler));
		}
		return Compiler->CustomOutput(this, OutputIndex, Compiler->Constant(DefaultValue));
	};
	
	switch (EAnisotropicShadingModelOutput(OutputIndex))
	{
	case EAnisotropicShadingModelOutput::Anisotropy:
		return CompileExpressionInput(Anisotropy, 0.f);
	case EAnisotropicShadingModelOutput::ShadowColor:
		return CompileExpressionInput(ShadowColor, 0.f);
	case EAnisotropicShadingModelOutput::TangentRotation:
		return CompileExpressionInput(TangentRotation, 0.f);
	}
	return CompilerError(Compiler, TEXT("Input missing"));
}

void UAnisotropicShadingModelOutput::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("AnisotropicShading")));
}

uint32 UAnisotropicShadingModelOutput::GetInputType(int32 InputIndex)
{
	switch (EAnisotropicShadingModelOutput(InputIndex))
	{
	case EAnisotropicShadingModelOutput::Anisotropy:
		return MCT_Float1;
	case EAnisotropicShadingModelOutput::ShadowColor:
		return MCT_Float3;
	case EAnisotropicShadingModelOutput::TangentRotation:
		return MCT_Float1;
	}
	return MCT_Float1;
}

FExpressionInput* UAnisotropicShadingModelOutput::GetInput(int32 InputIndex)
{
	switch (EAnisotropicShadingModelOutput(InputIndex))
	{
	case EAnisotropicShadingModelOutput::Anisotropy:
		return &Anisotropy;
	case EAnisotropicShadingModelOutput::ShadowColor:
		return &ShadowColor;
	case EAnisotropicShadingModelOutput::TangentRotation:
		return &TangentRotation;
	}
	return nullptr;
}
#endif

#undef LOCTEXT_NAMESPACE