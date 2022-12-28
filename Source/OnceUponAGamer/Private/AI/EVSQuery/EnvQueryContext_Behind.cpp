// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EVSQuery/EnvQueryContext_Behind.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "GameFramework/Actor.h"

void UEnvQueryContext_Behind::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	FVector Loc = QueryOwner->GetActorForwardVector() * -1;
//	UEnvQueryItemType_Actor::SetContextHelper(ContextData, QueryOwner);
	UEnvQueryItemType_Point::SetContextHelper(ContextData, FVector(0.5,-0.9,0.1));
}
