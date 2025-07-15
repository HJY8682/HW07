#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "MyPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class HW07_API AMyPawn : public APawn
{
    GENERATED_BODY()

public:
    AMyPawn();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    /* ── 컴포넌트 ── */
    UPROPERTY(VisibleAnywhere)
    UCapsuleComponent* Capsule;

    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* Camera;

    /* ── 이동 파라미터 ── */
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveSpeed = 600.f;     // cm/s

    UPROPERTY(EditAnywhere, Category = "Movement")
    float TurnSpeed = 120.f;     // deg/s

    /* ── Enhanced Input 에셋 ── */
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputMappingContext* IMC_Player;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* IA_Move;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* IA_Look;

private:
    /* ── 입력 콜백 ── */
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

    FVector2D CachedMove{ 0.f, 0.f };
    FVector2D CachedLook{ 0.f, 0.f };
};
