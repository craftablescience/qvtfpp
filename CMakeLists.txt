cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# set up project
project(qvtfpp
		DESCRIPTION "A Qt6 QImageIO plugin to load VTF textures."
		VERSION "1.0.0"
		HOMEPAGE_URL "https://github.com/craftablescience/qvtfpp")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# options
option(QVTFPP_INSTALL_PLUGIN "Create install rules for ${PROJECT_NAME}" ON)
if(APPLE OR NOT UNIX)
	# Linux-specific sorry
	set(QVTFPP_INSTALL_PLUGIN OFF CACHE INTERNAL "" FORCE)
endif()

# add helpers
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/helpers")
include(CS_All)

# extra metadata variables
set(PROJECT_NAME_PRETTY "qvtf++")
cs_version_pretty() # PROJECT_VERSION_PRETTY
set(PROJECT_ORGANIZATION_NAME "craftablescience" CACHE INTERNAL "" FORCE)

# initial VS integration
cs_setup_vs_defaults()

# set common compile options
option(QVTFPP_USE_LTO "Compile ${PROJECT_NAME_PRETTY} with LTO/IPO" OFF)
cs_configure_defaults(QVTFPP_USE_LTO)

# shared code
cs_include_directory(ext)

# qvtfpp
qt_add_plugin(${PROJECT_NAME} PLUGIN_TYPE imageformats)
target_sources(${PROJECT_NAME} PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/src/QVTFPlugin.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/QVTFPlugin.h")
set_target_properties(qvtfpp PROPERTIES AUTOMOC ON)
target_link_libraries(${PROJECT_NAME} Qt6::Gui sourcepp::vtfpp)
target_compile_options(${PROJECT_NAME} PUBLIC "-fexceptions")

# install rules
if(QVTFPP_INSTALL_PLUGIN)
	cs_include_directory(install)
endif()
