

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MassHordeActor.generated.h"

UCLASS()
class MASSHORDE_API AMassHordeActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AMassHordeActor();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* MeshComponent;
	
};
