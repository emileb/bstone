
extern int main_mobile (int argc, char *argv[]);


#include "3d_def.h"
#include "id_in.h"

#include "game_interface.h"

#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include "SDL.h"
#include "SDL_keycode.h"

#ifndef LOGI
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"JNI", __VA_ARGS__))
//#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "JNI", __VA_ARGS__))
//#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"JNI", __VA_ARGS__))
#endif


extern bool g_mobileMenusAreFaded;
extern bool g_mobileInConfirm;
extern bool g_mobileInElevator;
extern bool g_mobileWaitForAnyKey;
extern bool g_mobileInPresenter;

extern "C"
{

extern int SDL_SendKeyboardKey(Uint8 state, SDL_Scancode scancode);

int PortableKeyEvent(int state, int code, int unicode){
	LOGI("PortableKeyEvent %d %d %d",state,code,unicode);

	if (state)
		SDL_SendKeyboardKey(SDL_PRESSED, (SDL_Scancode)code);
	else
		SDL_SendKeyboardKey(SDL_RELEASED, (SDL_Scancode) code);

	return 0;

}

static bool  android_buttonImpulse[e_bi_last_entry] = {0};
static bool  android_buttonDown[e_bi_last_entry] = {0};

static bool checkAck = false;

void PortableAction(int state, int action)
{
	LOGI("PortableAction %d   %d",state,action);

	BindingId key = e_bi_last_entry;

	if (((action >= PORT_ACT_MENU_UP) && (action <= PORT_ACT_MENU_ABORT)) &&
	    (( PortableGetScreenMode() == TS_MENU ) || ( PortableGetScreenMode() == TS_Y_N )))
	{
	    SDL_Scancode scanCode = SDL_SCANCODE_UNKNOWN;

	    switch (action)
		{
		case PORT_ACT_MENU_UP:
		    scanCode = SDL_SCANCODE_UP;
		    break;
        case PORT_ACT_MENU_DOWN:
            scanCode = SDL_SCANCODE_DOWN;
            break;
        case PORT_ACT_MENU_LEFT:
            scanCode = SDL_SCANCODE_LEFT;
            break;
        case PORT_ACT_MENU_RIGHT:
            scanCode = SDL_SCANCODE_RIGHT;
            break;
        case PORT_ACT_MENU_SELECT:
            scanCode = SDL_SCANCODE_RETURN;
            break;
        case PORT_ACT_MENU_BACK:
            scanCode = SDL_SCANCODE_ESCAPE;
            break;
        case PORT_ACT_MENU_CONFIRM:
            scanCode = SDL_SCANCODE_Y;
            break;
        case PORT_ACT_MENU_ABORT:
            scanCode = SDL_SCANCODE_N;
            break;
		}

		if( scanCode != SDL_SCANCODE_UNKNOWN )
			PortableKeyEvent( state, scanCode, 0);
	}
	else
	{
        switch (action)
        {

        case PORT_ACT_LEFT:
            key = e_bi_left;
            break;
        case PORT_ACT_RIGHT:
            key = e_bi_right;
            break;
        case PORT_ACT_FWD:
            key = e_bi_forward;
            break;
        case PORT_ACT_BACK:
            key = e_bi_backward;
            break;

        case PORT_ACT_MOVE_LEFT:
            key = e_bi_strafe_left;
            break;
        case PORT_ACT_MOVE_RIGHT:
            key = e_bi_strafe_right;
            break;

        case PORT_ACT_USE:
            key = e_bi_use;
            break;
        case PORT_ACT_ATTACK:
            key = e_bi_attack;
            break;

        case PORT_ACT_MAP:
            key = e_bi_stats;
            break;
        case PORT_ACT_MAP_ZOOM_IN:
            PortableKeyEvent(state,SDL_SCANCODE_EQUALS,0);
            break;
        case PORT_ACT_MAP_ZOOM_OUT:
            PortableKeyEvent(state,SDL_SCANCODE_MINUS,0);
            break;
        case PORT_ACT_NEXT_WEP:
            key = e_bi_cycle_next_weapon;
            break;
        case PORT_ACT_PREV_WEP:
            key = e_bi_cycle_previous_weapon;
            break;

        case PORT_ACT_QUICKSAVE:
            key = e_bi_quick_save;
            break;
        case PORT_ACT_QUICKLOAD:
            key = e_bi_quick_load;
            break;

        case PORT_ACT_WEAP1:
            key = e_bi_weapon_1;
            break;
        case PORT_ACT_WEAP2:
            key = e_bi_weapon_2;
            break;
        case PORT_ACT_WEAP3:
            key = e_bi_weapon_3;
            break;
        case PORT_ACT_WEAP4:
            key = e_bi_weapon_4;
            break;
        case PORT_ACT_WEAP5:
            key = e_bi_weapon_5;
            break;
        case PORT_ACT_WEAP6:
            key = e_bi_weapon_6;
            break;
        case PORT_ACT_WEAP7:
            key = e_bi_weapon_7;
            break;
        }
    }
	if (key != e_bi_last_entry)
	{
		android_buttonDown[ key ] = state;

		// used to capture keys which are pressed and released in zero time
		if( state )
			android_buttonImpulse[ key ] = true;
	}

	checkAck = true;
}

// =================== FORWARD and SIDE MOVMENT ==============

static float forwardmove, sidemove; //Joystick mode

void PortableMoveFwd(float fwd)
{
	if (fwd > 1)
		fwd = 1;
	else if (fwd < -1)
		fwd = -1;

	forwardmove = fwd;
}

void PortableMoveSide(float strafe)
{
	if (strafe > 1)
		strafe = 1;
	else if (strafe < -1)
		strafe = -1;

	sidemove = strafe;
}

void PortableMove(float fwd, float strafe)
{
	PortableMoveFwd(fwd);
	PortableMoveSide(strafe);
}

//======================================================================

//Look up and down
static int look_pitch_mode;
static float look_pitch_mouse,look_pitch_abs,look_pitch_joy;
void PortableLookPitch(int mode, float pitch)
{
	look_pitch_mode = mode;
	switch(mode)
	{
	case LOOK_MODE_MOUSE:
		look_pitch_mouse += pitch;
		break;
	case LOOK_MODE_ABSOLUTE:
		look_pitch_abs = pitch;
		break;
	case LOOK_MODE_JOYSTICK:
		look_pitch_joy = pitch;
		break;
	}
}

//left right
static int look_yaw_mode;
static float look_yaw_mouse,look_yaw_joy;
void PortableLookYaw(int mode, float yaw)
{
	look_yaw_mode = mode;
	switch(mode)
	{
	case LOOK_MODE_MOUSE:
		look_yaw_mouse += yaw;
		break;
	case LOOK_MODE_JOYSTICK:
		look_yaw_joy = yaw;
		break;
	}
}



void PortableInit(int argc,const char ** argv)
{
	main_mobile(argc,(char **)argv);
}


touchscreemode_t PortableGetScreenMode()
{
//LOGI("%d  %d  %d  %d",g_mobileMenusAreFaded,ingame , g_mobileInConfirm ,g_mobileInElevator);
	if( g_mobileInConfirm )
		 return TS_Y_N;
	else if (g_mobileWaitForAnyKey)
		return TS_MENU;
    else if (!g_mobileMenusAreFaded || !ingame || g_mobileInElevator || g_mobileInPresenter)
        return TS_MENU;
    else
        return TS_GAME;
}

void PortableBackButton()
{
    PortableKeyEvent(1, SDL_SCANCODE_ESCAPE,0 );
    // Horrible and risky, wait for 200ms, in this time the game must have run a frame or 2 to pickup the key press
    usleep( 1000 * 200 );
    PortableKeyEvent(0, SDL_SCANCODE_ESCAPE, 0);
}

void PortableAutomapControl(float zoom, float x, float y)
{

}

}

#define BASEMOVE                35
#define RUNMOVE                 70
#define BASETURN                35
#define RUNTURN                 70

void mobile_StartAck()
{
	checkAck = false;
}

bool mobile_CheckAck()
{
	bool ret = checkAck;
	checkAck = false;
	return ret;
}

bool mobile_is_binding_pressed(  BindingId binding_id )
{
	bool ret = android_buttonDown[ binding_id ] || android_buttonImpulse[ binding_id ];

	// Clear an impulse
	android_buttonImpulse[ binding_id ] = false;
	return ret;
}

void mobile_in_reset_binding_state(  BindingId binding_id )
{
	android_buttonDown[ binding_id ] = false;
}
//NOTE this is cpp
void pollAndroidControls(int tics,int *controlx, int *controly,int *controlstrafe)
{
	*controly  -= forwardmove     * tics * BASEMOVE;
	*controlstrafe  += sidemove   * tics * BASEMOVE;


	switch(look_yaw_mode)
	{
	case LOOK_MODE_MOUSE:
		*controlx += -look_yaw_mouse * 8000;
		look_yaw_mouse = 0;
		break;
	case LOOK_MODE_JOYSTICK:
		*controlx += -look_yaw_joy * 80;
		break;
	}

}


