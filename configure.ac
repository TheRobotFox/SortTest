AC_INIT([SortTest],[6.0], [pfoch.tomke@gmail.com])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([-Wall -Werror foreign])


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

AC_CONFIG_FILES([Makefile src/Makefile])

AC_CONFIG_SRCDIR([src/SortTest.c])

AC_CHECK_FUNCS([memset])
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([pow])
AC_CHECK_FUNCS([sqrt])
AC_FUNC_MALLOC
AC_PATH_X

AC_OUTPUT
