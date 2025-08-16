// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBAHeaderViewListItem.h"
#include "Abilities/GBAAttributeSetBlueprintBase.h"

class FGBAAttributeSetWizardViewModel;
struct FGBAGameplayClampedAttributeData;

/** A header view list item that displays a constructor declaration */
struct FGBASourceViewConstructorListItem : public FGBAHeaderViewListItem
{
	explicit FGBASourceViewConstructorListItem(const TSharedPtr<FGBAAttributeSetWizardViewModel>& InViewModel);
	
	static FGBAHeaderViewListItemPtr Create(const TSharedPtr<FGBAAttributeSetWizardViewModel>& InViewModel);

	//~ FHeaderViewListItem Interface
	virtual void ExtendContextMenu(FMenuBuilder& InMenuBuilder, TWeakObjectPtr<UObject> InAsset) override;
	//~ End FHeaderViewListItem Interface

protected:
	static void GetCodeTextsForClampedProperty(const FProperty* InProperty, FString& OutRawItemString, FString& OutRichTextString);

	static void GetCodeTextsForClampDefinition(const FGBAClampDefinition& InDefinition, const FString& PropertyName, const FString& ClampDefinitionPropertyName, FString& OutRawItemString, FString& OutRichTextString);
};
