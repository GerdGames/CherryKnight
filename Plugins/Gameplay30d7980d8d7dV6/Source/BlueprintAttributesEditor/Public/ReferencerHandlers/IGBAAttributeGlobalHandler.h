// Copyright 2022-2025 Mickael Daniel. All Rights Reserved.

#pragma once

struct FGBAAttributeReferencerPayload;

/**
 * Interface for custom referencer handler not tied to specific references
 *
 * Gets lifecycle methods invoked once per event, while referencer handler tied to a given UObject CDO gets invoked
 * once per referencer (assets having a dependency on AttributeSet)
 */
class BLUEPRINTATTRIBUTESEDITOR_API IGBAAttributeGlobalHandler : public TSharedFromThis<IGBAAttributeGlobalHandler>
{
public:
	virtual ~IGBAAttributeGlobalHandler() = default;

	virtual void OnPreCompile(const FString& InPackageName) = 0;
	virtual void OnPostCompile(const FString& InPackageName) = 0;
	
	virtual bool HandleAttributeRename(const TArray<FAssetData>& InReferencers, const FGBAAttributeReferencerPayload& InPayload, TArray<TSharedRef<FTokenizedMessage>>& OutMessages) = 0;
	virtual bool HandleAttributeRemoved(const TArray<FAssetData>& InReferencers, const FGBAAttributeReferencerPayload& InPayload, TArray<TSharedRef<FTokenizedMessage>>& OutMessages) = 0;
};
