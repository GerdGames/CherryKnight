// Copyright 2022-2025 Mickael Daniel. All Rights Reserved.

#pragma once

#include "ReferencerHandlers/IGBAAttributeGlobalHandler.h"

/**
 * Not used as of now (e.g. not registered towards GBAEditorSubsystem)
 *
 * Experimenting with using FCoreRedirectors for properties being renamed / removed.
 *
 * Unfortunately, not full reliable.
 */
class FGBACoreRedirectReferencerHandler : public IGBAAttributeGlobalHandler
{
public:
	virtual ~FGBACoreRedirectReferencerHandler() override;
	
	static TSharedPtr<IGBAAttributeGlobalHandler> Create();
	
	virtual void OnPreCompile(const FString& InPackageName) override;
	virtual void OnPostCompile(const FString& InPackageName) override;
	virtual bool HandleAttributeRename(const TArray<FAssetData>& InReferencers, const FGBAAttributeReferencerPayload& InPayload, TArray<TSharedRef<FTokenizedMessage>>& OutMessages) override;
	virtual bool HandleAttributeRemoved(const TArray<FAssetData>& InReferencers, const FGBAAttributeReferencerPayload& InPayload, TArray<TSharedRef<FTokenizedMessage>>& OutMessages) override;
	
protected:
	TArray<UPackage*> PackagesToReload;
	
	static void HandleNextTick(FString InPackageName, TArray<UPackage*> InPackagesToReload);
};
