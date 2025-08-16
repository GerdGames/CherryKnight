// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UGameplayAbility;

struct BLUEPRINTATTRIBUTES_API FGBADelegates
{
	DECLARE_MULTICAST_DELEGATE_TwoParams(FGBAOnVariableAddedOrRemoved, const FName&, const FName&);
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FGBAOnVariableRenamed, const FName&, const FName&, const FName&);
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FGBAOnVariableTypeChanged, const FName&, FString, UObject*);
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FGBAOnPostCompile, const FName&);
	DECLARE_MULTICAST_DELEGATE_OneParam(FGBAOnPreCompile, const FName&);

	DECLARE_MULTICAST_DELEGATE(FGBAOnRequestDetailsRefresh)

	/**
	 * Triggered whenever a variable is added to a GBA Blueprint
	 *
	 * @param PackageName the package FName of the Blueprint where the variable was added
	 * @param PropertyName the property name that was added
	 */
	static FGBAOnVariableAddedOrRemoved OnVariableAdded;
	
	/**
	 * Triggered whenever a variable is removed from a GBA Blueprint
	 *
	 * @param PackageName the package FName of the Blueprint from which the variable was removed
	 * @param PropertyName the property name that was removed
	 */
	static FGBAOnVariableAddedOrRemoved OnVariableRemoved;
	
	/**
	 * Triggered whenever a variable is removed from a GBA Blueprint
	 *
	 * @param PackageName the package FName of the Blueprint from which the variable was renamed
	 * @param OldPropertyName the old property name
	 * @param NewPropertyName the new property name after rename
	 */
	static FGBAOnVariableRenamed OnVariableRenamed;
	static FGBAOnVariableTypeChanged OnVariableTypeChanged;
	
	static FGBAOnPreCompile OnPreCompile;
	static FGBAOnPostCompile OnPostCompile;
	
	static FGBAOnRequestDetailsRefresh OnRequestDetailsRefresh;
};
