add_library(SqHTTP SHARED
		Common.hpp
		Common.cpp
		Module.cpp
		CRequest.h
		CRequest.cpp
		CResponse.h
		CResponse.cpp
		Events.h
		Events.cpp
		includes.h
		Misc.h
		Misc.cpp
		)

# Force 32-bit binaries when necessary
if(FORCE_32BIT_BIN)
	set_target_properties(SqHTTP PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	message("FORCING 32 BIT BINARIES")
endif()

# Size of a pointer in bytes. To identify CPU architecture.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	message("DEFAULTING TO 64 BIT BINARIES")
endif()

# Compile definitions
target_compile_definitions(SqHTTP PUBLIC SQMOD_PLUGIN_API=1)

# Determine if build mode
if(CMAKE_BUILD_TYPE MATCHES Release)
	target_compile_definitions(SqHTTP PRIVATE NDEBUG=1)
else()
	target_compile_definitions(SqHTTP PRIVATE _DEBUG=1 SQMOD_EXCEPTLOC=1)
endif()

target_compile_definitions(${CPR_LIBRARIES} PUBLIC CURL_STATICLIB)

# Link to base libraries
target_link_libraries(SqHTTP VCMP SquirrelAPI Sqrat SqSDK ${CPR_LIBRARIES})

# Don't prefix the module binary.
set_target_properties(SqHTTP PROPERTIES PREFIX "")

# Custmize module binary name/
if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT FORCE_32BIT_BIN)
		set_target_properties(SqHTTP PROPERTIES OUTPUT_NAME "mod_httprequest_64")
	else()
		set_target_properties(SqHTTP PROPERTIES OUTPUT_NAME "mod_httprequest_32")
	endif()
else(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT FORCE_32BIT_BIN)
		set_target_properties(SqHTTP PROPERTIES OUTPUT_NAME "mod_httprequest_64")
	else()
		set_target_properties(SqHTTP PROPERTIES OUTPUT_NAME "mod_httprequest_32")
	endif()
endif(WIN32)

target_include_directories(SqHTTP
	PRIVATE
		${PROJECT_SOURCE_DIR}/src
		${CPR_INCLUDE_DIRS}
)