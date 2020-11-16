LOCAL_PATH := $(call my-dir)/../



include $(CLEAR_VARS)


LOCAL_MODULE    := bstone

LOCAL_CFLAGS =  -O0 -g -fsigned-char -D__MOBILE__ -DENGINE_NAME=\"bstone\" -DBSTONE_USE_GLES -DBSTONE_OGL_DIRECT_LINK -DCEILING_FLOOR_COLORS -DBSTONE_REN_3D_TEST_NO_GL_2_0 -DBSTONE_REN_3D_TEST_NO_GL_3_2_C
LOCAL_CFLAGS += -fexceptions -frtti

LOCAL_C_INCLUDES = $(LOCAL_PATH)/dosbox \
                   $(LOCAL_PATH)/lib/xbrz \
                   $(LOCAL_PATH)/lib/glm \
                   $(LOCAL_PATH)/android \
                   $(TOP_DIR)/ \
	               $(LOCAL_PATH)/../../../Clibs_OpenTouch/alpha

#also include my SDL for SDL_android_extra.h 
LOCAL_C_INCLUDES += $(SDL_INCLUDE_PATHS)  $(TOP_DIR)/MobileTouchControls $(TOP_DIR)/Clibs_OpenTouch


     #
          
SRC =  \
        ../../../Clibs_OpenTouch/alpha/android_jni.cpp \
        ../../../Clibs_OpenTouch/alpha/touch_interface.cpp \
        android/game_interface.cpp \
 		dosbox/dbopl.cpp \
 		lib/xbrz/xbrz.cpp \
 		bstone_archiver.cpp \
 		bstone_extent_2d.cpp \
 		bstone_offset_2d.cpp \
 		bstone_rect_2d.cpp \
 		bstone_hw_shader_registry.cpp \
 		bstone_hw_texture_mgr.cpp \
 		bstone_missing_sprite_64x64_image.cpp \
 		bstone_missing_wall_64x64_image.cpp \
 		bstone_ren_3d_mgr.cpp \
 		bstone_sdl2_exception.cpp \
 		bstone_ren_3d_cmd_buffer.cpp \
 		bstone_detail_ren_3d_utils.cpp \
 		bstone_detail_ren_3d_gl_api.cpp \
 		bstone_detail_ren_3d_gl_buffer.cpp \
 		bstone_detail_ren_3d_gl_context.cpp \
 		bstone_detail_ren_3d_gl_error.cpp \
 		bstone_detail_ren_3d_gl_extension_mgr.cpp \
 		bstone_detail_ren_3d_gl.cpp \
 		bstone_detail_ren_3d_gl_utils.cpp \
 		bstone_detail_ren_3d_gl_sampler.cpp \
 		bstone_detail_ren_3d_gl_sampler_mgr.cpp \
 		bstone_detail_ren_3d_gl_shader.cpp \
 		bstone_detail_ren_3d_gl_shader_stage.cpp \
 		bstone_detail_ren_3d_gl_shader_stage_mgr.cpp \
 		bstone_detail_ren_3d_gl_shader_var.cpp \
 		bstone_detail_ren_3d_gl_texture_2d.cpp \
 		bstone_detail_ren_3d_gl_texture_mgr.cpp \
 		bstone_detail_ren_3d_gl_vertex_input.cpp \
 		bstone_detail_ren_3d_gl_vertex_input_mgr.cpp \
 		3d_act1.cpp \
 		3d_act2.cpp \
 		3d_agent.cpp \
 		3d_debug.cpp \
 		3d_draw.cpp \
 		3d_draw2.cpp \
 		3d_game.cpp \
 		3d_inter.cpp \
 		3d_main.cpp \
 		3d_menu.cpp \
 		3d_msgs.cpp \
 		3d_play.cpp \
 		3d_scale.cpp \
 		3d_state.cpp \
 		colormap.cpp \
 		d3_d2.cpp \
 		d3_dr2.cpp \
 		id_ca.cpp \
 		id_in.cpp \
 		id_pm.cpp \
 		id_sd.cpp \
 		id_us.cpp \
 		id_us_1.cpp \
 		id_vh.cpp \
 		id_vl.cpp \
 		jm_free.cpp \
 		jm_tp.cpp \
 		jm_lzh.cpp \
 		markhack.cpp \
 		movie.cpp \
 		scale.cpp \
 		stub.cpp \
 		vgapal.cpp \
 		bstone_low_pass_filter.cpp \
 		bstone_adlib_decoder.cpp \
 		bstone_adlib_music_decoder.cpp \
 		bstone_adlib_sfx_decoder.cpp \
 		bstone_content_path.cpp \
 		bstone_audio_decoder.cpp \
 		bstone_audio_mixer.cpp \
 		bstone_binary_reader.cpp \
 		bstone_binary_writer.cpp \
 		bstone_cl_args.cpp \
 		bstone_crc32.cpp \
 		bstone_dosbox_dbopl.cpp \
 		bstone_encoding.cpp \
 		bstone_exception.cpp \
 		bstone_file_stream.cpp \
 		bstone_file_system.cpp \
 		bstone_fizzle_fx.cpp \
 		bstone_format_string.cpp \
 		bstone_generic_fizzle_fx.cpp \
 		bstone_gog_content_path.cpp \
 		bstone_logger.cpp \
 		bstone_memory_binary_reader.cpp \
 		bstone_memory_stream.cpp \
 		bstone_mt_task_mgr.cpp \
 		bstone_opl3.cpp \
 		bstone_pcm_audio_decoder.cpp \
 		bstone_audio_sample_converter.cpp \
 		bstone_ps_fizzle_fx.cpp \
 		bstone_rgb8.cpp \
 		bstone_sha1.cpp \
 		bstone_sprite.cpp \
 		bstone_sprite_cache.cpp \
 		bstone_steam_content_path.cpp \
 		bstone_stream.cpp \
 		bstone_string_helper.cpp \
 		bstone_text_reader.cpp \
 		bstone_text_writer.cpp \
 		bstone_version.cpp \
	
	
 
LOCAL_SRC_FILES = $(SRC)

LOCAL_LDLIBS :=   -lEGL -ldl -llog -lz -lm -lc  -lGLESv2 -lGLESv1_CM
LOCAL_STATIC_LIBRARIES := logwritter
LOCAL_SHARED_LIBRARIES := touchcontrols  SDL2 SDL2_mixer core_shared

LOCAL_STATIC_LIBRARIES +=

include $(BUILD_SHARED_LIBRARY)






