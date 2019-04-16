/*
BStone: A Source port of
Blake Stone: Aliens of Gold and Blake Stone: Planet Strike

Copyright (c) 1992-2013 Apogee Entertainment, LLC
Copyright (c) 2013-2019 Boris I. Bendovsky (bibendovsky@hotmail.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the
Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


//
// OpenGL Renderer utils.
//
// !!! Internal usage only. !!!
//


#ifndef BSTONE_DETAIL_OGL_RENDERER_UTILS_INCLUDED
#define BSTONE_DETAIL_OGL_RENDERER_UTILS_INCLUDED


#include <vector>
#include "glm/glm.hpp"
#include "bstone_detail_renderer_utils.h"
#include "bstone_ogl.h"
#include "bstone_sdl_types.h"


namespace bstone
{
namespace detail
{


class OglRendererUtils
{
public:
	const std::string& get_error_message() const;


	bool load_library();

	static void unload_library();


	SdlGlContextUPtr create_context(
		SdlWindowPtr sdl_window);


	bool make_context_current(
		SdlWindowPtr sdl_window,
		SdlGlContextPtr sdl_gl_context);


	bool create_window_and_context(
		const RendererUtilsCreateWindowParam& param,
		SdlWindowUPtr& sdl_window,
		SdlGlContextUPtr& sdl_gl_context);

	bool create_probe_window_and_context(
		SdlWindowUPtr& sdl_window,
		SdlGlContextUPtr& sdl_gl_context);


	bool window_get_drawable_size(
		SdlWindowPtr sdl_window,
		int& width,
		int& height);


	static bool resolve_symbols_1_1();


	static void clear_buffers();

	static void swap_window(
		SdlWindowPtr sdl_window);

	static bool was_errors();


	static void set_color_buffer_clear_color(
		const RendererColor32& color);

	static void scissor_enable(
		const bool is_enabled);

	static void scissor_set_box(
		const int x,
		const int y,
		const int width,
		const int height);

	static void viewport_set_rectangle(
		const int x,
		const int y,
		const int width,
		const int height);

	static void viewport_set_depth_range(
		const float min_depth,
		const float max_depth);

	static void texture_2d_set(
		const GLuint ogl_texture_name);

	static void blending_set_function(
		const RendererBlendingFactor src_factor,
		const RendererBlendingFactor dst_factor);

	static GLenum index_buffer_get_element_type_by_byte_depth(
		const int byte_depth);


private:
	std::string error_message_;


	static GLenum blending_get_factor(
		const RendererBlendingFactor factor);

	static void* resolve_symbol(
		const char* const symbol);

	template<typename T>
	static void resolve_symbol(
		const char* const name,
		T& symbol,
		bool& is_failed)
	{
		symbol = reinterpret_cast<T>(resolve_symbol(name));

		if (!symbol)
		{
			is_failed = true;
		}
	}

	static void clear_unique_symbols_1_0();

	static bool resolve_unique_symbols_1_0();


	static void clear_unique_symbols_1_1();

	static bool resolve_unique_symbols_1_1();
}; // OglRendererUtils


} // detail
} // bstone


#endif // !BSTONE_DETAIL_OGL_RENDERER_UTILS_INCLUDED
