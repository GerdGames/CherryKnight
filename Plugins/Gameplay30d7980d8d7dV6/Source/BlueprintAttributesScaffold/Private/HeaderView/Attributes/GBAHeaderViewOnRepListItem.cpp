// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#include "GBAHeaderViewOnRepListItem.h"

#include "LineEndings/GBALineEndings.h"
#include "UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "FGBAHeaderViewOnRepListItem"

FGBAHeaderViewListItemPtr FGBAHeaderViewOnRepListItem::Create(const TSharedPtr<FGBAAttributeSetWizardViewModel>& InViewModel, const FProperty& InVarProperty)
{
	return MakeShared<FGBAHeaderViewOnRepListItem>(InViewModel, InVarProperty);
}

void FGBAHeaderViewOnRepListItem::ExtendContextMenu(FMenuBuilder& InMenuBuilder, TWeakObjectPtr<UObject> InAsset)
{
}

FGBAHeaderViewOnRepListItem::FGBAHeaderViewOnRepListItem(const TSharedPtr<FGBAAttributeSetWizardViewModel>& InViewModel, const FProperty& VarProperty)
{
	// Add the UFUNCTION specifiers
	// i.e. UFUNCTION()
	{
		RawItemString += TEXT("\nUFUNCTION()");
		RichTextString += FString::Printf(TEXT("\n<%s>UFUNCTION</>()"), *GBA::HeaderViewSyntaxDecorators::MacroDecorator);
	}
	
	const FString ClampedPropertyTypename = TEXT("FGBAGameplayClampedAttributeData");
	
	// FGBAGameplayClampedAttributeData handling requires logic in PostGameplayEffectExecute handled by UGBAAttributeSetBlueprintBase
	const bool bSupportsClampedAttributeData = IsSupportingClampedAttributeData(InViewModel);
	const bool bIsClampedAttributeData = VarProperty.GetCPPType() == ClampedPropertyTypename;

	// Add the function declaration line
	// i.e. virtual void OnRep_AttributeName(Type OldVarName)
	{
		FString Typename = GetCPPTypenameForProperty(&VarProperty, /*bIsMemberProperty=*/true);
		const FString VarName = VarProperty.GetAuthoredName();
		const bool bLegalName = IsValidCPPIdentifier(VarName);
		
		if (bIsClampedAttributeData && !bSupportsClampedAttributeData)
		{
			// Convert FGBAGameplayClampedAttributeData to regular FGameplayAttributeData in case parent class is not
			// of type UGBAAttributeSetBlueprintBase, whose PostGameplayEffectExecute (or PreAttributeChange / PreAttributeBaseChange
			// if reworked)
			Typename = TEXT("FGameplayAttributeData");
		}

		const FString* IdentifierDecorator = &GBA::HeaderViewSyntaxDecorators::IdentifierDecorator;
		if (!bLegalName)
		{
			IllegalName = FName(VarName);
			IdentifierDecorator = &GBA::HeaderViewSyntaxDecorators::ErrorDecorator;
		}

		if (ShouldUseConstRef(Typename))
		{
			RawItemString += FString::Printf(TEXT("\nvirtual void OnRep_%s(const %s& Old%s);"), *VarName, *Typename, *VarName);
			RichTextString += FString::Printf(
				TEXT("\n<%s>virtual</> <%s>void</> <%s>OnRep_%s</>(<%s>const</> <%s>%s</>& <%s>Old%s</>);"),
				*GBA::HeaderViewSyntaxDecorators::KeywordDecorator,
				*GBA::HeaderViewSyntaxDecorators::KeywordDecorator,
				**IdentifierDecorator,
				*VarName,
				*GBA::HeaderViewSyntaxDecorators::KeywordDecorator,
				*GBA::HeaderViewSyntaxDecorators::TypenameDecorator,
				*Typename,
				**IdentifierDecorator,
				*VarName
			);
		}
		else
		{
			RawItemString += FString::Printf(TEXT("\nvirtual void OnRep_%s(%s Old%s);"), *VarName, *Typename, *VarName);
			RichTextString += FString::Printf(
				TEXT("\n<%s>virtual</> <%s>void</> <%s>OnRep_%s</>(<%s>%s</> <%s>Old%s</>);"),
				*GBA::HeaderViewSyntaxDecorators::KeywordDecorator,
				*GBA::HeaderViewSyntaxDecorators::KeywordDecorator,
				**IdentifierDecorator,
				*VarName,
				*GBA::HeaderViewSyntaxDecorators::TypenameDecorator,
				*Typename,
				**IdentifierDecorator,
				*VarName
			);
		}
	}

	// indent item
	RawItemString.InsertAt(0, TEXT("\t"));
	RichTextString.InsertAt(0, TEXT("\t"));
	RawItemString.ReplaceInline(TEXT("\n"), TEXT("\n\t"));
	RichTextString.ReplaceInline(TEXT("\n"), TEXT("\n\t"));

	// normalize to platform newlines
	GBA::String::ToHostLineEndingsInline(RawItemString);
	GBA::String::ToHostLineEndingsInline(RichTextString);
}

#undef LOCTEXT_NAMESPACE
