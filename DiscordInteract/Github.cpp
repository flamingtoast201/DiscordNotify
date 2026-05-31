//==============================================================================================
// Libraries To Include
//==============================================================================================
#include "Github.h"
//==============================================================================================
// Github CPP
//==============================================================================================
// Function: parse_webhook
//----------------------------------------------------------------------------------------------
PushPayload ClassGitHub::parse_webhook(const std::string& raw_json) {
    PushPayload payload;
    try {
        auto data = json::parse(raw_json);
        payload.repo_name = data["repository"]["full_name"];

        for (const auto& commit : data["commits"]) {
            payload.commit_summary += "- " + commit["message"].get<std::string>() + "\n";
        }
        payload.is_valid = !payload.commit_summary.empty();
    }
    catch (...) {
        payload.is_valid = false;
    }
    return payload;
}
//==============================================================================================
// End of Github CPP
//==============================================================================================