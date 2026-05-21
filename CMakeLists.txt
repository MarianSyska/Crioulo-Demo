cmake_minimum_required(VERSION 4.2)
project(CriouloDemo)

add_subdirectory("lib/Crioulo")

# Configure Assimp Libary
file(GLOB_RECURSE ASSIMP_HEADER_FILES CONFIGURE_DEPENDS "include/assimp/*.h" "include/assimp/*.hpp")

add_library(assimp SHARED IMPORTED)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/include/assimp/config.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/assimp/config.h"
)

set_target_properties(assimp PROPERTIES
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-vc145-mtd.lib"
    
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/assimp-vc145-mtd.dll"
)

target_sources(assimp 
	INTERFACE 
		FILE_SET HEADERS 
		BASE_DIRS 
			"include"
		FILES 
			${ASSIMP_HEADER_FILES}
			"include/assimp/config.h"
)

# Configure Example Executables

# Define Common Libs
set(COMMON_LIBS
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib" 
	Crioulo
	assimp
)

# Define Common Source Files
file(GLOB_RECURSE COMMON_SRC_FILES CONFIGURE_DEPENDS "src/common/*.cpp")

# Define Common Header Files
file(GLOB_RECURSE STB_IMAGE_HEADER_FILES CONFIGURE_DEPENDS "include/stb_image/*.h" "include/stb_image/*.hpp")
file(GLOB_RECURSE GLFW_HEADER_FILES CONFIGURE_DEPENDS "include/GLFW/*.h" "include/GLFW/*.hpp")
file(GLOB_RECURSE COMMON_HEADER_FILES CONFIGURE_DEPENDS "include/CriouloDemo/common/*.h" "include/CriouloDemo/common/*.hpp")
file(GLOB_RECURSE UTIL_HEADER_FILES CONFIGURE_DEPENDS "include/CriouloDemo/util/*.h" "include/CriouloDemo/util/*.hpp")

# Define Common Shader Filess
file(GLOB_RECURSE COMMON_SHADER_FILES CONFIGURE_DEPENDS
    "shaders/common/*.glsl" 
    "shaders/common/*.vert" 
    "shaders/common/*.frag"
)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configuration/root_directory.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/configuration/root_directory.h"
)

function(configure_example TARGET_NAME FOLDER_NAME EXTRA_HEADER_FILES EXTRA_LIBS)

	file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/examples/${FOLDER_NAME}/*.cpp")

	file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS 
		"include/examples/${FOLDER_NAME}/*.h" 
		"include/examples/${FOLDER_NAME}/*.hpp"
	)

	file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS
		"shaders/examples/${FOLDER_NAME}/*.glsl" 
		"shaders/examples/${FOLDER_NAME}/*.vert" 
		"shaders/examples/${FOLDER_NAME}/*.frag"
	)

	add_executable(${TARGET_NAME})

	target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)
	if (MSVC)
		target_compile_options(${TARGET_NAME} PRIVATE /W4)
	endif()
	source_group("shaders" 
		FILES 
			${COMMON_SHADER_FILES}
			${SHADER_FILES}
	)

	set_source_files_properties(
		${COMMON_SHADER_FILES}
		${SHADER_FILES}
		PROPERTIES 
			HEADER_FILE_ONLY TRUE
	)

	target_sources(${TARGET_NAME}
		PRIVATE
			${COMMON_SRC_FILES}
			${SRC_FILES}
			${COMMON_SHADER_FILES}
			${SHADER_FILES}

		PRIVATE

			FILE_SET HEADERS
			BASE_DIRS
				"include"
			FILES
				${GLFW_HEADER_FILES}
				${STB_IMAGE_HEADER_FILES}
				${COMMON_HEADER_FILES}
				${UTIL_HEADER_FILES}
				${EXTRA_HEADER_FILES}
				${HEADER_FILES}
		
			FILE_SET configurationHeaders
			TYPE HEADERS
			BASE_DIRS
				"configuration"
			FILES
				"configuration/root_directory.h"
	)

	target_link_libraries(${TARGET_NAME} PRIVATE ${COMMON_LIBS} ${EXTRA_LIBS})

	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:assimp>
        "${CMAKE_CURRENT_SOURCE_DIR}/bin/assimp-vc145-mtd.pdb"
        $<TARGET_FILE_DIR:${TARGET_NAME}>
    COMMENT "Copying Assimp binaries to ${TARGET_NAME} directory..."
)

endfunction()

configure_example(01ExampleSimpleSquad 01_simple_squad "" "")
configure_example(02ExampleModelLoading 02_model_loading "" "")