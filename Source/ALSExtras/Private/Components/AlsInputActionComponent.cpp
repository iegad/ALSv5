#include "Components/AlsInputActionComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsInputActionComponent)

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Utility/AlsMath.h"

UAlsInputActionComponent::UAlsInputActionComponent() {
}

void
UAlsInputActionComponent::Input_OnLookMouse(const FInputActionValue& ActionValue) {
	const auto Value{ ActionValue.Get<FVector2D>() };

	Character->AddControllerPitchInput(Value.Y * Character->GetInputActions().LookUpMouseSensitivity);
	Character->AddControllerYawInput(Value.X * Character->GetInputActions().LookRightMouseSensitivity);
}

void
UAlsInputActionComponent::Input_OnLook(const FInputActionValue& ActionValue) {
	const auto Value{ ActionValue.Get<FVector2D>() };

	Character->AddControllerPitchInput(Value.Y * Character->GetInputActions().LookUpRate);
	Character->AddControllerYawInput(Value.X * Character->GetInputActions().LookRightRate);
}

void
UAlsInputActionComponent::Input_OnMove(const FInputActionValue& ActionValue) {
	const auto Value{ UAlsMath::ClampMagnitude012D(ActionValue.Get<FVector2D>()) };

	const auto ForwardDirection{ UAlsMath::AngleToDirectionXY(UE_REAL_TO_FLOAT(Character->GetViewState().Rotation.Yaw)) };
	const auto RightDirection{ UAlsMath::PerpendicularCounterClockwiseXY(ForwardDirection) };

	Character->AddMovementInput(ForwardDirection * Value.Y + RightDirection * Value.X);
}

void
UAlsInputActionComponent::Input_OnSprint(const FInputActionValue& ActionValue) {
	Character->SetDesiredGait(ActionValue.Get<bool>() ? AlsGaitTags::Sprinting : AlsGaitTags::Running);
}

void
UAlsInputActionComponent::Input_OnWalk() {
	if (Character->GetDesiredGait() == AlsGaitTags::Walking) {
		Character->SetDesiredGait(AlsGaitTags::Running);
	}
	else if (Character->GetDesiredGait() == AlsGaitTags::Running) {
		Character->SetDesiredGait(AlsGaitTags::Walking);
	}
}

void
UAlsInputActionComponent::Input_OnCrouch() {
	if (Character->GetDesiredStance() == AlsStanceTags::Standing) {
		Character->SetDesiredStance(AlsStanceTags::Crouching);
	}
	else if (Character->GetDesiredStance() == AlsStanceTags::Crouching) {
		Character->SetDesiredStance(AlsStanceTags::Standing);
	}
}

void
UAlsInputActionComponent::Input_OnJump(const FInputActionValue& ActionValue) {
	if (ActionValue.Get<bool>()) {
		if (Character->StopRagdolling()) {
			return;
		}

		if (Character->StartMantlingGrounded()) {
			return;
		}

		if (Character->GetStance() == AlsStanceTags::Crouching) {
			Character->SetDesiredStance(AlsStanceTags::Standing);
			return;
		}

		Character->Jump();
	}
	else {
		Character->StopJumping();
	}
}

void
UAlsInputActionComponent::Input_OnAim(const FInputActionValue& ActionValue) {
	Character->SetDesiredAiming(ActionValue.Get<bool>());
}

void
UAlsInputActionComponent::Input_OnRagdoll() {
	if (!Character->StopRagdolling()) {
		Character->StartRagdolling();
	}
}

void
UAlsInputActionComponent::Input_OnRoll() {
	static constexpr auto PlayRate{ 1.3f };

	Character->StartRolling(PlayRate);
}

void
UAlsInputActionComponent::Input_OnRotationMode() {
	Character->SetDesiredRotationMode(Character->GetDesiredRotationMode() == AlsRotationModeTags::VelocityDirection
		? AlsRotationModeTags::ViewDirection
		: AlsRotationModeTags::VelocityDirection);
}

void
UAlsInputActionComponent::Input_OnViewMode() {
	Character->SetViewMode(Character->GetViewMode() == AlsViewModeTags::ThirdPerson ? AlsViewModeTags::FirstPerson : AlsViewModeTags::ThirdPerson);
}

void 
UAlsInputActionComponent::BeginPlay() {
	Super::BeginPlay();

	Character = Cast<AAlsCharacter>(GetOwner());
	if (!IsValid(Character)) {
		XERROR("Character is invalid");
		return;
	}

	auto* NewPlayer{ Cast<APlayerController>(Character->GetController()) };
	if (!IsValid(NewPlayer)) {
		XERROR("PlayerController is invalid");
		return;
	}

	NewPlayer->InputYawScale_DEPRECATED = 1.0f;
	NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
	NewPlayer->InputRollScale_DEPRECATED = 1.0f;

	auto* InputSubsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer()) };
	if (!IsValid(InputSubsystem)) {
		XERROR("EnhancedInputLocalPlayerSubsystem is invalid");
		return;
	}

	FModifyContextOptions Options;
	Options.bNotifyUserSettings = true;
	InputSubsystem->AddMappingContext(Character->GetInputActions().InputMappingContext, 0, Options);

	auto* EnhancedInput = Character->FindComponentByClass<UEnhancedInputComponent>();
	if (!IsValid(EnhancedInput)) {
		XERROR("EnhancedInputComponent is invalid");
		return;
	}

	EnhancedInput->BindAction(Character->GetInputActions().LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLookMouse);
	EnhancedInput->BindAction(Character->GetInputActions().LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLook);
	EnhancedInput->BindAction(Character->GetInputActions().MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnMove);
	EnhancedInput->BindAction(Character->GetInputActions().SprintAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnSprint);
	EnhancedInput->BindAction(Character->GetInputActions().WalkAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnWalk);
	EnhancedInput->BindAction(Character->GetInputActions().CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnCrouch);
	EnhancedInput->BindAction(Character->GetInputActions().JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnJump);
	EnhancedInput->BindAction(Character->GetInputActions().AimAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnAim);
	EnhancedInput->BindAction(Character->GetInputActions().RagdollAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRagdoll);
	EnhancedInput->BindAction(Character->GetInputActions().RollAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRoll);
	EnhancedInput->BindAction(Character->GetInputActions().RotationModeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRotationMode);
	EnhancedInput->BindAction(Character->GetInputActions().ViewModeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnViewMode);

	Character->GetInputActions().SetMoveInputValue(EnhancedInput->BindActionValue(Character->GetInputActions().MoveAction));
}
