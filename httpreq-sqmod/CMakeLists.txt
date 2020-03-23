add_library(mod_httprequest SHARED
	Common.hpp
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
	${PROJECT_SOURCE_DIR}/shared/Base/Buffer.cpp
	${PROJECT_SOURCE_DIR}/shared/SqMod.cpp
)

if(BUILD_64)
	target_compile_options(mod_httprequest PRIVATE -D_SQ64 -DSQMOD_PLUGIN_API -DSCRAT_USE_EXCEPTIONS -DSCRAT_USE_CXX11_OPTIMIZATIONS)
else()
	target_compile_options(mod_httprequest PRIVATE -DSQMOD_PLUGIN_API -DSCRAT_USE_EXCEPTIONS -DSCRAT_USE_CXX11_OPTIMIZATIONS)
endif()

target_include_directories(mod_httprequest
	PRIVATE
		${PROJECT_SOURCE_DIR}/httpreq-sqmod
		${PROJECT_SOURCE_DIR}/shared
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/config/common
		${PROJECT_SOURCE_DIR}/shared/Base
		${CPR_INCLUDE_DIRS}
)

if(CURL_STATICLIB)
	target_compile_definitions(${CPR_LIBRARIES} PUBLIC CURL_STATICLIB)
endif()
		
target_link_libraries(mod_httprequest ${CPR_LIBRARIES})