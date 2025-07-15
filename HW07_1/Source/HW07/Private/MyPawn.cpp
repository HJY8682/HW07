#include "MyPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

/* ─────────────────────────── */

AMyPawn::AMyPawn()
{
    /* ── 컴포넌트 생성 ── */
    Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    RootComponent = Capsule;
    Capsule->InitCapsuleSize(42.f, 96.f);
    Capsule->SetSimulatePhysics(false);

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetSimulatePhysics(false);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

    AutoPossessPlayer = EAutoReceiveInput::Player0;
    PrimaryActorTick.bCanEverTick = true;
}

/* ─────────────────────────── */

void AMyPawn::BeginPlay()
{
    Super::BeginPlay();

    /* IMC 등록 */
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsys =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (IMC_Player)
                    Subsys->AddMappingContext(IMC_Player, 0);
            }
        }
    }
}

/* ─────────────────────────── */

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        /* ── 이동 ── : 키를 누를 때(Triggered)와 뗄 때(Completed) 모두 같은 함수로 */
        if (IA_Move)
        {
            EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyPawn::Move);
            EIC->BindAction(IA_Move, ETriggerEvent::Completed, this, &AMyPawn::Move);
        }

        /* ── 마우스 Look ── */
        if (IA_Look)
        {
            /* 마우스를 움직이는 동안 값 전달 */
            EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyPawn::Look);

            /* 마우스를 멈춘(버튼에서 손 뗀) 순간 0 값 전달 */
            EIC->BindAction(IA_Look, ETriggerEvent::Completed, this, &AMyPawn::Look);
        }
    }
}

/* ─────────────────────────── */

void AMyPawn::Move(const FInputActionValue& Value)
{
    /* WASD → X,Y 값 (Released 시 0,0) */
    CachedMove = Value.Get<FVector2D>();
}

void AMyPawn::Look(const FInputActionValue& Value)
{
    CachedLook = Value.Get<FVector2D>();

    /* 아주 작은 값은 0으로 정리 (옵션) */
    if (FMath::IsNearlyZero(CachedLook.X, 0.001f))
        CachedLook.X = 0.f;
    if (FMath::IsNearlyZero(CachedLook.Y, 0.001f))
        CachedLook.Y = 0.f;
}

/* ─────────────────────────── */

void AMyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /* ── 1) 이동 : 카메라가 보는 방향 기준으로 ── */
    if (!CachedMove.IsNearlyZero())
    {
        /* 카메라의 수평 Yaw만 이용 */
        const FRotator CamYaw(0.f, Camera->GetComponentRotation().Yaw, 0.f);

        const FVector CamForward = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::X);
        const FVector CamRight = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::Y);

        const FVector Dir = CamForward * CachedMove.Y + CamRight * CachedMove.X;

        AddActorWorldOffset(Dir * MoveSpeed * DeltaTime, true);  // 충돌 포함
    }

    /* ── 2) 회전(Yaw) : 마우스 움직임은 항상 반영 ── */
    if (!CachedLook.IsNearlyZero())
    {
        const FRotator DeltaRot(0.f, CachedLook.X * TurnSpeed * DeltaTime, 0.f);
        AddActorLocalRotation(DeltaRot, true);
        // Pitch(상·하)는 필요하면 SpringArm에 적용
    }
}
