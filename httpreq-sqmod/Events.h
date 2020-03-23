#pragma once

#include "Common.hpp"

// --------------------------------------------------------------------------------------------
namespace SqHTTP
{
	struct EventManager {
		HSQOBJECT signalObj;
		void CreateSQEvent();
		void PublishSQEvent();
		void CallSQEvent(std::string, std::string, std::string, unsigned int);
		void ParseError(HSQUIRRELVM);

		~EventManager();
	};
	
} // Namespace - SqHTTP