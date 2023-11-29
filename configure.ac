AC_INIT([SortTest],[8.0], [pfoch.tomke@gmail.com])
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [enable GDB Debug options])],
  [enable_debug=$enableval],
  [enable_debug=no])

# Output a message indicating the state of conscreen
AC_MSG_NOTICE([DEBUG: $enable_debug])

if test "x$enable_debug" = xyes; then
  AC_DEFINE([DEBUG], [1], [Define to 1 to compile in Debug])
fi

AC_ARG_ENABLE([conscreen],
  [AS_HELP_STRING([--enable-conscreen], [Use Conscreen for GUI])],
  [enable_conscreen=$enableval],
  [enable_conscreen=no])

if test "x$enable_conscreen" = xyes; then
  AC_DEFINE([CONSCREEN], [1], [Define to 1 to use Conscreen for GUI])
fi

AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])


build_linux=no
build_windows=no
build_nspire=no


# Detect the target system
case "${host_os}" in
    linux*)
        build_linux=yes
        ;;
    cygwin*|mingw*|msys*)
        build_windows=yes
        CC="clang"
        ;;
    nspire)
    	build_nspire=yes
        CC="nspire-gcc"   
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac

AC_PROG_CC

# Pass the conditionals to automake
AM_CONDITIONAL([BUILD_LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([BUILD_WIN32], [test "$build_windows" = "yes"])
AM_CONDITIONAL([BUILD_NSPIRE], [test "$build_nspire" = "yes"])
AM_CONDITIONAL([DEBUG], [test "x$enable_debug" = "xyes"])

AC_CONFIG_FILES([Makefile])

AC_CONFIG_SRCDIR([SortTest.c])

AC_CHECK_FUNCS([memset pow sqrt log])
AC_PATH_X
AC_OUTPUT
