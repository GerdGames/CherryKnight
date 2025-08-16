// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#include "GBAHeaderViewConstructorListItem.h"

#include "LineEndings/GBALineEndings.h"
#include "Models/GBAAttributeSetWizardViewModel.h"

#define LOCTEXT_NAMESPACE "FGBAHeaderViewConstructorListItem"

void FGBAHeaderViewConstructorListItem::ExtendContextMenu(FMenuBuilder& InMenuBuilder, TWeakObjectPtr<UObject> InAsset)
{
}

FGBAHeaderViewConstructorListItem::FGBAHeaderViewConstructorListItem(const TSharedPtr<FGBAAttributeSetWizardViewModel>& InViewModel)
{
	check(InViewModel.IsValid());

	if (!InViewModel->GetSelectedBlueprint().IsValid())
	{
		return;
	}

	const UBlueprint* Blueprint = InViewModel->GetSelectedBlueprint().Get();
	check(Blueprint);
	
	const FString DesiredClassName = GetClassNameToGenerate(Blueprint, InViewModel->GetNewClassName());

	// Add new line
	RawItemString += TEXT("\n");
	RichTextString += TEXT("\n");
	
	// Comment
	{
		const FString ClassName = GetClassNameToGenerate(Blueprint, InViewModel->GetNewClassName());
		const FString Comment = TEXT("Default constructor");

		FString RawString;
		FString RichString;
		FormatCommentString(Comment, RawString, RichString);

		RawItemString += RawString;
		RichTextString += RichString;
	}
	
	RawItemString += TEXT("\n");
	RichTextString += TEXT("\n");
	
	RawItemString += FString::Printf(TEXT("%s();"), *DesiredClassName);
	RichTextString += FString::Printf(
		TEXT("<%s>%s</>();"),
		*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
		*DesiredClassName
	);

	RawItemString.InsertAt(0, TEXT("\t"));
	RichTextString.InsertAt(0, TEXT("\t"));
	RawItemString.ReplaceInline(TEXT("\n"), TEXT("\n\t"));
	RichTextString.ReplaceInline(TEXT("\n"), TEXT("\n\t"));

	// normalize to platform newlines
	GBA::String::ToHostLineEndingsInline(RawItemString);
	GBA::String::ToHostLineEndingsInline(RichTextString);
}

FGBAHeaderViewListItemPtr FGBAHeaderViewConstructorListItem::Create(const TSharedPtr<FGBAAttributeSetWizardViewModel>& InViewModel)
{
	return MakeShared<FGBAHeaderViewConstructorListItem>(InViewModel);
}

#undef LOCTEXT_NAMESPACE
