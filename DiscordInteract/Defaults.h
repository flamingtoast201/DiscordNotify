//==============================================================================================
// Include Only Once
//==============================================================================================
#pragma once
//==============================================================================================
// Libraries To Include
//==============================================================================================
#include <string>
#include <cstdlib>
#include <iostream>
#include "secrets.h"
//==============================================================================================
// Define Namespace Defaults Header
//==============================================================================================
namespace Defaults {
    //=========================================================================================
	// Define Default Variables
    //=========================================================================================
    static const std::string primary_url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=" + Secrets::GEMINI_KEY;
    static const std::string fallback_url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash-lite:generateContent?key=" + Secrets::GEMINI_KEY;
	static const std::string GeminiProjectName = "Current Project: SkySmith(Work In Progress)\n";
	static const std::string GeminiPrompt = "Review this push to the repository and provide short immediate blunt criticism as if you were a scottish lady reviewing it if its a bad commit message that doesn't say much. If its a good commit message praise it. :\n";
    //=========================================================================================
	// Public Function Declarations
    //=========================================================================================
    std::string const ProjectName();
    std::string const GeminiPromptText();
    std::string get_gemini_key();
    std::string get_discord_webhook();
    //=========================================================================================
}  
//==============================================================================================
// End of Define Namespace Defaults Header
//==============================================================================================