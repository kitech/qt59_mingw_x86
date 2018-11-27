EXTRA_DEFINES += Q_USE_SYBASE
host_build {
    QT_CPU_FEATURES.i386 = 
} else {
    QT_CPU_FEATURES.i386 = 
}
QT.global_private.enabled_features = alloca_malloc_h alloca sse2 dbus dbus-linked gui network qml-debug sql system-zlib testlib widgets xml
QT.global_private.disabled_features = alloca_h android-style-assets private_tests libudev posix_fallocate reduce_exports reduce_relocations release_tools stack-protector-strong
PKG_CONFIG_EXECUTABLE = i686-w64-mingw32.shared-pkg-config
QMAKE_LIBS_DBUS = -L/opt/mxe/usr/i686-w64-mingw32.shared/lib/pkgconfig/../..//lib -ldbus-1
QMAKE_INCDIR_DBUS = /opt/mxe/usr/i686-w64-mingw32.shared/lib/pkgconfig/../..//include/dbus-1.0 /opt/mxe/usr/i686-w64-mingw32.shared/lib/pkgconfig/../..//lib/dbus-1.0/include
QT_COORD_TYPE = double
QMAKE_LIBS_ZLIB = -lz
CONFIG -= precompile_header
CONFIG += cross_compile sse2 sse3 ssse3 sse4_1 sse4_2 avx avx2 avx512f avx512bw avx512cd avx512dq avx512er avx512ifma avx512pf avx512vbmi avx512vl compile_examples f16c largefile
QT_BUILD_PARTS += libs
QT_HOST_CFLAGS_DBUS += -I//opt/mxe/usr/i686-w64-mingw32.shared/lib/pkgconfig/../..//include/dbus-1.0 -I//opt/mxe/usr/i686-w64-mingw32.shared/lib/pkgconfig/../..//lib/dbus-1.0/include
