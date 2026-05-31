//==============================================================================================
// Libraries To Include
//==============================================================================================
#include "Defaults.h"
//==============================================================================================
// Namespace Defaults CPP
//==============================================================================================
// Function: Project Name
//----------------------------------------------------------------------------------------------
std::string const Defaults::ProjectName() {
    return GeminiProjectName;
}
//==============================================================================================
// Function: Gemini Prompt Text
//----------------------------------------------------------------------------------------------
std::string const Defaults::GeminiPromptText() {
    return GeminiPrompt;
}
//==============================================================================================
// Function: Get Gemini Key
//----------------------------------------------------------------------------------------------
std::string Defaults::get_gemini_key() {
    char* env_key = nullptr;
    size_t len = 0;
    if (_dupenv_s(&env_key, &len, "GEMINI_API_KEY") == 0 && env_key != nullptr) {
        std::string result(env_key);
        free(env_key);
        return result;
    }
    std::cerr << "[CRITICAL] Environment variable GEMINI_API_KEY not found!\n";
    return "";
}
//==============================================================================================
// Function: Get Discord Webhook
//----------------------------------------------------------------------------------------------
std::string Defaults::get_discord_webhook() {
    char* env_url = nullptr;
    size_t len = 0;
    if (_dupenv_s(&env_url, &len, "DISCORD_WEBHOOK_URL") == 0 && env_url != nullptr) {
        std::string result(env_url);
        free(env_url); 
        return result;
    }
    std::cerr << "[CRITICAL] Environment variable DISCORD_WEBHOOK_URL not found!\n";
    return "";
}
//==============================================================================================
// End of Namespace Defaults CPP
//==============================================================================================