﻿#pragma once

#include "SkeletalMeshComponent_Outline.h"
#include "SkeletalMeshSceneProxy.h"

class FMultiDrawSceneProxy : public FSkeletalMeshSceneProxy
{
public:
	FMultiDrawSceneProxy(USkeletalMeshComponent_Outline* Component, FSkeletalMeshRenderData* InSkelMeshRenderData)
		: FSkeletalMeshSceneProxy(Component, InSkelMeshRenderData)
	, MultiDrawMeshComponent(Component)
	{
	}
	
	USkeletalMeshComponent_Outline* MultiDrawMeshComponent;
	
	virtual void DrawStaticElements(FStaticPrimitiveDrawInterface* PDI) override;

	// TODO : cant access, copy to there too
	void CreateBaseMeshBatch(const FSceneView* View, const FSkeletalMeshLODRenderData& LODData, const int32 LODIndex, const int32 SectionIndex, const FSectionElementInfo& SectionElementInfo, FMeshBatch& Mesh, ESkinVertexFactoryMode VFMode = ESkinVertexFactoryMode::Default) const;
	
	// TODO : Cant virtual, we should copy by the version, This is the main function to do the overlay material logic.
	void GetDynamicElementsSection(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap,
									const FSkeletalMeshLODRenderData& LODData, const int32 LODIndex, const int32 SectionIndex, bool bSectionSelected,
								   const FSectionElementInfo& SectionElementInfo, bool bInSelectable, FMeshElementCollector& Collector) const;

	// TODO : Contain GetDynamicElementsSection function logic.
	void GetMeshElementsConditionallySelectable(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, bool bInSelectable, uint32 VisibilityMap, FMeshElementCollector& Collector) const;

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;
};
