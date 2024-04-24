#pragma once

#include "AlsCharacter.h"
#include "AlsCharacterExample.generated.h"

struct FInputActionValue;
class UAlsCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS(AutoExpandCategories = ("Settings|Als Character Example", "State|Als Character Example"))
class ALSEXTRAS_API AAlsCharacterExample : public AAlsCharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Als Character Example")
	TObjectPtr<UAlsCameraComponent> Camera;

public:
	AAlsCharacterExample();

	virtual void NotifyControllerChanged() override;

protected:
	virtual void Tick(float InDeltaTime) override;

	// Camera
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

	// Input
	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

private:
	void Input_OnLookMouse(const FInputActionValue& ActionValue);

	void Input_OnLook(const FInputActionValue& ActionValue);

	void Input_OnMove(const FInputActionValue& ActionValue);

	void Input_OnSprint(const FInputActionValue& ActionValue);

	void Input_OnWalk();

	void Input_OnCrouch();

	void Input_OnJump(const FInputActionValue& ActionValue);

	void Input_OnAim(const FInputActionValue& ActionValue);

	void Input_OnRagdoll();

	void Input_OnRoll();

	void Input_OnRotationMode();

	void Input_OnViewMode();

	void Input_OnSwitchShoulder();

	// Debug

public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;

private:
	bool SwimmingUp = false;
};
