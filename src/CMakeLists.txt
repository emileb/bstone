cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(bstone VERSION 1.2.10 LANGUAGES CXX)


#
# Version.
#
set(BSTONE_VERSION_CORE "${PROJECT_VERSION}")
set(BSTONE_VERSION_PRE_RELEASE "b.1")

if (BSTONE_VERSION_PRE_RELEASE STREQUAL "")
	set(BSTONE_VERSION "${BSTONE_VERSION_CORE}")
else ()
	set(BSTONE_VERSION "${BSTONE_VERSION_CORE}-${BSTONE_VERSION_PRE_RELEASE}")
endif ()

message(STATUS "[${PROJECT_NAME}] v${BSTONE_VERSION}")


#
# Options.
#
option(BSTONE_USE_STATIC_LINKING "Link build statically." OFF)
option(BSTONE_PANDORA "Build for Pandora." OFF)
option(BSTONE_USE_PCH "Use precompiled headers if available." ON)
option(BSTONE_USE_MULTI_PROCESS_COMPILATION "Use multi-process compilation." ON)


#
# Modules.
#
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# SDL2W
#
set(BSTONE_SDL2_COMPONENTS "")

if (BSTONE_USE_STATIC_LINKING)
	set(BSTONE_SDL2_COMPONENTS "${BSTONE_SDL2_COMPONENTS};static")
endif ()

find_package(SDL2W 2.0.4 REQUIRED COMPONENTS ${BSTONE_SDL2_COMPONENTS})

# Threads
#
set(THREADS_PREFER_PTHREAD_FLAG "TRUE")

find_package(Threads REQUIRED)


#
# Target.
#
add_executable(${PROJECT_NAME} "")


#
# Configure files.
#
configure_file(
	"${PROJECT_SOURCE_DIR}/bstone_version_.h.in"
	"${PROJECT_BINARY_DIR}/bstone_version_.h"
)

configure_file(
	"${PROJECT_SOURCE_DIR}/sys/win32/bstone_win32.rc.in"
	"${PROJECT_BINARY_DIR}/sys/win32/bstone_win32.rc"
)


#
# Target properties.
#
set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 14
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
)

if (WIN32)
	set_target_properties(
		${PROJECT_NAME}
		PROPERTIES
			WIN32_EXECUTABLE TRUE
	)
endif ()


#
# Target compile definitions.
#
if (MSVC)
	target_compile_definitions(
		${PROJECT_NAME}
		PRIVATE
			_CRT_SECURE_NO_WARNINGS
			_SCL_SECURE_NO_WARNINGS
			_BIND_TO_CURRENT_VCLIBS_VERSION=1
	)
endif ()

if (BSTONE_PANDORA)
	target_compile_definitions(
		${PROJECT_NAME}
		PRIVATE
			BSTONE_PANDORA
	)
endif ()


#
# Target compile options.
#
if (MSVC)
	if (BSTONE_USE_STATIC_LINKING)
		target_compile_options(
			${PROJECT_NAME}
			PRIVATE
				$<$<CONFIG:DEBUG>:-MTd>
				$<$<NOT:$<CONFIG:DEBUG>>:-MT>
		)
	endif ()

	if (BSTONE_USE_MULTI_PROCESS_COMPILATION)
		target_compile_options(
			${PROJECT_NAME}
			PRIVATE
				-MP
		)
	endif ()
endif ()

if (BSTONE_PANDORA)
	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
			-fsigned-char
			-fsigned-char
			-mcpu=cortex-a8
			-mfpu=neon
			-mfloat-abi=softfp
			-ffast-math
			-fsingle-precision-constant
			-fno-inline-functions
	)
endif ()


#
# Target link libraries.
#
if (BSTONE_USE_STATIC_LINKING AND (NOT WIN32 OR MINGW))
	target_link_libraries(
		${PROJECT_NAME}
		PRIVATE
			-static
	)
endif ()

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		SDL2W::SDL2Wmain
		SDL2W::SDL2W
		Threads::Threads
)

if (BSTONE_PANDORA)
	target_link_libraries(
		${PROJECT_NAME}
		PRIVATE
			Xext
			Xau
			Xdmcp
			X11
			GLESv2
			EGL
			IMGegl
			srv_um
			ts
	)
endif ()


#
# Target include directories.
#
target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		.
		dosbox
		lib/glm
		lib/xbrz
		sys/win32
		${PROJECT_BINARY_DIR}
)

if (BSTONE_PANDORA)
	target_include_directories(
		${PROJECT_NAME}
		PRIVATE
			$ENV{PNDSDK}
	)
endif ()


if (NOT (${CMAKE_VERSION} VERSION_LESS "3.16.0"))
	target_precompile_headers(
		${PROJECT_NAME}
		PRIVATE
			bstone_precompiled.h
	)
endif ()

#
# Target sources.
#
target_sources(
	${PROJECT_NAME}
	PRIVATE
		dosbox/adlib.h
		dosbox/dbopl.h
		dosbox/dosbox.h
		dosbox/hardware.h
		dosbox/inout.h
		dosbox/mixer.h
		dosbox/pic.h
		dosbox/setup.h

		bstone_extent_2d.h
		bstone_offset_2d.h
		bstone_rect_2d.h

		bstone_hw_shader_registry.h
		bstone_hw_texture_mgr.h
		bstone_missing_sprite_64x64_image.h
		bstone_missing_wall_64x64_image.h
		bstone_renderer_kind.h
		bstone_sdl2_exception.h
		bstone_sdl2_types.h

		bstone_ren_3d.h
		bstone_ren_3d_buffer.h
		bstone_ren_3d_cmd_buffer.h
		bstone_ren_3d_limits.h
		bstone_ren_3d_mgr.h
		bstone_ren_3d_sampler.h
		bstone_ren_3d_shader.h
		bstone_ren_3d_shader_stage.h
		bstone_ren_3d_shader_var.h
		bstone_ren_3d_tests.h
		bstone_ren_3d_texture_2d.h
		bstone_ren_3d_types.h
		bstone_ren_3d_vertex_input.h

		bstone_detail_ren_3d_utils.h

		bstone_detail_ren_3d_gl_api.h
		bstone_detail_ren_3d_gl_buffer.h
		bstone_detail_ren_3d_gl_context.h
		bstone_detail_ren_3d_gl_context_kind.h
		bstone_detail_ren_3d_gl_device_features.h
		bstone_detail_ren_3d_gl_error.h
		bstone_detail_ren_3d_gl_extension_mgr.h
		bstone_detail_ren_3d_gl.h
		bstone_detail_ren_3d_gl_utils.h
		bstone_detail_ren_3d_gl_sampler.h
		bstone_detail_ren_3d_gl_sampler_mgr.h
		bstone_detail_ren_3d_gl_shader.h
		bstone_detail_ren_3d_gl_shader_stage.h
		bstone_detail_ren_3d_gl_shader_stage_mgr.h
		bstone_detail_ren_3d_gl_shader_var.h
		bstone_detail_ren_3d_gl_texture_2d.h
		bstone_detail_ren_3d_gl_texture_mgr.h
		bstone_detail_ren_3d_gl_version.h
		bstone_detail_ren_3d_gl_vertex_input.h
		bstone_detail_ren_3d_gl_vertex_input_mgr.h

		3d_def.h
		3d_menu.h
		an_codes.h
		audio.h
		gfxv.h
		id_ca.h
		id_heads.h
		id_in.h
		id_pm.h
		id_sd.h
		id_us.h
		id_vh.h
		id_vl.h
		jm_cio.h
		jm_lzh.h
		jm_tp.h
		jm_vl.h
		movie.h

		bstone_adlib_decoder.h
		bstone_archiver.h
		bstone_atomic.h
		bstone_audio_decoder.h
		bstone_audio_mixer.h
		bstone_audio_sample_converter.h
		bstone_binary_reader.h
		bstone_binary_writer.h
		bstone_cl_args.h
		bstone_crc32.h
		bstone_endian.h
		bstone_encoding.h
		bstone_exception.h
		bstone_file_stream.h
		bstone_file_system.h
		bstone_fizzle_fx.h
		bstone_format_string.h
		bstone_generic_fizzle_fx.h
		bstone_content_path.h
		bstone_logger.h
		bstone_low_pass_filter.h
		bstone_math.h
		bstone_memory_binary_reader.h
		bstone_memory_stream.h
		bstone_mod_value.h
		bstone_mt_queue_1r1w.h
		bstone_mt_task_mgr.h
		bstone_opl3.h
		bstone_precompiled.h
		bstone_ps_fizzle_fx.h
		bstone_ref_values.h
		bstone_rgb8.h
		bstone_rgb_palette.h
		bstone_scope_guard.h
		bstone_sha1.h
		bstone_sprite.h
		bstone_sprite_cache.h
		bstone_stream.h
		bstone_string_helper.h
		bstone_text_reader.h
		bstone_text_writer.h
		bstone_un_value.h
		bstone_unique_resource.h
		bstone_uptr_resource_list.h
		bstone_version.h
)

if (WIN32)
	target_sources(
		${PROJECT_NAME}
		PRIVATE
			sys/win32/bstone_win32_rc.h
			sys/win32/bstone_win32_registry.h
	)
endif ()

target_sources(
	${PROJECT_NAME}
	PRIVATE
		dosbox/dbopl.cpp

		lib/xbrz/xbrz.cpp

		bstone_extent_2d.cpp
		bstone_offset_2d.cpp
		bstone_rect_2d.cpp

		bstone_hw_shader_registry.cpp
		bstone_hw_texture_mgr.cpp
		bstone_missing_sprite_64x64_image.cpp
		bstone_missing_wall_64x64_image.cpp
		bstone_ren_3d_mgr.cpp
		bstone_sdl2_exception.cpp

		bstone_ren_3d_cmd_buffer.cpp

		bstone_detail_ren_3d_utils.cpp

		bstone_detail_ren_3d_gl_api.cpp
		bstone_detail_ren_3d_gl_buffer.cpp
		bstone_detail_ren_3d_gl_context.cpp
		bstone_detail_ren_3d_gl_error.cpp
		bstone_detail_ren_3d_gl_extension_mgr.cpp
		bstone_detail_ren_3d_gl.cpp
		bstone_detail_ren_3d_gl_utils.cpp
		bstone_detail_ren_3d_gl_sampler.cpp
		bstone_detail_ren_3d_gl_sampler_mgr.cpp
		bstone_detail_ren_3d_gl_shader.cpp
		bstone_detail_ren_3d_gl_shader_stage.cpp
		bstone_detail_ren_3d_gl_shader_stage_mgr.cpp
		bstone_detail_ren_3d_gl_shader_var.cpp
		bstone_detail_ren_3d_gl_texture_2d.cpp
		bstone_detail_ren_3d_gl_texture_mgr.cpp
		bstone_detail_ren_3d_gl_vertex_input.cpp
		bstone_detail_ren_3d_gl_vertex_input_mgr.cpp

		3d_act1.cpp
		3d_act2.cpp
		3d_agent.cpp
		3d_debug.cpp
		3d_draw.cpp
		3d_draw2.cpp
		3d_game.cpp
		3d_inter.cpp
		3d_main.cpp
		3d_menu.cpp
		3d_msgs.cpp
		3d_play.cpp
		3d_scale.cpp
		3d_state.cpp
		colormap.cpp
		d3_d2.cpp
		d3_dr2.cpp
		id_ca.cpp
		id_in.cpp
		id_pm.cpp
		id_sd.cpp
		id_us.cpp
		id_us_1.cpp
		id_vh.cpp
		id_vl.cpp
		jm_free.cpp
		jm_lzh.cpp
		jm_tp.cpp
		markhack.cpp
		movie.cpp
		scale.cpp
		stub.cpp
		vgapal.cpp

		bstone_adlib_decoder.cpp
		bstone_adlib_music_decoder.cpp
		bstone_adlib_sfx_decoder.cpp
		bstone_archiver.cpp
		bstone_content_path.cpp
		bstone_audio_decoder.cpp
		bstone_audio_mixer.cpp
		bstone_audio_sample_converter.cpp
		bstone_binary_reader.cpp
		bstone_binary_writer.cpp
		bstone_cl_args.cpp
		bstone_crc32.cpp
		bstone_dosbox_dbopl.cpp
		bstone_encoding.cpp
		bstone_exception.cpp
		bstone_file_stream.cpp
		bstone_file_system.cpp
		bstone_fizzle_fx.cpp
		bstone_format_string.cpp
		bstone_generic_fizzle_fx.cpp
		bstone_gog_content_path.cpp
		bstone_logger.cpp
		bstone_low_pass_filter.cpp
		bstone_memory_binary_reader.cpp
		bstone_memory_stream.cpp
		bstone_mt_task_mgr.cpp
		bstone_opl3.cpp
		bstone_pcm_audio_decoder.cpp
		bstone_ps_fizzle_fx.cpp
		bstone_rgb8.cpp
		bstone_sha1.cpp
		bstone_sprite.cpp
		bstone_sprite_cache.cpp
		bstone_steam_content_path.cpp
		bstone_stream.cpp
		bstone_string_helper.cpp
		bstone_text_reader.cpp
		bstone_text_writer.cpp
		bstone_version.cpp
)

if (WIN32)
	target_sources(
		${PROJECT_NAME}
		PRIVATE
			"${PROJECT_BINARY_DIR}/sys/win32/bstone_win32.rc"
			sys/win32/bstone_win32_registry.cpp
	)
endif ()


#
# Install.
#
install(
	TARGETS
		${PROJECT_NAME}
	RUNTIME DESTINATION "."
)

install(
	FILES
		../CHANGELOG.md
	DESTINATION "."
	RENAME bstone_changelog.txt
)

install(
	FILES
		../README.md
	DESTINATION "."
	RENAME bstone_readme.txt
)

install(
	FILES
		../LICENSE
	DESTINATION "."
	RENAME bstone_license.txt
)

install(
	FILES
		../COPYING-SDL2.txt
	DESTINATION "."
	RENAME bstone_sdl2_license.txt
)
