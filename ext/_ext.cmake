# sourcepp
set(SOURCEPP_LIBS_START_ENABLED OFF CACHE INTERNAL "" FORCE)
set(SOURCEPP_USE_VTFPP          ON  CACHE INTERNAL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/sourcepp")

# Qt
find_package(Qt6 REQUIRED COMPONENTS Gui)

# ECM
if(QVTFPP_INSTALL_PLUGIN)
	find_package(ECM REQUIRED 6.11.0 NO_MODULE)
	list(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}" "${ECM_KDE_MODULE_DIR}")
	include(KDECompilerSettings)
	include(KDEInstallDirs)
endif()
