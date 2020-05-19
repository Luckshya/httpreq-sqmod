// ------------------------------------------------------------------------------------------------
#include "Common.hpp"
#include "CRequest.h"
#include "CResponse.h"
#include "Misc.h"
#include "Events.h"

// ------------------------------------------------------------------------------------------------
using namespace SqHTTP;
using namespace Sqrat;

// ------------------------------------------------------------------------------------------------
std::unique_ptr< EventManager > g_EventManager;

// ------------------------------------------------------------------------------------------------
namespace SqHTTP
{
typedef const char * CCStr;

/* ------------------------------------------------------------------------------------------------
 * Register the module API under the obtained virtual machine.
*/
static bool RegisterAPI(HSQUIRRELVM vm)
{
	// Make sure there's a valid virtual machine before proceeding
	if (!vm)
	{
		OutputError("%s: Cannot register API without a valid virtual machine", SQHTTP_NAME);
		// Registration failed
		return false;
	}

	Sqrat::Table HTTPcn(vm);

	Sqrat::RootTable(vm).Bind("SqHTTP", HTTPcn);
	GetRequest::Register(HTTPcn);

	// Registration was successful
	return true;
}

/* ------------------------------------------------------------------------------------------------
 * Load the module on the virtual machine provided by the host module.
*/
static bool OnSquirrelLoad()
{
	// Make sure that we have a valid module API
	if (!SqMod_GetSquirrelVM)
	{
		OutputError("%s: Cannot obtain the Squirrel virtual machine without the module API", SQHTTP_NAME);
		// Unable to proceed!
		return false;
	}
	// Obtain the Squirrel virtual machine from the host plug-in
	DefaultVM::Set(SqMod_GetSquirrelVM());
	// Make sure that a valid virtual machine exists
	if (!DefaultVM::Get())
	{
		OutputError("%s: Squirrel virtual machine obtained from the host plug-in is invalid", SQHTTP_NAME);
		// Unable to proceed!
		return false;
	}

	// Create and publish our event
	g_EventManager = std::unique_ptr< EventManager >(new EventManager());

	g_EventManager->CreateSQEvent();
	g_EventManager->PublishSQEvent();

	// Register the module API
	if (RegisterAPI(DefaultVM::Get()))
	{
		OutputMessage("Registered: %s", SQHTTP_NAME);
	}
	else
	{
		return false;
	}
	// At this point, the module was successfully loaded
	return true;
}

/* ------------------------------------------------------------------------------------------------
 * The virtual machine is about to be terminated and script resources should be released.
*/
static void OnSquirrelTerminate()
{
	OutputMessage("Terminating: %s", SQHTTP_NAME);

	// Release script resources...
	clearFuture();
	Response::Clear();

	g_EventManager.reset();
	g_EventManager = nullptr;
}

/* ------------------------------------------------------------------------------------------------
 * The virtual machined is about to be closed. Last chance to release anything manually.
*/
static void OnSquirrelClosing()
{
}

/* ------------------------------------------------------------------------------------------------
 * The virtual machined was closed and all memory associated with it was released.
*/
static void OnSquirrelReleased()
{
	// Release the current virtual machine, if any
	DefaultVM::Set(nullptr);
}

/* ------------------------------------------------------------------------------------------------
 * React to command sent by other plug-ins.
*/
static uint8_t OnPluginCommand(uint32_t command_identifier, CCStr message)
{
	switch (command_identifier)
	{
	case SQMOD_INITIALIZE_CMD:
	{
		if (CheckModuleAPIVer(message, SQHTTP_NAME))
		{
			try
			{
				ImportModuleAPI(_Func, SQHTTP_NAME);
			}
			catch (const Sqrat::Exception & e)
			{
				OutputError("%s", e.what());
				// Failed to initialize
				return 0;
			}
		}
	} break;
	case SQMOD_LOAD_CMD:
	{
		return OnSquirrelLoad();
	} break;
	case SQMOD_TERMINATE_CMD:
	{
		OnSquirrelTerminate();
	} break;
	case SQMOD_CLOSING_CMD:
	{
		OnSquirrelClosing();
	} break;
	case SQMOD_RELEASED_CMD:
	{
		OnSquirrelReleased();
	} break;
	default: break;
	}
	return 1;
}

/* ------------------------------------------------------------------------------------------------
 * The server was initialized and this plug-in was loaded successfully.
*/
static uint8_t OnServerInitialise()
{
	return 1; // Initialization was successful
}

/* ------------------------------------------------------------------------------------------------
 * The server is about to shutdown gracefully.
*/
static void OnServerShutdown(void)
{
	// The server may still send callbacks
	_Clbk->OnServerInitialise = nullptr;
	_Clbk->OnServerShutdown = nullptr;
	_Clbk->OnPluginCommand = nullptr;
	_Clbk->OnServerFrame = nullptr;
}

static void OnServerFrame(float)
{
	Response::Update();
}
} // Namespace:: SqMod

// ------------------------------------------------------------------------------------------------
SQMOD_API_EXPORT unsigned int VcmpPluginInit(PluginFuncs * functions, PluginCallbacks * callbacks, PluginInfo * info)
{
	using namespace SqHTTP;

	// Output plug-in header
	puts("");
	OutputMessage("--------------------------------------------------------------------");
	OutputMessage("Plug-in: %s", SQHTTP_NAME);
	OutputMessage("Author: %s", SQHTTP_AUTHOR);
	OutputMessage("Legal: %s", SQHTTP_COPYRIGHT);
	OutputMessage("--------------------------------------------------------------------");
	puts("");
	// Make sure that the module was loaded after the host plug-in
	if (!CheckModuleOrder(functions, info->pluginId, SQHTTP_NAME))
	{
		return SQMOD_FAILURE;
	}
	// Store server proxies
	_Func = functions;
	_Clbk = callbacks;
	_Info = info;
	// Assign plug-in version
	_Info->pluginVersion = SQHTTP_VERSION;
	_Info->apiMajorVersion = PLUGIN_API_MAJOR;
	_Info->apiMinorVersion = PLUGIN_API_MINOR;
	// Assign the plug-in name
	std::snprintf(_Info->name, sizeof(_Info->name), "%s", SQHTTP_HOST_NAME);
	// Bind to the server callbacks
	_Clbk->OnServerInitialise = OnServerInitialise;
	_Clbk->OnServerShutdown = OnServerShutdown;
	_Clbk->OnPluginCommand = OnPluginCommand;
	_Clbk->OnServerFrame = OnServerFrame;
	// Notify that the plug-in was successfully loaded
	OutputMessage("Successfully loaded %s", SQHTTP_NAME);
	// Dummy spacing
	puts("");
	// Done!
	return SQMOD_SUCCESS;
}
