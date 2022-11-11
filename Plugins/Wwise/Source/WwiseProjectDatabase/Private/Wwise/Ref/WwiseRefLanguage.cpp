/*******************************************************************************
The content of the files in this repository include portions of the
AUDIOKINETIC Wwise Technology released in source code form as part of the SDK
package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use these files in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Copyright (c) 2022 Audiokinetic Inc.
*******************************************************************************/

#include "Wwise/Ref/WwiseRefLanguage.h"
#include "Wwise/Stats/ProjectDatabase.h"
#include "Wwise/WwiseProjectDatabaseModule.h"

#include "Wwise/Metadata/WwiseMetadataLanguage.h"
#include "Wwise/Metadata/WwiseMetadataProjectInfo.h"

const TCHAR* const FWwiseRefLanguage::NAME = TEXT("Language");

const FWwiseMetadataLanguage* FWwiseRefLanguage::GetLanguage() const
{
	const auto* ProjectInfo = GetProjectInfo();
	if (UNLIKELY(!ProjectInfo))
	{
		return nullptr;
	}
	const auto& Platforms = ProjectInfo->Languages;
	if (Platforms.IsValidIndex(LanguageIndex))
	{
		return &Platforms[LanguageIndex];
	}
	else
	{
		UE_LOG(LogWwiseProjectDatabase, Error, TEXT("Could not get Language index #%zu"), LanguageIndex);
		return nullptr;
	}
}

uint32 FWwiseRefLanguage::LanguageId() const
{
	const auto* Language = GetLanguage();
	if (UNLIKELY(!Language))
	{
		return 0;
	}
	return Language->Id;
}

FGuid FWwiseRefLanguage::LanguageGuid() const
{
	const auto* Language = GetLanguage();
	if (UNLIKELY(!Language))
	{
		return {};
	}
	return Language->GUID;
}

FString FWwiseRefLanguage::LanguageName() const
{
	const auto* Language = GetLanguage();
	if (UNLIKELY(!Language))
	{
		return {};
	}
	return Language->Name;
}

uint32 FWwiseRefLanguage::Hash() const
{
	auto Result = FWwiseRefProjectInfo::Hash();
	Result = HashCombine(Result, ::GetTypeHash(LanguageIndex));
	return Result;
}