#include "MultiDrawSceneProxy.h"

#include "SkeletalMeshSceneProxy.h"
#include "SkeletalRenderPublic.h"
#include "EngineStats.h"
#include "Stats/Stats2.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Materials/MaterialRenderProxy.h"
#include "Rendering/SkeletalMeshRenderData.h"

// TODO : Link error fixing
bool FSkeletalMeshObject::IsMaterialHidden(int32 InLODIndex,int32 MaterialIdx) const
{
	check(LODInfo.IsValidIndex(InLODIndex));
	return LODInfo[InLODIndex].HiddenMaterials.IsValidIndex(MaterialIdx) && LODInfo[InLODIndex].HiddenMaterials[MaterialIdx];
}

float FSkeletalMeshObject::GetScreenSize(int32 LODIndex) const
{
	if (LODInfo.IsValidIndex(LODIndex))
	{
		return LODInfo[LODIndex].ScreenSize.GetValue();
	}
	return 0.f;
}
// TODO : link fixing end

// TODO : Must copy this class to there, so that we can use it
class FSkeletalMeshSectionIter
{
public:
	FSkeletalMeshSectionIter(const int32 InLODIdx, const FSkeletalMeshObject& InMeshObject, const FSkeletalMeshLODRenderData& InLODData, const FSkeletalMeshSceneProxy::FLODSectionElements& InLODSectionElements, bool bIgnorePreviewFilter = false)
		: SectionIndex(0)
		, MeshObject(InMeshObject)
		, LODSectionElements(InLODSectionElements)
		, Sections(InLODData.RenderSections)
#if WITH_EDITORONLY_DATA
		, SectionIndexPreview(bIgnorePreviewFilter ? INDEX_NONE : InMeshObject.SectionIndexPreview)
		, MaterialIndexPreview(bIgnorePreviewFilter ? INDEX_NONE : InMeshObject.MaterialIndexPreview)
#endif
	{
		while (NotValidPreviewSection())
		{
			SectionIndex++;
		}
	}
	FORCEINLINE FSkeletalMeshSectionIter& operator++()
	{
		do 
		{
		SectionIndex++;
		} while (NotValidPreviewSection());
		return *this;
	}
	FORCEINLINE explicit operator bool() const
	{
		return ((SectionIndex < Sections.Num()) && LODSectionElements.SectionElements.IsValidIndex(GetSectionElementIndex()));
	}
	FORCEINLINE const FSkelMeshRenderSection& GetSection() const
	{
		return Sections[SectionIndex];
	}
	FORCEINLINE const int32 GetSectionElementIndex() const
	{
		return SectionIndex;
	}
	FORCEINLINE const FSkeletalMeshSceneProxy::FSectionElementInfo& GetSectionElementInfo() const
	{
		int32 SectionElementInfoIndex = GetSectionElementIndex();
		return LODSectionElements.SectionElements[SectionElementInfoIndex];
	}
	FORCEINLINE bool NotValidPreviewSection()
	{
#if WITH_EDITORONLY_DATA
		if (MaterialIndexPreview == INDEX_NONE)
		{
			int32 ActualPreviewSectionIdx = SectionIndexPreview;

			return	(SectionIndex < Sections.Num()) &&
				((ActualPreviewSectionIdx >= 0) && (ActualPreviewSectionIdx != SectionIndex));
		}
		else
		{
			int32 ActualPreviewMaterialIdx = MaterialIndexPreview;
			int32 ActualPreviewSectionIdx = INDEX_NONE;
			if (ActualPreviewMaterialIdx != INDEX_NONE && Sections.IsValidIndex(SectionIndex))
			{
				const FSkeletalMeshSceneProxy::FSectionElementInfo& SectionInfo = LODSectionElements.SectionElements[SectionIndex];
				if (SectionInfo.UseMaterialIndex == ActualPreviewMaterialIdx)
				{
					ActualPreviewSectionIdx = SectionIndex;
				}
			}

			return	(SectionIndex < Sections.Num()) &&
				((ActualPreviewMaterialIdx >= 0) && (ActualPreviewSectionIdx != SectionIndex));
		}
#else
		return false;
#endif
	}
private:
	int32 SectionIndex;
	const FSkeletalMeshObject& MeshObject;
	const FSkeletalMeshSceneProxy::FLODSectionElements& LODSectionElements;
	const TArray<FSkelMeshRenderSection>& Sections;
#if WITH_EDITORONLY_DATA
	const int32 SectionIndexPreview;
	const int32 MaterialIndexPreview;
#endif
};

// TODO : Copy this too
void FMultiDrawSceneProxy::CreateBaseMeshBatch(const FSceneView* View, const FSkeletalMeshLODRenderData& LODData, const int32 LODIndex, const int32 SectionIndex, const FSectionElementInfo& SectionElementInfo, FMeshBatch& Mesh, ESkinVertexFactoryMode VFMode) const
{
	Mesh.VertexFactory = MeshObject->GetSkinVertexFactory(View, LODIndex, SectionIndex, VFMode);
	Mesh.MaterialRenderProxy = SectionElementInfo.Material->GetRenderProxy();
#if RHI_RAYTRACING
	Mesh.SegmentIndex = SectionIndex;
	Mesh.CastRayTracedShadow = SectionElementInfo.bEnableShadowCasting && bCastDynamicShadow;
#endif

	FMeshBatchElement& BatchElement = Mesh.Elements[0];
	BatchElement.FirstIndex = LODData.RenderSections[SectionIndex].BaseIndex;
	BatchElement.IndexBuffer = LODData.MultiSizeIndexContainer.GetIndexBuffer();
	BatchElement.MinVertexIndex = LODData.RenderSections[SectionIndex].GetVertexBufferIndex();
	BatchElement.MaxVertexIndex = LODData.RenderSections[SectionIndex].GetVertexBufferIndex() + LODData.RenderSections[SectionIndex].GetNumVertices() - 1;

	BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();
	BatchElement.NumPrimitives = LODData.RenderSections[SectionIndex].NumTriangles;
}

void FMultiDrawSceneProxy::DrawStaticElements(FStaticPrimitiveDrawInterface* PDI)
{
	if (!MeshObject)
	{
		return;
	}

	if (!HasViewDependentDPG())
	{
		ESceneDepthPriorityGroup PrimitiveDPG = GetStaticDepthPriorityGroup();
		bool bUseSelectedMaterial = false;

		int32 NumLODs = SkeletalMeshRenderData->LODRenderData.Num();
		int32 ClampedMinLOD = 0; // TODO: MinLOD, Bias?

		for (int32 LODIndex = ClampedMinLOD; LODIndex < NumLODs; ++LODIndex)
		{
			const FSkeletalMeshLODRenderData& LODData = SkeletalMeshRenderData->LODRenderData[LODIndex];

			if (LODSections.Num() > 0 && LODData.GetNumVertices() > 0)
			{
				float ScreenSize = MeshObject->GetScreenSize(LODIndex);
				const FLODSectionElements& LODSection = LODSections[LODIndex];
				check(LODSection.SectionElements.Num() == LODData.RenderSections.Num());

				for (FSkeletalMeshSectionIter Iter(LODIndex, *MeshObject, LODData, LODSection); Iter; ++Iter)
				{
					const FSkelMeshRenderSection& Section = Iter.GetSection();
					const int32 SectionIndex = Iter.GetSectionElementIndex();
					const FVertexFactory* VertexFactory = MeshObject->GetStaticSkinVertexFactory(LODIndex, SectionIndex, ESkinVertexFactoryMode::Default);

					if (!VertexFactory)
					{
						// hide this part
						continue;
					}

					const FSectionElementInfo& SectionElementInfo = Iter.GetSectionElementInfo();

					// If hidden skip the draw
					if (MeshObject->IsMaterialHidden(LODIndex, SectionElementInfo.UseMaterialIndex) || Section.bDisabled)
					{
						continue;
					}

				#if WITH_EDITOR
					if (GIsEditor)
					{
						bUseSelectedMaterial = (MeshObject->SelectedEditorSection == SectionIndex);
						PDI->SetHitProxy(SectionElementInfo.HitProxy);
					}
				#endif // WITH_EDITOR

					FMeshBatch MeshElement;
					FMeshBatchElement& BatchElement = MeshElement.Elements[0];
					MeshElement.DepthPriorityGroup = PrimitiveDPG;
					MeshElement.VertexFactory = VertexFactory;
					MeshElement.MaterialRenderProxy = SectionElementInfo.Material->GetRenderProxy();
					MeshElement.ReverseCulling = IsLocalToWorldDeterminantNegative();
					MeshElement.CastShadow = SectionElementInfo.bEnableShadowCasting;
				#if RHI_RAYTRACING
					MeshElement.CastRayTracedShadow = MeshElement.CastShadow && bCastDynamicShadow;
				#endif
					MeshElement.Type = PT_TriangleList;
					MeshElement.LODIndex = LODIndex;
					MeshElement.SegmentIndex = SectionIndex;
					MeshElement.MeshIdInPrimitive = SectionIndex;

					BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();
					BatchElement.FirstIndex = Section.BaseIndex;
					BatchElement.MinVertexIndex = Section.BaseVertexIndex;
					BatchElement.MaxVertexIndex = LODData.GetNumVertices() - 1;
					BatchElement.NumPrimitives = Section.NumTriangles;
					BatchElement.IndexBuffer = LODData.MultiSizeIndexContainer.GetIndexBuffer();

					PDI->DrawMesh(MeshElement, ScreenSize);

					//Draw multi first
					if (MultiDrawMeshComponent)
					{
						if (MultiDrawMeshComponent->DrawMaterial.IsValidIndex(SectionIndex))
						{
							if (MultiDrawMeshComponent->DrawMaterial[SectionIndex] != nullptr)
							{
								FMeshBatch OverlayMeshBatch(MeshElement);
								OverlayMeshBatch.bOverlayMaterial = true;
								OverlayMeshBatch.CastShadow = false;
								OverlayMeshBatch.bSelectable = false;
								OverlayMeshBatch.MaterialRenderProxy = MultiDrawMeshComponent->DrawMaterial[SectionIndex]->GetRenderProxy();
								// make sure overlay is always rendered on top of base mesh
								OverlayMeshBatch.MeshIdInPrimitive += LODData.RenderSections.Num();
								// Reuse mesh ScreenSize as cull distance for an overlay. Overlay does not need to compute LOD so we can avoid adding new members into MeshBatch or MeshRelevance
								float OverlayMeshScreenSize = OverlayMaterialMaxDrawDistance;
								PDI->DrawMesh(OverlayMeshBatch, OverlayMeshScreenSize);
							}
						}
					}
					//Draw overlay
				}
			}
		}
	}
}

void FMultiDrawSceneProxy::GetDynamicElementsSection(const TArray<const FSceneView*>& Views,
	const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, const FSkeletalMeshLODRenderData& LODData,
	const int32 LODIndex, const int32 SectionIndex, bool bSectionSelected,
	const FSectionElementInfo& SectionElementInfo, bool bInSelectable, FMeshElementCollector& Collector) const
{
	const FSkelMeshRenderSection& Section = LODData.RenderSections[SectionIndex];

#if !WITH_EDITOR
	const bool bIsSelected = false;
#else // #if !WITH_EDITOR
	bool bIsSelected = IsSelected();

	// if the mesh isn't selected but the mesh section is selected in the AnimSetViewer, find the mesh component and make sure that it can be highlighted (ie. are we rendering for the AnimSetViewer or not?)
	if( !bIsSelected && bSectionSelected && bCanHighlightSelectedSections )
	{
		bIsSelected = true;
	}
	if (WantsEditorEffects())
	{
		bIsSelected = true;
	}
#endif // #if WITH_EDITOR

	const bool bIsWireframe = ViewFamily.EngineShowFlags.Wireframe;

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			const FSceneView* View = Views[ViewIndex];

			FMeshBatch& Mesh = Collector.AllocateMesh();

			CreateBaseMeshBatch(View, LODData, LODIndex, SectionIndex, SectionElementInfo, Mesh);
			//For dynamic mesh elements, Mesh.MeshIdInPrimitive is setup in Collector.AddMesh.
			
			if(!Mesh.VertexFactory)
			{
				// hide this part
				continue;
			}

			Mesh.bWireframe |= bForceWireframe;
			Mesh.Type = PT_TriangleList;
			Mesh.bSelectable = bInSelectable;

			FMeshBatchElement& BatchElement = Mesh.Elements[0];

		#if WITH_EDITOR
			Mesh.BatchHitProxyId = SectionElementInfo.HitProxy ? SectionElementInfo.HitProxy->Id : FHitProxyId();

			if (bSectionSelected && bCanHighlightSelectedSections)
			{
				Mesh.bUseSelectionOutline = true;
			}
			else
			{
				Mesh.bUseSelectionOutline = !bCanHighlightSelectedSections && bIsSelected;
			}
		#endif

#if WITH_EDITORONLY_DATA
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			if (bIsSelected && ViewFamily.EngineShowFlags.VertexColors && AllowDebugViewmodes())
			{
				// Note: static mesh renderer does something more complicated involving per-section selection, but whole component selection seems ok for now.
				if (FMaterialRenderProxy* VertexColorVisualizationMaterialInstance = MeshPaintVisualize::GetMaterialRenderProxy(bIsSelected, IsHovered()))
				{
					Collector.RegisterOneFrameMaterialProxy(VertexColorVisualizationMaterialInstance);
					Mesh.MaterialRenderProxy = VertexColorVisualizationMaterialInstance;
				}
			}
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#endif // WITH_EDITORONLY_DATA

			BatchElement.MinVertexIndex = Section.BaseVertexIndex;
			Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
			Mesh.CastShadow = SectionElementInfo.bEnableShadowCasting;
			Mesh.bCanApplyViewModeOverrides = true;
			Mesh.bUseWireframeSelectionColoring = bIsSelected;

		#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			// TODO :: Temp remove this function
			//BatchElement.SkinCacheDebugColor = MeshObject->GetSkinCacheVisualizationDebugColor(View->CurrentGPUSkinCacheVisualizationMode, SectionIndex);
			BatchElement.VisualizeElementIndex = SectionIndex;
			Mesh.VisualizeLODIndex = LODIndex;
		#endif

			if (ensureMsgf(Mesh.MaterialRenderProxy, TEXT("GetDynamicElementsSection with invalid MaterialRenderProxy. Owner:%s LODIndex:%d UseMaterialIndex:%d"), *GetOwnerName().ToString(), LODIndex, SectionElementInfo.UseMaterialIndex))
			{
				Collector.AddMesh(ViewIndex, Mesh);
			}

			const int32 NumVertices = Section.GetNumVertices();
			// TODO : module problem ,we dont stat yet
			//INC_DWORD_STAT_BY(STAT_GPUSkinVertices,(uint32)(bIsCPUSkinned ? 0 : NumVertices)); // TODO: Nanite-Skinning
			//INC_DWORD_STAT_BY(STAT_SkelMeshTriangles,Mesh.GetNumPrimitives());
			//INC_DWORD_STAT(STAT_SkelMeshDrawCalls);

			// TODO : Multi draw mesh logic
			if (MultiDrawMeshComponent)
			{
				if (MultiDrawMeshComponent->DrawMaterial.IsValidIndex(SectionIndex))
				{
					if (MultiDrawMeshComponent->DrawMaterial[SectionIndex] != nullptr)
					{
						FMeshBatch& OutlineMeshBatch = Collector.AllocateMesh();
						OutlineMeshBatch = Mesh;
						OutlineMeshBatch.bOverlayMaterial = true;
						OutlineMeshBatch.CastShadow = false;
						OutlineMeshBatch.bSelectable = false;
						OutlineMeshBatch.MaterialRenderProxy = MultiDrawMeshComponent->DrawMaterial[SectionIndex]->GetRenderProxy();
						// make sure overlay is always rendered on top of base mesh
						OutlineMeshBatch.MeshIdInPrimitive += LODData.RenderSections.Num();
						Collector.AddMesh(ViewIndex, OutlineMeshBatch);
					}
				}
			}
			
			// negative cull distance disables overlay rendering
			if (OverlayMaterial != nullptr && OverlayMaterialMaxDrawDistance >= 0.f)
			{
				const bool bHasOverlayCullDistance = 
					OverlayMaterialMaxDrawDistance > 0.f && 
					OverlayMaterialMaxDrawDistance != FLT_MAX && 
					!ViewFamily.EngineShowFlags.DistanceCulledPrimitives;
				
				bool bAddOverlay = true;
				if (bHasOverlayCullDistance)
				{
					// this is already combined with ViewDistanceScale
					float MaxDrawDistanceScale = GetCachedScalabilityCVars().SkeletalMeshOverlayDistanceScale;
					MaxDrawDistanceScale *= GetCachedScalabilityCVars().CalculateFieldOfViewDistanceScale(View->DesiredFOV);
					float DistanceSquared = (GetBounds().Origin - View->ViewMatrices.GetViewOrigin()).SizeSquared();
					if (DistanceSquared > FMath::Square(OverlayMaterialMaxDrawDistance * MaxDrawDistanceScale))
					{
						// distance culled
						bAddOverlay = false;
					}
				}
				
				if (bAddOverlay)
				{
					FMeshBatch& OverlayMeshBatch = Collector.AllocateMesh();
					OverlayMeshBatch = Mesh;
					OverlayMeshBatch.bOverlayMaterial = true;
					OverlayMeshBatch.CastShadow = false;
					OverlayMeshBatch.bSelectable = false;
					OverlayMeshBatch.MaterialRenderProxy = OverlayMaterial->GetRenderProxy();
					// make sure overlay is always rendered on top of base mesh
					OverlayMeshBatch.MeshIdInPrimitive += LODData.RenderSections.Num();
					Collector.AddMesh(ViewIndex, OverlayMeshBatch);

					// TODO : module problem ,we dont stat yet
					//INC_DWORD_STAT_BY(STAT_SkelMeshTriangles, OverlayMeshBatch.GetNumPrimitives());
					//INC_DWORD_STAT(STAT_SkelMeshDrawCalls);
				}
			}
		}
	}
}

void FMultiDrawSceneProxy::GetMeshElementsConditionallySelectable(const TArray<const FSceneView*>& Views,
	const FSceneViewFamily& ViewFamily, bool bInSelectable, uint32 VisibilityMap,
	FMeshElementCollector& Collector) const
{
	if (!MeshObject)
	{
		return;
	}

	TRACE_CPUPROFILER_EVENT_SCOPE(SkeletalMesh);

	const FEngineShowFlags& EngineShowFlags = ViewFamily.EngineShowFlags;

	const int32 FirstLODIdx = SkeletalMeshRenderData->GetFirstValidLODIdx(FMath::Max(SkeletalMeshRenderData->PendingFirstLODIdx, SkeletalMeshRenderData->CurrentFirstLODIdx));
	if (FirstLODIdx == INDEX_NONE)
	{
#if DO_CHECK
		UE_LOG(LogSkeletalMesh, Warning, TEXT("Skeletal mesh %s has no valid LODs for rendering."), *GetResourceName().ToString());
#endif
	}
	else
	{
		const int32 LODIndex = MeshObject->GetLOD();
		check(LODIndex < SkeletalMeshRenderData->LODRenderData.Num());
		const FSkeletalMeshLODRenderData& LODData = SkeletalMeshRenderData->LODRenderData[LODIndex];

		if (LODSections.Num() > 0 && LODIndex >= FirstLODIdx)
		{
			check(SkeletalMeshRenderData->LODRenderData[LODIndex].GetNumVertices() > 0);

			const FLODSectionElements& LODSection = LODSections[LODIndex];

			check(LODSection.SectionElements.Num() == LODData.RenderSections.Num());

			for (FSkeletalMeshSectionIter Iter(LODIndex, *MeshObject, LODData, LODSection); Iter; ++Iter)
			{
				const FSkelMeshRenderSection& Section = Iter.GetSection();
				const int32 SectionIndex = Iter.GetSectionElementIndex();
				const FSectionElementInfo& SectionElementInfo = Iter.GetSectionElementInfo();

				bool bSectionSelected = false;

#if WITH_EDITORONLY_DATA
				// TODO: This is not threadsafe! A render command should be used to propagate SelectedEditorSection to the scene proxy.
				if (MeshObject->SelectedEditorMaterial != INDEX_NONE)
				{
					bSectionSelected = (MeshObject->SelectedEditorMaterial == SectionElementInfo.UseMaterialIndex);
				}
				else
				{
					bSectionSelected = (MeshObject->SelectedEditorSection == SectionIndex);
				}
			
#endif
				// If hidden skip the draw
				if (MeshObject->IsMaterialHidden(LODIndex, SectionElementInfo.UseMaterialIndex) || Section.bDisabled)
				{
					continue;
				}

				GetDynamicElementsSection(Views, ViewFamily, VisibilityMap, LODData, LODIndex, SectionIndex, bSectionSelected, SectionElementInfo, bInSelectable, Collector);
			}
		}
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			if( PhysicsAssetForDebug )
			{
				DebugDrawPhysicsAsset(ViewIndex, Collector, ViewFamily.EngineShowFlags);
			}

			if (EngineShowFlags.MassProperties && DebugMassData.Num() > 0)
			{
				FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
				if (MeshObject->GetComponentSpaceTransforms())
				{
					const TArray<FTransform>& ComponentSpaceTransforms = *MeshObject->GetComponentSpaceTransforms();

					for (const FDebugMassData& DebugMass : DebugMassData)
					{
						if (ComponentSpaceTransforms.IsValidIndex(DebugMass.BoneIndex))
						{
							const FTransform BoneToWorld = ComponentSpaceTransforms[DebugMass.BoneIndex] * FTransform(GetLocalToWorld());
							DebugMass.DrawDebugMass(PDI, BoneToWorld);
						}
					}
				}
			}

			if (ViewFamily.EngineShowFlags.SkeletalMeshes)
			{
				RenderBounds(Collector.GetPDI(ViewIndex), ViewFamily.EngineShowFlags, GetBounds(), IsSelected());
			}

			if (ViewFamily.EngineShowFlags.Bones || bDrawDebugSkeleton)
			{
				DebugDrawSkeleton(ViewIndex, Collector, ViewFamily.EngineShowFlags);
			}

#if WITH_EDITOR
			DebugDrawPoseWatchSkeletons(ViewIndex, Collector, ViewFamily.EngineShowFlags);
#endif
		}
	}
#endif
}

void FMultiDrawSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
                                                  const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FSkeletalMeshSceneProxy_GetMeshElements);
	GetMeshElementsConditionallySelectable(Views, ViewFamily, true, VisibilityMap, Collector);
}
