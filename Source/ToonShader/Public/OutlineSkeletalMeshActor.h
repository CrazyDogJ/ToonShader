#pragma once

#include "CoreMinimal.h"
#include "Animation/SkeletalMeshActor.h"
#include "OutlineSkeletalMeshActor.generated.h"

UCLASS()
class TOONSHADER_API AOutlineSkeletalMeshActor : public ASkeletalMeshActor
{
	GENERATED_BODY()

public:
	AOutlineSkeletalMeshActor(const FObjectInitializer& ObjectInitializer);
};
