// ------------------------------------------------------------------------------------------------
#include "Common.hpp"
#include "Misc.h"

// ------------------------------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

// ------------------------------------------------------------------------------------------------
// Mutex guard for future container
std::mutex m_futureGuard;

// Container to hold futures
std::vector<std::future<bool>> future_holder;

// ------------------------------------------------------------------------------------------------
namespace SqHTTP {

// ------------------------------------------------------------------------------------------------
void refreshFutureHolder() {

	if (!future_holder.empty())
	{
		//SqMod_LogDbg("[FUTURE HOLDER] Before clearing: %s", std::to_string(future_holder.size()));

		{
			std::lock_guard<std::mutex> lock(m_futureGuard);
			future_holder.erase(
				std::remove_if(future_holder.begin(), future_holder.end(), [](std::future<bool> & fr) {
				return fr.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
			}),
			future_holder.end()
			);
		}

		//SqMod_LogDbg("[FUTURE HOLDER] After clearing: %s", std::to_string(future_holder.size()));
	}
}

// ------------------------------------------------------------------------------------------------
void clearFuture() {
	if (!future_holder.empty()) {
		std::lock_guard<std::mutex> lock(m_futureGuard);

		// Remember this makes the current thread wait for the request to get completed first
		future_holder.clear();
	}
}
} // Namespace - SqHTTP