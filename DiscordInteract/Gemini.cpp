//==============================================================================================
// Libraries To Include
//==============================================================================================
#include "Gemini.h"
//==============================================================================================
// Function Definitions for ClassGemini CPP
//==============================================================================================
// Function: get_criticism
//----------------------------------------------------------------------------------------------
std::string ClassGemini::get_criticism(const std::string& repo, const std::string& changes) {
    std::string prompt = Defaults::ProjectName() + repo + Defaults::GeminiPromptText() + changes;
    auto response = cpr::Post(
        cpr::Url{ Defaults::GEMINI_URL + "?key=" + Defaults::get_gemini_key() },
        cpr::Header{ {"Content-Type", "application/json"} },
        // FIX: Adding an extra outer brace layer forces nlohmann::json to output standard arrays [ ]
        cpr::Body{ json{{"contents", { {{ "parts", {{ {"text", prompt} }} }} }}} .dump() }
    );
    try {
        auto res_json = json::parse(response.text);
        return res_json["candidates"][0]["content"]["parts"][0]["text"].get<std::string>();
    }
    catch (...) {
        return "Error: Failed to parse or extract Gemini response payload.";
    }
}
//==============================================================================================
// End of Gemini CPP
//==============================================================================================