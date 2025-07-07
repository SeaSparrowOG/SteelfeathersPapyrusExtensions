#include "Data/ModObjectManager.h"
#include "Hooks/Hooks.h"
#include "Papyrus/Papyrus.h"
#include "Serialization/Serde.h"
#include "Settings/INI/INISettings.h"
#include "Settings/JSON/JSONSettings.h"

static void MessageEventCallback(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		//if (!Settings::JSON::Read()) {
		//	SKSE::stl::report_and_fail("Failed to load JSON settings. Check the log for more information."sv);
		//}
		//SECTION_SEPARATOR;
		//if (!Data::PreloadModObjects()) {
		//	SKSE::stl::report_and_fail("Failed to preload mod objects. Check the log for more information."sv);
		//}
		SECTION_SEPARATOR;
		logger::info("Finished startup tasks, enjoy your game!"sv);
		break;
	default:
		break;
	}
}

#ifdef SKYRIM_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
	{
		SKSE::PluginVersionData v{};

		v.PluginVersion(Plugin::VERSION);
		v.PluginName(Plugin::NAME);
		v.AuthorName("Steelfeathers"sv);
		v.UsesAddressLibrary();
		v.UsesUpdatedStructs();

		return v;
	}();
#endif

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
#ifdef SKYRIM_AE
	if (ver < SKSE::RUNTIME_1_6_1130) {
#else
	if (ver < SKSE::RUNTIME_1_5_39) {
#endif
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);

	SECTION_SEPARATOR;
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());
	logger::info("Author: Steelfeathers"sv);
	SECTION_SEPARATOR;

#ifdef SKYRIM_AE
	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_6_1130) {
		return false;
	}
#endif

	logger::info("Performing startup tasks..."sv);

	// if (!Settings::INI::Read()) {
	//	SKSE::stl::report_and_fail("Failed to load INI settings. Check the log for more information."sv);
	//}
	SECTION_SEPARATOR;
	if (!Hooks::Install()) {
		SKSE::stl::report_and_fail("Failed to install hooks. Check the log for more information."sv);
	}
	SECTION_SEPARATOR;

	SKSE::GetPapyrusInterface()->Register(Papyrus::RegisterFunctions);
	SECTION_SEPARATOR;

	const auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(&MessageEventCallback);

	logger::info("Setting up serialization system..."sv);
	const auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID(Serialization::ID);
	serialization->SetSaveCallback(&Serialization::SaveCallback);
	serialization->SetLoadCallback(&Serialization::LoadCallback);
	serialization->SetRevertCallback(&Serialization::RevertCallback);
	logger::info("  >Registered necessary functions."sv);
	SECTION_SEPARATOR;


	return true;
}

void GetFormByID() {
	auto* sweetroll = RE::TESForm::LookupByID(0x64B3D);
	logger::info("Form Name: {}", sweetroll->GetName());
}