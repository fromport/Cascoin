PACKAGE=qt
$(package)_version=5.15.11
$(package)_download_path=https://download.qt.io/official_releases/qt/5.15/$($(package)_version)/single
$(package)_suffix=everywhere-opensource-src-$($(package)_version).tar.xz
$(package)_file_name=qt-$($(package)_suffix)
$(package)_sha256_hash=7426b1eaab52ed169ce53804bdd05dfe364b761468f888a0f15a308dc1dc2951
$(package)_dependencies=openssl zlib
$(package)_linux_dependencies=freetype fontconfig libxcb
$(package)_build_subdir=qtbase
$(package)_qt_libs=corelib network widgets gui plugins testlib
$(package)_patches=fix_qt_pkgconfig.patch mac-qmake.conf fix_no_printer.patch

$(package)_qttranslations_file_name=qttranslations-$($(package)_suffix)
$(package)_qttranslations_sha256_hash=a78be1c4dd39c33550013fc19a99074d827e45f5e93e1c528b39c994c1e002b9

$(package)_qttools_file_name=qttools-$($(package)_suffix)
$(package)_qttools_sha256_hash=98b2aaca230458f65996f3534fd471d2ef4fef12a2767bb4c56e3dcc0cd74e5d

$(package)_extra_sources  = $($(package)_qttranslations_file_name)
$(package)_extra_sources += $($(package)_qttools_file_name)

define $(package)_set_vars
$(package)_config_opts_release = -release
$(package)_config_opts_debug = -debug
$(package)_config_opts += -bindir $(build_prefix)/bin
$(package)_config_opts += -c++std c++17
$(package)_config_opts += -confirm-license
$(package)_config_opts += -hostprefix $(build_prefix)
$(package)_config_opts += -no-compile-examples
$(package)_config_opts += -no-cups
$(package)_config_opts += -no-egl
$(package)_config_opts += -no-eglfs
$(package)_config_opts += -no-evdev
$(package)_config_opts += -no-gif
$(package)_config_opts += -no-glib
$(package)_config_opts += -no-icu
$(package)_config_opts += -no-ico
$(package)_config_opts += -no-iconv
$(package)_config_opts += -no-kms
$(package)_config_opts += -no-linuxfb
$(package)_config_opts += -no-libjpeg
$(package)_config_opts += -no-libproxy
$(package)_config_opts += -no-libudev
$(package)_config_opts += -no-mtdev
$(package)_config_opts += -no-opengl
$(package)_config_opts += -no-openvg
$(package)_config_opts += -no-reduce-relocations
$(package)_config_opts += -no-schannel
$(package)_config_opts += -no-sctp
$(package)_config_opts += -no-securetransport
$(package)_config_opts += -no-sql-db2
$(package)_config_opts += -no-sql-ibase
$(package)_config_opts += -no-sql-oci
$(package)_config_opts += -no-sql-tds
$(package)_config_opts += -no-sql-mysql
$(package)_config_opts += -no-sql-odbc
$(package)_config_opts += -no-sql-psql
$(package)_config_opts += -no-sql-sqlite
$(package)_config_opts += -no-sql-sqlite2
$(package)_config_opts += -no-system-proxies
$(package)_config_opts += -no-use-gold-linker
$(package)_config_opts += -no-zstd
$(package)_config_opts += -nomake examples
$(package)_config_opts += -nomake tests
$(package)_config_opts += -nomake tools
$(package)_config_opts += -opensource
$(package)_config_opts += -pkg-config
$(package)_config_opts += -prefix $(host_prefix)
$(package)_config_opts += -qt-libpng
$(package)_config_opts += -qt-pcre
$(package)_config_opts += -qt-harfbuzz
$(package)_config_opts += -qt-zlib
$(package)_config_opts += -static
$(package)_config_opts += -silent
$(package)_config_opts += -v
$(package)_config_opts += -no-feature-bearermanagement
$(package)_config_opts += -no-feature-colordialog
$(package)_config_opts += -no-feature-dial
$(package)_config_opts += -no-feature-fontcombobox
$(package)_config_opts += -no-feature-ftp
$(package)_config_opts += -no-feature-http
$(package)_config_opts += -no-feature-image_heuristic_mask
$(package)_config_opts += -no-feature-keysequenceedit
$(package)_config_opts += -no-feature-lcdnumber
$(package)_config_opts += -no-feature-pdf
$(package)_config_opts += -no-feature-printdialog
$(package)_config_opts += -no-feature-printer
$(package)_config_opts += -no-feature-printpreviewdialog
$(package)_config_opts += -no-feature-printpreviewwidget
$(package)_config_opts += -no-feature-sessionmanager
$(package)_config_opts += -no-feature-sql
$(package)_config_opts += -no-feature-syntaxhighlighter
$(package)_config_opts += -no-feature-textbrowser
$(package)_config_opts += -no-feature-textodfwriter
$(package)_config_opts += -no-feature-topleveldomain
$(package)_config_opts += -no-feature-udpsocket
$(package)_config_opts += -no-feature-undocommand
$(package)_config_opts += -no-feature-undogroup
$(package)_config_opts += -no-feature-undostack
$(package)_config_opts += -no-feature-undoview
$(package)_config_opts += -no-feature-vnc
$(package)_config_opts += -no-feature-wizard
$(package)_config_opts += -no-feature-xml

$(package)_config_opts_darwin = -no-dbus
$(package)_config_opts_darwin += -no-openssl-linked
$(package)_config_opts_darwin += -no-feature-corewlan
$(package)_config_opts_darwin += -no-freetype
$(package)_config_opts_darwin += QMAKE_MACOSX_DEPLOYMENT_TARGET=$(OSX_MIN_VERSION)

ifneq ($(build_os),darwin)
$(package)_config_opts_darwin += -xplatform macx-clang-linux
$(package)_config_opts_darwin += -device-option MAC_SDK_PATH=$(OSX_SDK)
$(package)_config_opts_darwin += -device-option MAC_SDK_VERSION=$(OSX_SDK_VERSION)
$(package)_config_opts_darwin += -device-option CROSS_COMPILE="$(host)-"
$(package)_config_opts_darwin += -device-option MAC_MIN_VERSION=$(OSX_MIN_VERSION)
$(package)_config_opts_darwin += -device-option MAC_TARGET=$(host)
$(package)_config_opts_darwin += -device-option XCODE_VERSION=$(XCODE_VERSION)
endif

$(package)_config_opts_linux = -xcb
$(package)_config_opts_linux += -no-xcb-xlib
$(package)_config_opts_linux += -no-feature-xlib
$(package)_config_opts_linux += -system-freetype
$(package)_config_opts_linux += -fontconfig
$(package)_config_opts_linux += -no-openssl-linked
$(package)_config_opts_linux += -dbus-linked
$(package)_config_opts_linux += -no-pulseaudio
$(package)_config_opts_linux += -no-alsa

$(package)_config_opts_mingw32 = -no-dbus
$(package)_config_opts_mingw32 += -no-freetype
$(package)_config_opts_mingw32 += -no-fontconfig
$(package)_config_opts_mingw32 += -no-openssl
$(package)_config_opts_mingw32 += -xplatform win32-g++
$(package)_config_opts_mingw32 += -device-option CROSS_COMPILE="$(host)-"
endef

define $(package)_fetch_cmds
$(call fetch_file,$(package),$($(package)_download_path),$($(package)_download_file),$($(package)_file_name),$($(package)_sha256_hash))
endef

define $(package)_extract_cmds
  mkdir -p $($(package)_extract_dir) && \
  echo "$($(package)_sha256_hash)  $($(package)_source)" > $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  $(build_SHA256SUM) -c $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  python3 $(BASEDIR)/extract_helper.py $($(package)_source) $($(package)_extract_dir) 1
endef

define $(package)_preprocess_cmds
  sed -i.old "s|updateqm.commands = \$$$$\$$$$LRELEASE|updateqm.commands = $($(package)_extract_dir)/qttools/bin/lrelease|" qttranslations/translations/translations.pro && \
  sed -i.old "/updateqm.depends =/d" qttranslations/translations/translations.pro && \
  patch -p1 -i $($(package)_patch_dir)/fix_qt_pkgconfig.patch && \
  patch -p1 -i $($(package)_patch_dir)/fix_no_printer.patch && \
  echo "!host_build: QMAKE_CFLAGS     += $($(package)_cflags) $($(package)_cppflags)" >> qtbase/mkspecs/common/gcc-base.conf && \
  echo "!host_build: QMAKE_CXXFLAGS   += $($(package)_cxxflags) $($(package)_cppflags)" >> qtbase/mkspecs/common/gcc-base.conf && \
  echo "!host_build: QMAKE_LFLAGS     += $($(package)_ldflags)" >> qtbase/mkspecs/common/gcc-base.conf && \
  sed -i.old "s|QMAKE_CC                = \$$$$\$$$${CROSS_COMPILE}gcc|QMAKE_CC                = $($(package)_cc)|" qtbase/mkspecs/common/gcc-base.conf && \
  sed -i.old "s|QMAKE_CXX               = \$$$$\$$$${CROSS_COMPILE}g++|QMAKE_CXX               = $($(package)_cxx)|" qtbase/mkspecs/common/gcc-base.conf && \
  sed -i.old "s|QMAKE_LINK              = \$$$$\$$$${CROSS_COMPILE}g++|QMAKE_LINK              = $($(package)_cxx)|" qtbase/mkspecs/common/gcc-base.conf && \
  sed -i.old "s|QMAKE_LINK_SHLIB        = \$$$$\$$$${CROSS_COMPILE}g++|QMAKE_LINK_SHLIB        = $($(package)_cxx)|" qtbase/mkspecs/common/gcc-base.conf && \
  sed -i.old "s|QMAKE_AR                = \$$$$\$$$${CROSS_COMPILE}ar|QMAKE_AR                = $($(package)_ar)|" qtbase/mkspecs/common/gcc-base.conf && \
  sed -i.old "s|QMAKE_OBJCOPY           = \$$$$\$$$${CROSS_COMPILE}objcopy|QMAKE_OBJCOPY           = $($(package)_objcopy)|" qtbase/mkspecs/common/gcc-base.conf && \
  sed -i.old "s|QMAKE_NM                = \$$$$\$$$${CROSS_COMPILE}nm|QMAKE_NM                = $($(package)_nm)|" qtbase/mkspecs/common/gcc-base.conf && \
  sed -i.old "s|QMAKE_STRIP             = \$$$$\$$$${CROSS_COMPILE}strip|QMAKE_STRIP             = $($(package)_strip)|" qtbase/mkspecs/common/gcc-base.conf && \
  sed -i.old "s|QMAKE_RC                = \$$$$\$$$${CROSS_COMPILE}windres|QMAKE_RC                = $($(package)_windres)|" qtbase/mkspecs/win32-g++/qmake.conf && \
  sed -i.old "s|QMAKE_LRELEASE          = \$$$$\$$$$\[QT_HOST_BINS\]/lrelease|QMAKE_LRELEASE          = $($(package)_extract_dir)/qttools/bin/lrelease|" qtbase/mkspecs/common/macx.conf
endef

define $(package)_config_cmds
  export PKG_CONFIG_SYSROOT_DIR=/ && \
  export PKG_CONFIG_LIBDIR=$(host_prefix)/lib/pkgconfig && \
  export PKG_CONFIG_PATH=$(host_prefix)/share/pkgconfig  && \
  export QT_MAC_SDK_NO_VERSION_CHECK=1 && \
  ./configure $($(package)_config_opts) $($(package)_config_opts_$(host_os)) -I $(host_prefix)/include
endef

define $(package)_build_cmds
  $(MAKE) -j$(nproc)
endef

define $(package)_stage_cmds
  $(MAKE) -j$(nproc) INSTALL_ROOT=$($(package)_staging_dir) install
endef

define $(package)_postprocess_cmds
  rm -rf native/lib/ lib/lib*.la lib/*.prl plugins/*/*.prl
endef
