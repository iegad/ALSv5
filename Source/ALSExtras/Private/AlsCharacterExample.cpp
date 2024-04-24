#include "AlsCharacterExample.h"

#include "AlsCameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsCharacterExample)

AAlsCharacterExample::AAlsCharacterExample()
{
	Camera = CreateDefaultSubobject<UAlsCameraComponent>(FName{TEXTVIEW("Camera")});
	Camera->SetupAttachment(GetMesh());
	Camera->SetRelativeRotation_Direct({0.0f, 90.0f, 0.0f});
}

void AAlsCharacterExample::NotifyControllerChanged()
{
	const auto* PreviousPlayer{Cast<APlayerController>(PreviousController)};
	if (IsValid(PreviousPlayer))
	{
		auto* InputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPlayer->GetLocalPlayer())};
		if (IsValid(InputSubsystem))
		{
			InputSubsystem->RemoveMappingContext(InputActions.InputMappingContext);
		}
	}

	auto* NewPlayer{Cast<APlayerController>(GetController())};
	if (IsValid(NewPlayer))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		auto* InputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer())};
		if (IsValid(InputSubsystem))
		{
			FModifyContextOptions Options;
			Options.bNotifyUserSettings = true;

			InputSubsystem->AddMappingContext(InputActions.InputMappingContext, 0, Options);
		}
	}

	Super::NotifyControllerChanged();
}

void 
AAlsCharacterExample::Tick(float InDeltaTime) {
	Super::Tick(InDeltaTime);

	if (IsPlayerControlled()) {
		auto&& FeetTraceStart = GetMesh()->GetSocketLocation(TEXT("root")) + GetActorUpVector() * 10.f;
		auto&& FeetTraceEnd = GetMesh()->GetSocketLocation(TEXT("root")) + GetActorUpVector() * -10.f;
		FHitResult LandResult;
		UKismetSystemLibrary::LineTraceSingle(GetWorld(), FeetTraceStart, FeetTraceEnd,
			UEngineTypes::ConvertToTraceType(ECC_Visibility), true, {}, EDrawDebugTrace::ForOneFrame, LandResult, true);
		if (LandResult.bBlockingHit) {
			if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Swimming) {
				GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				return;
			}
		} 
		
		auto&& WaterTraceStart = GetMesh()->GetSocketLocation(TEXT("WaterTracer"));
		FHitResult WaterResult;
		UKismetSystemLibrary::SphereTraceSingle(GetWorld(), WaterTraceStart, WaterTraceStart, 5.f,
			UEngineTypes::ConvertToTraceType(ECC_TraceWater), true, {}, EDrawDebugTrace::ForOneFrame, WaterResult, true);

		SetValue(SwimmingUp, WaterResult.bBlockingHit);
		if (WaterResult.bBlockingHit) {
			if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Swimming) {
				GetCharacterMovement()->SetMovementMode(MOVE_Swimming);
			}
		}
	}
}

void AAlsCharacterExample::CalcCamera(const float DeltaTime, FMinimalViewInfo& ViewInfo)
{
	if (Camera->IsActive())
	{
		Camera->GetViewInfo(ViewInfo);
		return;
	}

	Super::CalcCamera(DeltaTime, ViewInfo);
}

void AAlsCharacterExample::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	auto* EnhancedInput{Cast<UEnhancedInputComponent>(Input)};
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(InputActions.LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLookMouse);
		EnhancedInput->BindAction(InputActions.LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLook);
		EnhancedInput->BindAction(InputActions.MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnMove);
		EnhancedInput->BindAction(InputActions.SprintAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnSprint);
		EnhancedInput->BindAction(InputActions.WalkAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnWalk);
		EnhancedInput->BindAction(InputActions.CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnCrouch);
		EnhancedInput->BindAction(InputActions.JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnJump);
		EnhancedInput->BindAction(InputActions.AimAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnAim);
		EnhancedInput->BindAction(InputActions.RagdollAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRagdoll);
		EnhancedInput->BindAction(InputActions.RollAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRoll);
		EnhancedInput->BindAction(InputActions.RotationModeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRotationMode);
		EnhancedInput->BindAction(InputActions.ViewModeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnViewMode);
		EnhancedInput->BindAction(InputActions.SwitchShoulderAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnSwitchShoulder);

		InputActions.SetMoveInputValue(EnhancedInput->BindActionValue(InputActions.MoveAction));
	}
}

void AAlsCharacterExample::Input_OnLookMouse(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * InputActions.LookUpMouseSensitivity);
	AddControllerYawInput(Value.X * InputActions.LookRightMouseSensitivity);
}

void AAlsCharacterExample::Input_OnLook(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * InputActions.LookUpRate);
	AddControllerYawInput(Value.X * InputActions.LookRightRate);
}

void AAlsCharacterExample::Input_OnMove(const FInputActionValue& ActionValue)
{
	const auto Value{UAlsMath::ClampMagnitude012D(ActionValue.Get<FVector2D>())};

	const auto ForwardDirection{UAlsMath::AngleToDirectionXY(UE_REAL_TO_FLOAT(GetViewState().Rotation.Yaw))};
	const auto RightDirection{UAlsMath::PerpendicularCounterClockwiseXY(ForwardDirection)};

	AddMovementInput(ForwardDirection * Value.Y + RightDirection * Value.X);
}

void AAlsCharacterExample::Input_OnSprint(const FInputActionValue& ActionValue)
{
	SetDesiredGait(ActionValue.Get<bool>() ? AlsGaitTags::Sprinting : AlsGaitTags::Running);
}

void AAlsCharacterExample::Input_OnWalk()
{
	if (GetDesiredGait() == AlsGaitTags::Walking)
	{
		SetDesiredGait(AlsGaitTags::Running);
	}
	else if (GetDesiredGait() == AlsGaitTags::Running)
	{
		SetDesiredGait(AlsGaitTags::Walking);
	}
}

void AAlsCharacterExample::Input_OnCrouch()
{
	if (GetDesiredStance() == AlsStanceTags::Standing)
	{
		SetDesiredStance(AlsStanceTags::Crouching);
	}
	else if (GetDesiredStance() == AlsStanceTags::Crouching)
	{
		SetDesiredStance(AlsStanceTags::Standing);
	}
}

void AAlsCharacterExample::Input_OnJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
	{
		if (StopRagdolling())
		{
			return;
		}

		if (StartMantlingGrounded())
		{
			return;
		}

		if (GetCharacterMovement()->MovementMode == MOVE_Swimming && SwimmingUp) {
			AddMovementInput(GetActorUpVector());
			return;
		}

		if (GetStance() == AlsStanceTags::Crouching)
		{
			SetDesiredStance(AlsStanceTags::Standing);
			return;
		}

		Jump();
	}
	else
	{
		StopJumping();
	}
}

void AAlsCharacterExample::Input_OnAim(const FInputActionValue& ActionValue)
{
	SetDesiredAiming(ActionValue.Get<bool>());
}

void AAlsCharacterExample::Input_OnRagdoll()
{
	if (!StopRagdolling())
	{
		StartRagdolling();
	}
}

void AAlsCharacterExample::Input_OnRoll()
{
	static constexpr auto PlayRate{1.3f};

	StartRolling(PlayRate);
}

void AAlsCharacterExample::Input_OnRotationMode()
{
	SetDesiredRotationMode(GetDesiredRotationMode() == AlsRotationModeTags::VelocityDirection
		                       ? AlsRotationModeTags::ViewDirection
		                       : AlsRotationModeTags::VelocityDirection);
}

void AAlsCharacterExample::Input_OnViewMode()
{
	SetViewMode(GetViewMode() == AlsViewModeTags::ThirdPerson ? AlsViewModeTags::FirstPerson : AlsViewModeTags::ThirdPerson);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AAlsCharacterExample::Input_OnSwitchShoulder()
{
	Camera->SetRightShoulder(!Camera->IsRightShoulder());
}

void AAlsCharacterExample::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation)
{
	if (Camera->IsActive())
	{
		Camera->DisplayDebug(Canvas, DisplayInfo, VerticalLocation);
	}

	Super::DisplayDebug(Canvas, DisplayInfo, Unused, VerticalLocation);
}
