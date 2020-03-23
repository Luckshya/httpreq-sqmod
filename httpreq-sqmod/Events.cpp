#include "Events.h"

// --------------------------------------------------------------------------------------------
using namespace Sqrat;
using namespace SqMod;

// --------------------------------------------------------------------------------------------
namespace SqHTTP
{	
// --------------------------------------------------------------------------------------------
void EventManager::CreateSQEvent()
{
	try {
		HSQUIRRELVM vm = DefaultVM::Get();
		const SQInteger top = sq_gettop(vm);

		// Push root table
		sq_pushroottable(vm);

		// Push function name
		sq_pushstring(vm, "SqCreateSignal", -1);

		// Get function from root table with function name
		if (SQ_FAILED(sq_get(vm, -2)))
		{
			// Set back the top and pop all above it
			sq_settop(vm, top);

			return ParseError(vm);
		}

		// Push the environment
		sq_pushroottable(vm);

		// Push the parameter (signal name)
		sq_pushstring(vm, "HTTPResponse", -1);

		// Call the SqCreateSignal function
		// Returns signal object
		if (SQ_FAILED(sq_call(vm, 2, SQTrue, SQTrue)))
		{
			// Set back the top and pop all above it
			sq_settop(vm, top);

			return ParseError(vm);
		}

		// Store the returned signal object by the function
		if (SQ_FAILED(sq_getstackobj(vm, -1, &signalObj)))
		{
			// Set back the top and pop all above it
			sq_settop(vm, top);

			return ParseError(vm);
		}

		// Add a reference to the object
		sq_addref(vm, &signalObj);

		// Pop the roottable, function and the returned object
		sq_pop(vm, 3);
	}
	catch (const Sqrat::Exception & e)
	{
		SqMod_LogErr("EventManager [%s] => Squirrel error [%s]", "CreateSQEvent", e.what());
	}
	catch (const std::exception & e)
	{
		SqMod_LogErr("EventManager [%s] => Program error [%s]", "CreateSQEvent", e.what());
	}
	catch (...)
	{
		SqMod_LogErr("EventManager [%s] => Unknown error", "CreateSQEvent");
	}
}

// --------------------------------------------------------------------------------------------
// Store our event in SqCore.On().HTTPResponse
void EventManager::PublishSQEvent()
{
	try {
		HSQUIRRELVM vm = DefaultVM::Get();
		const SQInteger top = sq_gettop(vm);

		// Push the root table
		sq_pushroottable(vm);

		// Push the table name
		sq_pushstring(vm, "SqCore", -1);

		// Get the SqCore table from root table
		if (SQ_FAILED(sq_get(vm, -2)))
		{
			// Set back the top and pop all above it
			sq_settop(vm, top);

			return ParseError(vm);
		}

		// Push the function name
		sq_pushstring(vm, "On", -1);

		// Get the function from SqCore table with function name
		if (SQ_FAILED(sq_get(vm, -2)))
		{
			// Set back the top and pop all above it
			sq_settop(vm, top);

			return ParseError(vm);
		}

		// Push the environment
		sq_pushroottable(vm);

		// Call the SqCore.On() function
		// Returns events table
		if (SQ_FAILED(sq_call(vm, 1, SQTrue, SQTrue)))
		{
			// Set back the top and pop all above it
			sq_settop(vm, top);

			return ParseError(vm);
		}

		// Push the event name
		sq_pushstring(vm, "HTTPResponse", -1);

		// Push the signal to bind with event
		sq_pushobject(vm, signalObj);

		// Create event
		if (SQ_FAILED(sq_newslot(vm, -3, SQFalse)))
		{
			// Set back the top and pop all above it
			sq_settop(vm, top);

			return ParseError(vm);
		}

		// Pop roottable, SqCore table, SqCore.On function, returned table
		sq_pop(vm, 4);
	}
	catch (const Sqrat::Exception & e)
	{
		SqMod_LogErr("EventManager [%s] => Squirrel error [%s]", "PublishSQEvent", e.what());
	}
	catch (const std::exception & e)
	{
		SqMod_LogErr("EventManager [%s] => Program error [%s]", "PublishSQEvent", e.what());
	}
	catch (...)
	{
		SqMod_LogErr("EventManager [%s] => Unknown error", "PublishSQEvent");
	}
}

// --------------------------------------------------------------------------------------------
// Whenever we want to emit that signal
void EventManager::CallSQEvent(std::string reqTag, std::string url, std::string text, unsigned int statusCode)
{
	try {
		HSQUIRRELVM vm = DefaultVM::Get();
		const SQInteger top = sq_gettop(vm);

		// Push signal object
		sq_pushobject(vm, signalObj);

		// Push function name
		sq_pushstring(vm, "Emit", -1);

		// Get Emit function from pushed object with function name
		if (SQ_FAILED(sq_get(vm, -2)))
		{
			// Set back the top and pop all above it
			sq_settop(vm, top);

			return ParseError(vm);
		}

		// Push environment
		sq_pushobject(vm, signalObj);

		// Push parameters
		sq_pushstring(vm, reqTag.c_str(), -1);
		sq_pushstring(vm, url.c_str(), -1);
		sq_pushinteger(vm, statusCode);
		sq_pushstring(vm, text.c_str(), -1);

		// Call the Emit function
		if (SQ_FAILED(sq_call(vm, 5, SQFalse, SQTrue)))
		{
			// Set back the top and pop all above it
			sq_settop(vm, top);

			return ParseError(vm);
		}

		// Pop the signal object, Emit function
		sq_pop(vm, 2);
	}
	catch (const Sqrat::Exception & e)
	{
		SqMod_LogErr("EventManager [%s] => Squirrel error [%s]", "CallSQEvent", e.what());
	}
	catch (const std::exception & e)
	{
		SqMod_LogErr("EventManager [%s] => Program error [%s]", "CallSQEvent", e.what());
	}
	catch (...)
	{
		SqMod_LogErr("EventManager [%s] => Unknown error", "CallSQEvent");
	}
}

// --------------------------------------------------------------------------------------------
void EventManager::ParseError(HSQUIRRELVM vm)
{
	// Get the last error
	sq_getlasterror(vm);

	CCStr error = nullptr;

	// Put it in the variable
	sq_getstring(vm, -1, &error);

	// throw exception
	SQTHROW(vm, error);
}

// --------------------------------------------------------------------------------------------
EventManager::~EventManager()
{
	// Release the reference to the object that we added in createSQEvent function
	if (!sq_isnull(signalObj))
	{
		// Only release if reference count > 1
		if (sq_getrefcount(DefaultVM::Get(), &signalObj) > 1) {
			sq_release(DefaultVM::Get(), &signalObj);
		}

		// Reset the object
		sq_resetobject(&signalObj);
	}
}
} // Namespace - SqHTTP