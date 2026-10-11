// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/AttributeInfo.h"

FGASAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
    for (const FGASAttributeInfo& AttributeInfo : AttributeInfos)
    {
        if (AttributeInfo.AttributeTag.MatchesTag(AttributeTag))
        {
            return AttributeInfo;
        }
    }
    if (bLogNotFound)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attribute Info not found for tag: %s"), *AttributeTag.ToString());
    }
    return FGASAttributeInfo();
}
