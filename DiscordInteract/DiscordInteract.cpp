//==============================================================================================
// Libraries To Include
//==============================================================================================
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <string>
#include <iostream> 
#include <cstdlib> 
#include "httplib.h"
#include "cpr/cpr.h"
#include "nlohmann/json.hpp"
#include "Gemini.h"  // Bringing in your native Gemini class
#include "Defaults.h"
//==============================================================================================
// Forward Declaration
//==============================================================================================
using json = nlohmann::json;
//==============================================================================================
// Function Declarations
//----------------------------------------------------------------------------------------------
void ship_to_discord(const std::string& bot_username, const std::string& message);
//==============================================================================================
// Main Engine Loop:
//==============================================================================================
int main() {
    ClassGemini gemini;
    // Step A: Define the ntfy channel URL and initialize tracking for the last processed message ID
    std::string ntfy_url = "https://ntfy.sh/skysmith_dev_pipeline_2026/json?poll=1&since=latest";
    std::string last_processed_id = "";
    //==========================================================================================
    // Display The Banner and Start The Engine Loop
    //==========================================================================================
    std::cout << "======================================================\n";
    std::cout << "     CONDUIT ACTIVE: Stream-Parsing ntfy Receiver     \n";
    std::cout << "======================================================\n";
    std::cout << "Monitoring channel across the public internet...\n\n";
    //==========================================================================================
    // While True Execute Loop
    //==========================================================================================
    while (true) {
        // Step B: Fetch the data stream from the cloud box
        auto response = cpr::Get(cpr::Url{ ntfy_url });
        if (response.status_code == 200 && !response.text.empty()) {
            std::stringstream ss(response.text);
            std::string line;
            // Read line-by-line to safely parse ntfy's newline-delimited JSON format
            while (std::getline(ss, line)) {
                if (line.empty()) continue;
                try {
                    auto event_json = json::parse(line);

                    // Pull message timestamp and system time to calculate age
                    long long message_time = event_json.value("time", 0LL);
                    long long current_time = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();

                    // If the message is older than 10 seconds, skip it completely
                    if ((current_time - message_time) > 10) {
                        continue;
                    }

                    // Filter out system events (like "open" or "keep-alive" lines)
                    if (event_json.contains("event") && event_json["event"].get<std::string>() == "message") {
                        std::string current_id = event_json["id"].get<std::string>();
                        // Check if this message is fresh
                        if (current_id != last_processed_id) {
                            last_processed_id = current_id;
                            std::cout << "[EVENT DETECTED] New payload cleanly intercepted from stream!\n";
                            // Extract the inner string payload
                            std::string raw_payload = event_json["message"].get<std::string>();
                            auto incoming_data = json::parse(raw_payload);
                            std::string repo_name = "SkySmith";
                            std::string commit_msg = "Manual Test";
                            if (incoming_data.contains("repository") && incoming_data["repository"].contains("full_name")) {
                                repo_name = incoming_data["repository"]["full_name"].get<std::string>();
                            }
                            if (incoming_data.contains("message")) {
                                commit_msg = incoming_data["message"].get<std::string>();
                            }
                            // Step C: Execute Gemini Criticism
                            std::cout << "[PROCESSING] Sending data payload to Gemini API...\n";
                            std::string criticism = gemini.get_criticism(repo_name, commit_msg);
                            std::cout << "[CRITICISM RECEIVED]:\n" << criticism << "\n\n";
                            // Step D: Fire straight to Discord
                            ship_to_discord("SkySmith Bot", criticism);
                        }
                    }
                }
                catch (const std::exception& e) {
                    // Log out any unexpected formatting issues to screen so we see it
                    std::cout << "[PARSING DEBUG] Skipped non-message block: " << e.what() << "\n";
                }
                catch (...) {
                    // Catch-all to keep the engine loop immortal
                }
            }
        }
        // Sleep 4 seconds before pulling the mailbox again
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }
    return 0;
    //==========================================================================================
    // End of Main
    //==========================================================================================
}
//==============================================================================================
// Function ship_to_discord: Takes a bot username and message, formats it as JSON, and POSTs to the Discord webhook URL
//----------------------------------------------------------------------------------------------
void ship_to_discord(const std::string& bot_username, const std::string& message) {
    std::cout << "[" << bot_username << "] Shipping text to Discord channel...\n";
    std::string webhook_url = Defaults::get_discord_webhook();
    // Leverage nlohmann/json to guarantee structural integrity for Discord's payload
    json payload = {
        {"username", bot_username},
        {"content", message}
    };
    // Use cpr for a clean outbound POST step instead of forcing system commands
    auto response = cpr::Post(
        cpr::Url{ webhook_url },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ payload.dump() }
    );
    if (response.status_code == 200 || response.status_code == 204) {
        std::cout << "[SUCCESS] Payload cleanly delivered to Discord stack!\n\n";
    }
    else {
        std::cout << "[ERROR] Discord webhook reject. HTTP Code: " << response.status_code << "\n";
        std::cout << "Response text: " << response.text << "\n\n";
    }
}//==============================================================================================
// End of DiscordInteract.cpp
//==============================================================================================