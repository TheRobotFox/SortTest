AC_INIT([SortTest],[6.0], [pfoch.tomke@gmail.com])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC

build_linux=no
build_windows=no

CFLAGS=" -O3 -mavx2 -s -m64"

# Detect the target system
case "${host_os}" in
    linux*)
        build_linux=yes
        ;;
    cygwin*|mingw*|msys*)
        build_windows=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac

# Pass the conditionals to automake
AM_CONDITIONAL([BUILD_LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([BUILD_WIN32], [test "$build_windows" = "yes"])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_CONFIG_SRCDIR([src/SortTest.c])

AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([pow])
AC_CHECK_FUNCS([sqrt])
AC_FUNC_MALLOC
AC_PATH_X

AC_OUTPUT
