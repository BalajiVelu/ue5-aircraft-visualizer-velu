#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "MyPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ACameraManagerActor; // Forward declare the class

UCLASS()
class VELU_CASE_STUDY_V1_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	// Removed EditAnywhere/EditInstanceOnly. We find the instance dynamically at runtime.
	// BlueprintReadWrite is kept if you need to access this reference from Blueprint graphs after BeginPlay.
	UPROPERTY(BlueprintReadWrite, Category = "Camera")
	ACameraManagerActor* CameraManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* Camera1Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* Camera2Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* Camera3Action;

private:
	void HandleCamera1(const FInputActionValue& Value);
	void HandleCamera2(const FInputActionValue& Value);
	void HandleCamera3(const FInputActionValue& Value);
};