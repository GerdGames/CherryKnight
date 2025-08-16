// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#include "GBASourceViewConstructorListItem.h"

#include "GBAScaffoldLog.h"
#include "Abilities/GBAAttributeSetBlueprintBase.h"
#include "Engine/Blueprint.h"
#include "LineEndings/GBALineEndings.h"
#include "Models/GBAAttributeSetWizardViewModel.h"

#define LOCTEXT_NAMESPACE "FGBASourceViewConstructorListItem"

FGBAHeaderViewListItemPtr FGBASourceViewConstructorListItem::Create(const TSharedPtr<FGBAAttributeSetWizardViewModel>& InViewModel)
{
	return MakeShared<FGBASourceViewConstructorListItem>(InViewModel);
}

void FGBASourceViewConstructorListItem::ExtendContextMenu(FMenuBuilder& InMenuBuilder, TWeakObjectPtr<UObject> InAsset)
{
}

FGBASourceViewConstructorListItem::FGBASourceViewConstructorListItem(const TSharedPtr<FGBAAttributeSetWizardViewModel>& InViewModel)
{
	check(InViewModel.IsValid());

	if (!InViewModel->GetSelectedBlueprint().IsValid())
	{
		return;
	}

	const UBlueprint* Blueprint = InViewModel->GetSelectedBlueprint().Get();
	check(Blueprint);

	const FString DesiredClassName = GetClassNameToGenerate(Blueprint, InViewModel->GetNewClassName());

	RawItemString += FString::Printf(TEXT("%s::%s()"), *DesiredClassName, *DesiredClassName);
	RichTextString += FString::Printf(
		TEXT("<%s>%s</>::<%s>%s</>()"),
		*GBA::HeaderViewSyntaxDecorators::TypenameDecorator,
		*DesiredClassName,
		*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
		*DesiredClassName
	);

	// Add opening brace line
	RawItemString += TEXT("\n{\n");
	RichTextString += TEXT("\n{\n");

	// Add clamp definitions if we have any clamped attribute to handle
	const bool bSupportsClampedAttributeData = IsSupportingClampedAttributeData(InViewModel);
	if (IsSupportingClampedAttributeData(InViewModel))
	{
		// Filter to only clamped attributes
		// TArray<const FProperty*> Props = GetAllProperties(Blueprint->GeneratedClass).FilterByPredicate([](const FProperty* InProperty)
		TArray<const FProperty*> Props = GetAllProperties(Blueprint->GeneratedClass).FilterByPredicate([](const FProperty* InProperty)
		{
			return InProperty->GetCPPType() == TEXT("FGBAGameplayClampedAttributeData");
		});

		GBA_SCAFFOLD_LOG(Verbose, TEXT("Props: %d"), Props.Num())
		int32 CurrentIndex = 0;
		for (const FProperty* Prop : Props)
		{
			if (!Prop)
			{
				continue;
			}


			FString AttributeRawItemString;
			FString AttributeRichTextString;
			GetCodeTextsForClampedProperty(Prop, AttributeRawItemString, AttributeRichTextString);

			CurrentIndex++;

			RawItemString += AttributeRawItemString;
			RichTextString += AttributeRichTextString;

			if (CurrentIndex < Props.Num())
			{
				RawItemString += TEXT("\n");
				RichTextString += TEXT("\n");
			}
		}
	}
	
	// Add closing brace line
	RawItemString += TEXT("}\n");
	RichTextString += TEXT("}\n");

	// normalize to platform newlines
	GBA::String::ToHostLineEndingsInline(RawItemString);
	GBA::String::ToHostLineEndingsInline(RichTextString);
}

void FGBASourceViewConstructorListItem::GetCodeTextsForClampedProperty(const FProperty* InProperty, FString& OutRawItemString, FString& OutRichTextString)
{
	check(InProperty);

	const FString PropertyName = InProperty->GetName();

	const TSharedPtr<FGBAGameplayClampedAttributeData> AttributeData = GetGameplayAttributeData<FGBAGameplayClampedAttributeData>(*InProperty);
	check(AttributeData.IsValid());

	FGBAClampDefinition MaxValue = AttributeData->MaxValue;

	GetCodeTextsForClampDefinition(AttributeData->MinValue, PropertyName, TEXT("MinValue"), OutRawItemString, OutRichTextString);
	OutRawItemString += TEXT("\n");
	OutRichTextString += TEXT("\n");
	OutRawItemString += TEXT("\n");
	OutRichTextString += TEXT("\n");
	
	GetCodeTextsForClampDefinition(AttributeData->MaxValue, PropertyName, TEXT("MaxValue"), OutRawItemString, OutRichTextString);
	
	OutRawItemString += TEXT("\n");
	OutRichTextString += TEXT("\n");
}

void FGBASourceViewConstructorListItem::GetCodeTextsForClampDefinition(const FGBAClampDefinition& InDefinition, const FString& PropertyName, const FString& ClampDefinitionPropertyName, FString& OutRawItemString, FString& OutRichTextString)
{
	OutRawItemString += FString::Printf(
		TEXT("\t%s.%s.ClampType = %s;"),
		*PropertyName,
		*ClampDefinitionPropertyName,
		*UEnum::GetValueAsString(InDefinition.ClampType)
	);

	FString EnumName;
	FString ValueName;
	UEnum::GetValueAsString(InDefinition.ClampType).Split(TEXT("::"), &EnumName, &ValueName);
	
	OutRichTextString += FString::Printf(
		TEXT("\t<%s>%s</>.<%s>%s</>.<%s>ClampType</> = <%s>%s</>::%s;"),
		*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
		*PropertyName,
		*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
		*ClampDefinitionPropertyName,
		*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
		*GBA::HeaderViewSyntaxDecorators::TypenameDecorator,
		*EnumName,
		*ValueName
	);
	
	OutRawItemString += TEXT("\n");
	OutRichTextString += TEXT("\n");

	if (InDefinition.ClampType == EGBAClampingType::Float)
	{
		OutRawItemString += FString::Printf(
			TEXT("\t%s.%s.Value = %.2ff;"),
			*PropertyName,
			*ClampDefinitionPropertyName,
			InDefinition.Value
		);
		
		OutRichTextString += FString::Printf(
			TEXT("\t<%s>%s</>.<%s>%s</>.<%s>Value</> = <%s>%.2ff</>;"),
			*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
			*PropertyName,
			*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
			*ClampDefinitionPropertyName,
			*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
			*GBA::HeaderViewSyntaxDecorators::NumericLiteral,
			InDefinition.Value
		);
	}
	else if (InDefinition.ClampType == EGBAClampingType::AttributeBased)
	{
		OutRawItemString += FString::Printf(
			TEXT("\t%s.%s.Attribute = Get%sAttribute();"),
			*PropertyName,
			*ClampDefinitionPropertyName,
			*InDefinition.Attribute.GetName()
		);
		
		OutRichTextString += FString::Printf(
			TEXT("\t<%s>%s</>.<%s>%s</>.<%s>Attribute</> = <%s>Get%sAttribute()</>;"),
			*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
			*PropertyName,
			*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
			*ClampDefinitionPropertyName,
			*GBA::HeaderViewSyntaxDecorators::IdentifierDecorator,
			*GBA::HeaderViewSyntaxDecorators::MacroDecorator,
			*InDefinition.Attribute.GetName()
		);
	}
}


#undef LOCTEXT_NAMESPACE
