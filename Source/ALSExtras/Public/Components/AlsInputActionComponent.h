#pragma once

#include "AlsCharacter.h"
#include "Components/ActorComponent.h"
#include "AlsInputActionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALSEXTRAS_API UAlsInputActionComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	UAlsInputActionComponent();

	UFUNCTION(BlueprintCallable)
	void Input_OnLookMouse(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable)
	void Input_OnLook(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable)
	void Input_OnMove(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable)
	void Input_OnSprint(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable)
	void Input_OnWalk();

	UFUNCTION(BlueprintCallable)
	void Input_OnCrouch();

	UFUNCTION(BlueprintCallable)
	void Input_OnJump(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable)
	void Input_OnAim(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable)
	void Input_OnRagdoll();

	UFUNCTION(BlueprintCallable)
	void Input_OnRoll();

	UFUNCTION(BlueprintCallable)
	void Input_OnRotationMode();

	UFUNCTION(BlueprintCallable)
	void Input_OnViewMode();

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<AAlsCharacter> Character;
};
