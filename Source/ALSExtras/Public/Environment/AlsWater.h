#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AlsWater.generated.h"

UCLASS()
class ALSEXTRAS_API AAlsWater : public AActor {
	GENERATED_BODY()
	
public:	
	AAlsWater();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Als|Components")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Als|Components")
	TObjectPtr<class UStaticMeshComponent> Surface;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Als|Components")
	TObjectPtr<class UBoxComponent> Box;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Als|Components")
	TObjectPtr<class UPostProcessComponent> PostProcess;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Als|Components")
	TObjectPtr<class UMaterialInstance> WaterPPMaterial;

	virtual void BeginPlay() override;
};
