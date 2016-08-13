#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([L2Stack], [2.0])
AC_CONFIG_SRCDIR([L2Stack/archive.cpp])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_RANLIB
AC_PROG_CXX
AC_PROG_CC

AC_CANONICAL_HOST
# check which host OS we are using
case $host in
  *darwin* )
	LIBGL="-framework OpenGL -framework Cocoa"
	;;
  * )
	LIBGL="-lGL"
	;;
esac

AC_SUBST(LIBGL)

#TODO: fix boost checks
#AX_BOOST_BASE
#AX_BOOST_FILESYSTEM

# Checks for libraries.
AC_SEARCH_LIBS([lua_pushboolean], [lua51 lua5.1], [], \
			   [AC_MSG_ERROR(Lua 5.1 required)])
AC_SEARCH_LIBS([SDL_Init], [SDL], [], [AC_MSG_ERROR(SDL required)])
AC_SEARCH_LIBS([SDLNet_Init], [SDL_net], [], [AC_MSG_ERROR(SDL_net required)])
AC_SEARCH_LIBS([Sound_Init], [SDL_sound], [], \
			   [AC_MSG_ERROR(SDL_sound required)])
AC_SEARCH_LIBS([IMG_LoadPNG_RW], [SDL_image], [], \
			   [AC_MSG_ERROR(SDL_image required)])
AC_SEARCH_LIBS([pcre_compile], [pcre pcre3], [], [AC_MSG_ERROR(pcre required)])
#AC_SEARCH_LIBS([glBegin], [gl GL], [], [AC_MSG_ERROR(OpenGL required)])
AC_SEARCH_LIBS([uncompress], [z], [], [AC_MSG_ERROR(zlib required)])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h stddef.h stdlib.h string.h])

# This seems a bit... unneccessary /J
# Checks for typedefs, structures, and compiler characteristics.
#AC_HEADER_STDBOOL
#AC_C_CONST
#AC_TYPE_INT32_T
#AC_TYPE_SIZE_T

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_FUNC_MEMCMP
#AC_CHECK_FUNCS([atexit memset pow sqrt])

# Equivalent?
AC_CHECK_FUNCS([atexit memset pow sqrt malloc memcmp])

#AC_CONFIG_FILES([Makefile])

AC_OUTPUT([Makefile tetlib/Makefile L2Stack/Makefile bigpak/Makefile evolver/Makefile])
