// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonHairShadingModelOutput.h"

#if WITH_EDITOR
#include "MaterialCompiler.h"
#endif

#define LOCTEXT_NAMESPACE "ToonHairShadingModelOutput"

UToonHairShadingModelOutput::UToonHairShadingModelOutput(const FObjectInitializer& ObjectInitializer)
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
int32 UToonHairShadingModelOutput::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	auto CompileExpressionInput = [&](FExpressionInput& ExpressionInput, float DefaultValue)
	{
		if (ExpressionInput.GetTracedInput().Expression)
		{
			return Compiler->CustomOutput(this, OutputIndex, ExpressionInput.Compile(Compiler));
		}
		return Compiler->CustomOutput(this, OutputIndex, Compiler->Constant(DefaultValue));
	};
	
	switch (EToonHairShadingModelOutput(OutputIndex))
	{
	case EToonHairShadingModelOutput::TightenSpecular:
		return CompileExpressionInput(TightenSpecular, 0.f);
	case EToonHairShadingModelOutput::Scatter:
		return CompileExpressionInput(Scatter, 0.f);
	case EToonHairShadingModelOutput::SpecularOffset:
		return CompileExpressionInput(SpecularOffset, 0.f);
	}
	checkNoEntry();
	return CompilerError(Compiler, TEXT("Input missing"));
}

void UToonHairShadingModelOutput::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("ToonHairShading")));
}

uint32 UToonHairShadingModelOutput::GetInputType(int32 InputIndex)
{
	switch (EToonHairShadingModelOutput(InputIndex))
	{
	case EToonHairShadingModelOutput::TightenSpecular:
		return MCT_Float1;
	case EToonHairShadingModelOutput::Scatter:
		return MCT_Float1;
	case EToonHairShadingModelOutput::SpecularOffset:
		return MCT_Float1;
	}
	checkNoEntry();
	return MCT_Float1;
}

FExpressionInput* UToonHairShadingModelOutput::GetInput(int32 InputIndex)
{
	switch (EToonHairShadingModelOutput(InputIndex))
	{
	case EToonHairShadingModelOutput::TightenSpecular:
		return &TightenSpecular;
	case EToonHairShadingModelOutput::Scatter:
		return &Scatter;
	case EToonHairShadingModelOutput::SpecularOffset:
		return &SpecularOffset;
	}
	checkNoEntry();
	return nullptr;
}
#endif

#undef LOCTEXT_NAMESPACE