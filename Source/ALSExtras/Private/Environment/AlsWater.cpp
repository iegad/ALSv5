#include "Environment/AlsWater.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsWater)

#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"
#include "Utility/AlsMacros.h"

AAlsWater::AAlsWater() : Super() {
	RootScene = CreateDefaultSubobject<USceneComponent>(FName{ TEXTVIEW("RootScene") });
	SetRootComponent(RootScene);

	Surface = CreateDefaultSubobject<UStaticMeshComponent>(FName{ TEXTVIEW("Surface") });
	Surface->SetupAttachment(RootScene);
	Surface->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Surface->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	FCollisionResponseContainer SurfaceResponse;
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_LedgeClimbable, ECollisionResponse::ECR_Block);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_Bubbles, ECollisionResponse::ECR_Ignore);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_Rope, ECollisionResponse::ECR_Block);
	SurfaceResponse.SetResponse(ECollisionChannel::ECC_Beam, ECollisionResponse::ECR_Block);
	Surface->SetCollisionResponseToChannels(SurfaceResponse);

	Box = CreateDefaultSubobject<UBoxComponent>(FName{ TEXTVIEW("Box") });
	Box->SetupAttachment(RootScene);
	Box->SetBoxExtent({ 50, 50, 50 });
	Box->SetRelativeLocation_Direct({ 0, 0, -Box->GetUnscaledBoxExtent().Z });
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	FCollisionResponseContainer BoxResponse;
	BoxResponse.SetResponse(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	BoxResponse.SetResponse(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	BoxResponse.SetResponse(ECollisionChannel::ECC_LedgeClimbable, ECollisionResponse::ECR_Overlap);
	BoxResponse.SetResponse(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	BoxResponse.SetResponse(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	BoxResponse.SetResponse(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	BoxResponse.SetResponse(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	BoxResponse.SetResponse(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	BoxResponse.SetResponse(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);
	BoxResponse.SetResponse(ECollisionChannel::ECC_Bubbles, ECollisionResponse::ECR_Ignore);
	BoxResponse.SetResponse(ECollisionChannel::ECC_Rope, ECollisionResponse::ECR_Block);
	BoxResponse.SetResponse(ECollisionChannel::ECC_Beam, ECollisionResponse::ECR_Block);
	Box->SetCollisionResponseToChannels(BoxResponse);

	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(FName{ TEXTVIEW("PostProcess") });
	PostProcess->SetupAttachment(Box);
	PostProcess->bUnbound = false;
	PostProcess->BlendRadius = 1.f;

	FPostProcessSettings PostProcessSettings;
	PostProcessSettings.WhiteTemp = 4875.001465;
	PostProcessSettings.WhiteTint = 0.64;
	PostProcessSettings.FilmSlope = 0.533333;
	PostProcessSettings.ColorSaturation = FVector4{ 1.437286 ,1.607725,2.0, 1.0 };
	PostProcessSettings.ColorContrast = FVector4{ 1.06 ,0.975779, 0.599666,0.63 };
	PostProcessSettings.ColorGamma = FVector4{ 1.0, 1.0, 1.0, 1.0 };
	PostProcessSettings.ColorGain = FVector4{ 0.202192, 0.402654, 1.0, 1.0 };
	PostProcess->Settings = PostProcessSettings;
}

void 
AAlsWater::BeginPlay() {
	Super::BeginPlay();

	if (IsValid(WaterPPMaterial)) {
		FPostProcessSettings PostProcessSettings;
		PostProcessSettings.WeightedBlendables.Array.Add({ 1.f, WaterPPMaterial });
		PostProcess->Settings = PostProcessSettings;
	}
}
