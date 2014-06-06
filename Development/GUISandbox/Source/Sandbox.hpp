#ifndef __SANDBOX_HPP__
#define __SANDBOX_HPP__

#define _CRT_SECURE_NO_WARNINGS

//Lousy Win32 defs...
#ifdef CreateDirectory
#undef CreateDirectory
#endif

#ifdef SetMenu
#undef SetMenu
#endif

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

#define GAME_WINDOW_WIDTH 800
#define GAME_WINDOW_HEIGHT 600

#endif
