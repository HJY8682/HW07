#include "MyPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

/* ������������������������������������������������������ */

AMyPawn::AMyPawn()
{
    /* ���� ������Ʈ ���� ���� */
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

/* ������������������������������������������������������ */

void AMyPawn::BeginPlay()
{
    Super::BeginPlay();

    /* IMC ��� */
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

/* ������������������������������������������������������ */

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        /* ���� �̵� ���� : Ű�� ���� ��(Triggered)�� �� ��(Completed) ��� ���� �Լ��� */
        if (IA_Move)
        {
            EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyPawn::Move);
            EIC->BindAction(IA_Move, ETriggerEvent::Completed, this, &AMyPawn::Move);
        }

        /* ���� ���콺 Look ���� */
        if (IA_Look)
        {
            /* ���콺�� �����̴� ���� �� ���� */
            EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyPawn::Look);

            /* ���콺�� ����(��ư���� �� ��) ���� 0 �� ���� */
            EIC->BindAction(IA_Look, ETriggerEvent::Completed, this, &AMyPawn::Look);
        }
    }
}

/* ������������������������������������������������������ */

void AMyPawn::Move(const FInputActionValue& Value)
{
    /* WASD �� X,Y �� (Released �� 0,0) */
    CachedMove = Value.Get<FVector2D>();
}

void AMyPawn::Look(const FInputActionValue& Value)
{
    CachedLook = Value.Get<FVector2D>();

    /* ���� ���� ���� 0���� ���� (�ɼ�) */
    if (FMath::IsNearlyZero(CachedLook.X, 0.001f))
        CachedLook.X = 0.f;
    if (FMath::IsNearlyZero(CachedLook.Y, 0.001f))
        CachedLook.Y = 0.f;
}

/* ������������������������������������������������������ */

void AMyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /* ���� 1) �̵� : ī�޶� ���� ���� �������� ���� */
    if (!CachedMove.IsNearlyZero())
    {
        /* ī�޶��� ���� Yaw�� �̿� */
        const FRotator CamYaw(0.f, Camera->GetComponentRotation().Yaw, 0.f);

        const FVector CamForward = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::X);
        const FVector CamRight = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::Y);

        const FVector Dir = CamForward * CachedMove.Y + CamRight * CachedMove.X;

        AddActorWorldOffset(Dir * MoveSpeed * DeltaTime, true);  // �浹 ����
    }

    /* ���� 2) ȸ��(Yaw) : ���콺 �������� �׻� �ݿ� ���� */
    if (!CachedLook.IsNearlyZero())
    {
        const FRotator DeltaRot(0.f, CachedLook.X * TurnSpeed * DeltaTime, 0.f);
        AddActorLocalRotation(DeltaRot, true);
        // Pitch(����)�� �ʿ��ϸ� SpringArm�� ����
    }
}
