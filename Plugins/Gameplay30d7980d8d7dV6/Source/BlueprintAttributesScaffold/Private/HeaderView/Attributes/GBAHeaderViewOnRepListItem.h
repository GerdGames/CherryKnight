// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBAHeaderViewListItem.h"

class FGBAAttributeSetWizardViewModel;

/** A header view list item that displays a gameplay attribute data variable declaration */
struct FGBAHeaderViewOnRepListItem : public FGBAHeaderViewListItem
{
	/** Creates a list item for the Header view representing a variable declaration for the given blueprint variable */
	static FGBAHeaderViewListItemPtr Create(const TSharedPtr<FGBAAttributeSetWizardViewModel>& InViewModel, const FProperty& InVarProperty);

	//~ FHeaderViewListItem Interface
	virtual void ExtendContextMenu(FMenuBuilder& InMenuBuilder, TWeakObjectPtr<UObject> InAsset) override;
	//~ End FHeaderViewListItem Interface

	explicit FGBAHeaderViewOnRepListItem(const TSharedPtr<FGBAAttributeSetWizardViewModel>& InViewModel, const FProperty& VarProperty);

protected:
	/** None if the name is legal, else holds the name of the variable */
	FName IllegalName = NAME_None;
};
