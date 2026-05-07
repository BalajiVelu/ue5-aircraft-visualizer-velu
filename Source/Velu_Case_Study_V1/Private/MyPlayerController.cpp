#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "CameraManagerActor.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;
    SetInputMode(FInputModeGameOnly());

    // Add input mapping
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (InputMapping)
        {
            Subsystem->AddMappingContext(InputMapping, 0);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: InputMapping not set!"));
        }
    }

    // Find CameraManagerActor using tag "camconnect"
    CameraManager = nullptr;
    TArray<AActor*> FoundActors;
    FName CameraTag = TEXT("camconnect");

    UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACameraManagerActor::StaticClass(), CameraTag, FoundActors);

    if (FoundActors.Num() > 0)
    {
        CameraManager = Cast<ACameraManagerActor>(FoundActors[0]);
        if (CameraManager)
        {
            UE_LOG(LogTemp, Log, TEXT("MyPlayerController: CameraManagerActor found with tag '%s'."), *CameraTag.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: Found actor but failed to cast to CameraManagerActor."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: No CameraManagerActor found with tag '%s'."), *CameraTag.ToString());
    }
}

void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (Camera1Action)
        {
            EIC->BindAction(Camera1Action, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleCamera1);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: Camera1Action not set!"));
        }

        if (Camera2Action)
        {
            EIC->BindAction(Camera2Action, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleCamera2);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: Camera2Action not set!"));
        }

        if (Camera3Action)
        {
            EIC->BindAction(Camera3Action, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleCamera3);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: Camera3Action not set!"));
        }
    }
}

void AMyPlayerController::HandleCamera1(const FInputActionValue& Value)
{
    if (CameraManager)
    {
        CameraManager->SetCameraIndex(0);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: CameraManager is null when handling Camera1 input!"));
    }
}

void AMyPlayerController::HandleCamera2(const FInputActionValue& Value)
{
    if (CameraManager)
    {
        CameraManager->SetCameraIndex(1);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: CameraManager is null when handling Camera2 input!"));
    }
}

void AMyPlayerController::HandleCamera3(const FInputActionValue& Value)
{
    if (CameraManager)
    {
        CameraManager->SetCameraIndex(2);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: CameraManager is null when handling Camera3 input!"));
    }
}
