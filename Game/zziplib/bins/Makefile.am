AUTOMAKE_OPTIONS = 1.4 foreign
AUTOTOOL_VERSION=autoconf-2.52 automake-1.5 libtool-1.4.2
DEFAULT_INCLUDES = # nothing - no default of -I. -I$(srcdir)
DEFS = @DEFS@ -I$(top_builddir) -I$(top_srcdir) # also for automake 1.4

bin_SCRIPTS = zzip-config
bin_PROGRAMS = zzcat zzdir zzxorcat zzxordir zzxorcopy
noinst_PROGRAMS = zziptest zzobfuscated
aclocaldir = $(datadir)/aclocal
aclocal_DATA = zziplib.m4
#
VERSION_INFO=@VERSION_INFO@
RELEASE_INFO=@RELEASE_INFO@
THREAD_SAFE=@THREAD_SAFE@
#

WCC10_CLEAN = *.obj *.lk1 *.mk1 *.mk *.sym *.tgt

EXTRA_DIST = $(aclocal_DATA)
CLEANFILES = $(WCC10_CLEAN)

zziptest_LDFLAGS = @ZZIPLIB_LDFLAGS@
zziptest_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzcat_LDFLAGS =  @ZZIPLIB_LDFLAGS@
zzcat_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzdir_LDFLAGS =  @ZZIPLIB_LDFLAGS@
zzdir_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzxorcat_LDFLAGS =  @ZZIPLIB_LDFLAGS@
zzxorcat_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzxordir_LDFLAGS =  @ZZIPLIB_LDFLAGS@
zzxordir_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzobfuscated_LDFLAGS =  @ZZIPLIB_LDFLAGS@
zzobfuscated_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz

../zzip/libzzip.la : @top_srcdir@/zzip/*.c
	(cd ../zzip && $(MAKE) `basename $@`)
