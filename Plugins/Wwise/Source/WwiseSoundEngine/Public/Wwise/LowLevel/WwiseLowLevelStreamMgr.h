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

#pragma once

#include "CoreMinimal.h"
#include "AkInclude.h"

#include "Wwise/WwiseSoundEngineModule.h"

class WWISESOUNDENGINE_API FWwiseLowLevelStreamMgr
{
public:
	static FWwiseLowLevelStreamMgr* Get()
	{
		IWwiseSoundEngineModule::ForceLoadModule();
		return IWwiseSoundEngineModule::StreamMgr;
	}

	static AK::IAkStreamMgr* GetAkStreamMgr();

	/// \name Audiokinetic implementation-specific Stream Manager factory.
	//@{
	/// Stream Manager factory.
	/// \remarks 
	/// - In order for the Stream Manager to work properly, you also need to create 
	/// at least one streaming device (and implement its I/O hook), and register the 
	/// File Location Resolver with AK::StreamMgr::SetFileLocationResolver().
	/// - Use AK::StreamMgr::GetDefaultSettings(), then modify the settings you want,
	/// then feed this function with them.
	/// \sa 
	/// - AK::IAkStreamMgr
	/// - AK::StreamMgr::SetFileLocationResolver()
	/// - AK::StreamMgr::GetDefaultSettings()
	AK::IAkStreamMgr* Create(
		const AkStreamMgrSettings& in_settings		///< Stream manager initialization settings.
		);

	/// Get the default values for the Stream Manager's settings.
	/// \sa 
	/// - AK::StreamMgr::Create()
	/// - AkStreamMgrSettings
	/// - \ref streamingmanager_settings
	void GetDefaultSettings(
		AkStreamMgrSettings& out_settings	///< Returned AkStreamMgrSettings structure with default values.
		);

	/// Get the one and only File Location Resolver registered to the Stream Manager.
	/// \sa
	/// - AK::StreamMgr::IAkFileLocationResolver
	/// - AK::StreamMgr::SetFileLocationResolver()
	AK::StreamMgr::IAkFileLocationResolver* GetFileLocationResolver();

	/// Register the one and only File Location Resolver to the Stream Manager.
	/// \sa 
	/// - AK::StreamMgr::IAkFileLocationResolver
	void SetFileLocationResolver(
		AK::StreamMgr::IAkFileLocationResolver* in_pFileLocationResolver ///< Interface to your File Location Resolver
		);

	//@}

	/// \name Stream Manager: High-level I/O devices management.
	//@{
	/// Streaming device creation.
	/// Creates a high-level device, with specific settings. 
	/// You need to provide the associated low-level I/O hook, implemented on your side.
	/// \return The device ID. AK_INVALID_DEVICE_ID if there was an error and it could not be created.
	/// \warning 
	/// - This function is not thread-safe.
	/// - Use a blocking hook (IAkIOHookBlocking) with SCHEDULER_BLOCKING devices, and a 
	/// deferred hook (IAkIOHookDeferredBatch) with SCHEDULER_DEFERRED_LINED_UP devices (these flags are
	/// specified in the device settings (AkDeviceSettings). The pointer to IAkLowLevelIOHook is
	/// statically cast internally into one of these hooks. Implementing the wrong (or no) interface
	/// will result into a crash.
	/// \remarks 
	/// - You may use AK::StreamMgr::GetDefaultDeviceSettings() first to get default values for the 
	/// settings, change those you want, then feed the structure to this function.
	/// - The returned device ID should be kept by the Low-Level IO, to assign it to file descriptors 
	/// in AK::StreamMgr::IAkFileLocationResolver::Open().
	/// \sa
	/// - AK::StreamMgr::IAkLowLevelIOHook
	/// - AK::StreamMgr::GetDefaultDeviceSettings()
	/// - \ref streamingmanager_settings
	AkDeviceID CreateDevice(
		const AkDeviceSettings& in_settings,		///< Device settings.
		AK::StreamMgr::IAkLowLevelIOHook* in_pLowLevelHook	///< Associated low-level I/O hook. Pass either a IAkIOHookBlocking or a IAkIOHookDeferredBatch interface, consistent with the type of the scheduler.
		);
	/// Streaming device destruction.
	/// \return AK_Success if the device was successfully destroyed.
	/// \warning This function is not thread-safe. No stream should exist for that device when it is destroyed.
	AKRESULT DestroyDevice(
		AkDeviceID					in_deviceID         ///< Device ID of the device to destroy.
		);

	/// Get the default values for the streaming device's settings. Recommended usage
	/// is to call this function first, then pass the settings to AK::StreamMgr::CreateDevice().
	/// \sa 
	/// - AK::StreamMgr::CreateDevice()
	/// - AkDeviceSettings
	/// - \ref streamingmanager_settings
	void GetDefaultDeviceSettings(
		AkDeviceSettings& out_settings		///< Returned AkDeviceSettings structure with default values.
		);
	//@}

	/// \name Language management.
	//@{
	/// Set the current language once and only once, here. The language name is stored in a static buffer 
	/// inside the Stream Manager. In order to resolve localized (language-specific) file location,
	/// AK::StreamMgr::IAkFileLocationResolver implementations query this string. They may use it to 
	/// construct a file path (for e.g. SDK/samples/SoundEngine/Common/AkFileLocationBase.cpp), or to
	/// find a language-specific file within a look-up table (for e.g. SDK/samples/SoundEngine/Common/AkFilePackageLUT.cpp).
	/// Pass a valid null-terminated string, without a trailing slash or backslash. Empty strings are accepted.
	/// You may register for language changes (see RegisterToLanguageChangeNotification()). 
	/// After changing the current language, all observers are notified.
	/// \return AK_Success if successful (if language string has less than AK_MAX_LANGUAGE_NAME_SIZE characters). AK_Fail otherwise.
	/// \warning Not multithread safe.
	/// \sa 
	/// - AK::StreamMgr::GetCurrentLanguage()
	/// - AK::StreamMgr::AddLanguageChangeObserver()
	AKRESULT SetCurrentLanguage(
		const AkOSChar* in_pszLanguageName			///< Language name.
		);

	/// Get the current language. The language name is stored in a static buffer inside the Stream Manager, 
	/// with AK::StreamMgr::SetCurrentLanguage(). In order to resolve localized (language-specific) file location,
	/// AK::StreamMgr::IAkFileLocationResolver implementations query this string. They may use it to 
	/// construct a file path (for e.g. SDK/samples/SoundEngine/Common/AkFileLocationBase.cpp), or to
	/// find a language-specific file within a look-up table (for e.g. SDK/samples/SoundEngine/Common/AkFilePackageLUT.cpp).
	/// \return Current language.
	/// \sa AK::StreamMgr::SetCurrentLanguage()
	const AkOSChar* GetCurrentLanguage();

	/// Register to language change notifications.
	/// \return AK_Success if successful, AK_Fail otherwise (no memory or no cookie).
	/// \warning Not multithread safe.
	/// \sa 
	/// - AK::StreamMgr::SetCurrentLanguage()
	/// - AK::StreamMgr::RemoveLanguageChangeObserver()
	AKRESULT AddLanguageChangeObserver(
		AK::StreamMgr::AkLanguageChangeHandler in_handler,	///< Callback function.
		void* in_pCookie					///< Cookie, passed back to AkLanguageChangeHandler. Must set.
		);

	/// Unregister to language change notifications. Use the cookie you have passed to 
	/// AddLanguageChangeObserver() to identify the observer.
	/// \warning Not multithread safe.
	/// \sa 
	/// - AK::StreamMgr::SetCurrentLanguage()
	/// - AK::StreamMgr::AddLanguageChangeObserver()
	void RemoveLanguageChangeObserver(
		void* in_pCookie					///< Cookie that was passed to AddLanguageChangeObserver().
		);

	/// \name Stream Manager: Cache management.
	//@{
	/// Flush cache of all devices. This function has no effect for devices where
	/// AkDeviceSettings::bUseStreamCache was set to false (no caching).
	/// \sa
	/// - \ref streamingmanager_settings
	void FlushAllCaches();

	//@}
};
