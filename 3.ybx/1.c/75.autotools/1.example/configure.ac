#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


#· AC_PREREQ 宏声明本文件要求的autoconf版本，本例使用的版本为2.59。
#· AC_INIT 宏用来定义软件的名称和版本等信息，
#		”FULL-PACKAGE-NAME”为软件包名称，
#		”VERSION”为软件版本号，
#		”BUG-REPORT-ADDRESS”为BUG报告地址（一般为软件作者邮件地址）。
#·AC_CONFIG_SRCDIR 宏用来侦测所指定的源码文件是否存在，来确定源码目录的有效性。此处为当前目录下的hello.c。
#·AC_CONFIG_HEADER 宏用于生成config.h文件，以便autoheader使用。
#·AC_PROG_CC 用来指定编译器，如果不指定，选用默认gcc。
#·AC_OUTPUT 用来设定 configure 所要产生的文件，
#			如果是makefile，configure会把它检查出来的结果带入makefile.in文件产生合适的makefile。
#			使用Automake时，还需要一些其他的参数，这些额外的宏用aclocal工具产生。





AC_PREREQ([2.63])
#======================================
AC_INIT(hello, 1.0, admin@163.com)
AM_INIT_AUTOMAKE(hello, 1.0)
#======================================
AC_CONFIG_SRCDIR([hello.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT
#======================================
AC_OUTPUT(Makefile)
#======================================
