#include "CResponse.h"
#include "Events.h"

// ------------------------------------------------------------------------------------------------
namespace SqHTTP {

std::vector<Response> m_Responses;
std::mutex m_ResponseGuard;

// ------------------------------------------------------------------------------------------------
Response::Response(std::string regTag, std::string url, std::string text, unsigned int statusCode) {
	this->regTag = regTag;
	this->url = url;
	this->text = text;
	this->statusCode = statusCode;
}

// ------------------------------------------------------------------------------------------------
void Response::Update() {
	if (!m_Responses.empty()) {
		std::lock_guard<std::mutex> lock(m_ResponseGuard);

		for (auto it = m_Responses.begin(); it != m_Responses.end(); ++it) {
			g_EventManager->CallSQEvent(it->regTag, it->url, it->text, it->statusCode);
		}

		m_Responses.clear();

		// Refresh future to remove the elements that are ready ?
		refreshFutureHolder();
	}
}

// ------------------------------------------------------------------------------------------------
void Response::Clear() {
	if (!m_Responses.empty()) {
		m_Responses.clear();
	}
}
} // Namespace - SqHTTP