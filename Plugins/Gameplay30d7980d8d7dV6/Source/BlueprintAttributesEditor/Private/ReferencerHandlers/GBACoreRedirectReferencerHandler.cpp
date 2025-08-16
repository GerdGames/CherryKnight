// Copyright 2022-2025 Mickael Daniel. All Rights Reserved.

#include "GBACoreRedirectReferencerHandler.h"

#include "GBAEditorLog.h"
#include "UObject/CoreRedirects.h"
#include "AssetRegistry/AssetData.h"
#include "PackageTools.h"
#include "ReferencerHandlers/IGBAAttributeReferencerHandler.h"

FGBACoreRedirectReferencerHandler::~FGBACoreRedirectReferencerHandler()
{
	PackagesToReload.Reset();
}

void ExampleSetupRedirectors()
{
	// Just an example keeping around, not actually run
	TArray<FCoreRedirect> Redirects;
	Redirects.Emplace(
		ECoreRedirectFlags::Type_Property,
		TEXT("/Game/ThirdPerson/Blueprints/GBA_Health_Set.GBA_Health_Set_C.Oof4444"),
		TEXT("/Game/ThirdPerson/Blueprints/GBA_Health_Set.GBA_Health_Set_C.CoreRedirected")
	);

	Redirects.Emplace(
		ECoreRedirectFlags::Type_Property,
		TEXT("/Game/ThirdPerson/Blueprints/GBA_Health_Set.GBA_Health_Set_C.Oof3"),
		TEXT("/Script/BlueprintAttributes.GBAAttributeSetBlueprintBase.None")
	);
	
	FCoreRedirects::AddRedirectList(Redirects, TEXT("BlueprintAttributes"));
}

TSharedPtr<IGBAAttributeGlobalHandler> FGBACoreRedirectReferencerHandler::Create()
{
	return MakeShared<FGBACoreRedirectReferencerHandler>();
}

void FGBACoreRedirectReferencerHandler::OnPreCompile(const FString& InPackageName)
{
	GBA_EDITOR_NS_LOG(Verbose, TEXT("InPackageName: %s"), *InPackageName)
	PackagesToReload.Reset();
}

void FGBACoreRedirectReferencerHandler::OnPostCompile(const FString& InPackageName)
{
	GBA_EDITOR_NS_LOG(Verbose, TEXT("InPackageName: %s"), *InPackageName)
	GBA_EDITOR_NS_LOG(Verbose, TEXT("Reloading packages: %d"), PackagesToReload.Num())

	// Same action as right-clicking in the Content Browser > Advanced Actions > Reload and make PostSerialize() on
	// FGameplayAttributes to run again, picking up and doing the fixup on core redirected properties
	// 
	// Core redirect was added, trigger a reload on each referencer to make sure their FGameplayAttribute property
	// PostSerialize() is run again in loading mode, where the fixup on the core redirect happens.
	//
	// Unfortunately, PostSerialize() may still run with saving mode, and the code to make the FGameplayAttribute
	// asset registry searchable for reference viewer is not checking the OwnerVariant (which will be invalid on renamed
	// or removed attribute), while Attribute field path itself is filled. Leading to an editor crash.

	// Optionally, prompt for checkout and save after

	// HandleNextTick();

	// Or using a delayed reload
	// if (GEditor)
	// {
	// 	FTimerHandle Handle;
	// 	GEditor->GetTimerManager()->SetTimer(Handle, FTimerDelegate::CreateStatic(&FGBACoreRedirectReferencerHandler::HandleNextTick, InPackageName, PackagesToReload), 2.f, false);
	// 	
	// 	GEditor->GetTimerManager()->SetTimerForNextTick(FTimerDelegate::CreateStatic(&FGBACoreRedirectReferencerHandler::HandleNextTick, InPackageName, PackagesToReload));
	// }
}

bool FGBACoreRedirectReferencerHandler::HandleAttributeRename(const TArray<FAssetData>& InReferencers, const FGBAAttributeReferencerPayload& InPayload, TArray<TSharedRef<FTokenizedMessage>>& OutMessages)
{
	// InAssetIdentifier: None, InPayload: DefaultObject: None, PackageName: /Game/ThirdPerson/Blueprints/GBA_Health_Set, OldPropertyName: Test22, NewPropertyName: Test222
	GBA_EDITOR_NS_LOG(Verbose, TEXT("InPayload: %s"), *InPayload.ToString())

	const FString PackageName = InPayload.PackageName;

	FString PathName;
	FString ClassName;
	if (!PackageName.Split(TEXT("/"), &PathName, &ClassName, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		// Split not successful
		return false;
	}
	
	FString OldName = FString::Printf(TEXT("%s.%s_C.%s"), *InPayload.PackageName, *ClassName, *InPayload.OldPropertyName);
	FString NewName = FString::Printf(TEXT("%s.%s_C.%s"), *InPayload.PackageName, *ClassName, *InPayload.NewPropertyName);
	
	TArray<FCoreRedirect> Redirects;
	Redirects.Emplace(
		ECoreRedirectFlags::Type_Property,
		OldName,
		NewName
	);

	GBA_EDITOR_NS_LOG(Verbose, TEXT("Adding core redirect from '%s' to '%s'"), *OldName, *NewName)
	FCoreRedirects::AddRedirectList(Redirects, TEXT("BlueprintAttributes"));

	// TODO: If implemented, need to persist core redirect in .ini config file, or handle them as part of StartupModule()
	// optionally displaying them and allow to tweak them in Developer Settings.

	GBA_EDITOR_NS_LOG(Verbose, TEXT("Searching for referencers, if we can serialize them"))
	for (const FAssetData& AssetData : InReferencers)
	{
		UBlueprint* Blueprint = Cast<UBlueprint>(AssetData.GetAsset());
		if (!Blueprint)
		{
			continue;
		}

		const UPackage* Package = Blueprint->GetPackage();
		const UPackage* AssetDataPackage = AssetData.GetPackage();
		GBA_EDITOR_NS_LOG(
			Verbose,
			TEXT("\t Found Blueprint %s (%p), Package: %s, AssetDataPackage: %s, Same: %s"),
			*GetNameSafe(Blueprint),
			Blueprint,
			*GetNameSafe(Package),
			*GetNameSafe(AssetDataPackage),
			*LexToString(Package == AssetDataPackage)
		)
		PackagesToReload.AddUnique(AssetData.GetPackage());
	}
	return true;
}

bool FGBACoreRedirectReferencerHandler::HandleAttributeRemoved(const TArray<FAssetData>& InReferencers, const FGBAAttributeReferencerPayload& InPayload, TArray<TSharedRef<FTokenizedMessage>>& OutMessages)
{
	GBA_EDITOR_NS_LOG(Verbose, TEXT("InPayload: %s"), *InPayload.ToString())
	
	const FString PackageName = InPayload.PackageName;

	FString PathName;
	FString ClassName;
	if (!PackageName.Split(TEXT("/"), &PathName, &ClassName, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		// Split not successful
		return false;
	}
	
	FString OldName = FString::Printf(TEXT("%s.%s_C.%s"), *InPayload.PackageName, *ClassName, *InPayload.RemovedPropertyName);
	FString NewName = FString::Printf(TEXT("%s.%s_C.%s"), *InPayload.PackageName, *ClassName, TEXT("__DummyAttribute__"));
	
	TArray<FCoreRedirect> Redirects;
	Redirects.Emplace(
		ECoreRedirectFlags::Type_Property,
		OldName,
		NewName
	);

	GBA_EDITOR_NS_LOG(Verbose, TEXT("Adding core redirect from '%s' to '%s'"), *OldName, *NewName)
	FCoreRedirects::AddRedirectList(Redirects, TEXT("BlueprintAttributes"));
	return true;
}

// ReSharper disable once CppPassValueParameterByConstReference
void FGBACoreRedirectReferencerHandler::HandleNextTick(FString InPackageName, TArray<UPackage*> InPackagesToReload)
{
	GBA_EDITOR_NS_LOG(Verbose, TEXT("Reloading packages: %d"), InPackagesToReload.Num())
	if (InPackagesToReload.Num() > 0)
	{
		UPackageTools::ReloadPackages(InPackagesToReload);
	}

	// if (InPackagesToReload.Num() > 0)
	// {
	// 	UPackageTools::ReloadPackages(InPackagesToReload);
	// }

	// if (InPackagesToReload.Num() > 0)
	// {
	// 	FEditorFileUtils::FPromptForCheckoutAndSaveParams SaveParams;
	// 	SaveParams.bCheckDirty = false;
	// 	// SaveParams.bPromptToSave = false;
	// 	SaveParams.bPromptToSave = true;
	// 	SaveParams.bIsExplicitSave = true;
	//
	// 	FEditorFileUtils::PromptForCheckoutAndSave(InPackagesToReload, SaveParams);
	// }
}
