#pragma once

// ------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <mutex>
#include <future>

// ------------------------------------------------------------------------------------------------
#include "Events.h"
#include "Misc.h"

// ------------------------------------------------------------------------------------------------
// Container to hold futures
extern std::vector<std::future<bool>> future_holder;

// ------------------------------------------------------------------------------------------------
extern std::unique_ptr< SqHTTP::EventManager > g_EventManager;


// ------------------------------------------------------------------------------------------------
namespace SqHTTP {
class Response {
public :
	std::string regTag;
	std::string url;
	std::string text;
	unsigned int statusCode;

	Response(std::string, std::string, std::string, unsigned int);

	static void Update();
	static void Clear();
};
} // Namespace - SqHTTP