PACKAGE=qt
$(package)_version=6.5.3
$(package)_download_path=https://download.qt.io/official_releases/qt/6.5/$($(package)_version)/submodules
$(package)_suffix=everywhere-src-$($(package)_version).tar.xz
$(package)_file_name=qtbase-$($(package)_suffix)
$(package)_sha256_hash=df2f4a230be4ea04f9798f2c19ab1413a3b8ec6a80bef359f50284235307b546
$(package)_dependencies=openssl zlib
$(package)_linux_dependencies=freetype fontconfig libxcb libX11 xproto libXext
$(package)_build_subdir=qtbase
$(package)_qt_libs=corelib network widgets gui plugins testlib
$(package)_patches=

$(package)_qttranslations_file_name=qttranslations-$($(package)_suffix)
$(package)_qttranslations_sha256_hash=83b44c0ddcf9c28e836f63c21a3fea2fb2c24cb630eefc79984f5bec44e949e3

$(package)_qttools_file_name=qttools-$($(package)_suffix)
$(package)_qttools_sha256_hash=fc91d32b3f696725bbb48b0df240c25b606bbee3bd22627cfcbee876a6405e37

$(package)_extra_sources  = $($(package)_qttranslations_file_name)
$(package)_extra_sources += $($(package)_qttools_file_name)

define $(package)_set_vars
$(package)_config_opts_release = -release
$(package)_config_opts_debug = -debug
$(package)_config_opts += -prefix $(host_prefix)
$(package)_config_opts += -extprefix $(build_prefix)
$(package)_config_opts += -confirm-license
$(package)_config_opts += -opensource
$(package)_config_opts += -static
$(package)_config_opts += -no-shared
$(package)_config_opts += -nomake examples
$(package)_config_opts += -nomake tests
$(package)_config_opts += -no-dbus
$(package)_config_opts += -no-feature-concurrent
$(package)_config_opts += -no-feature-sql
$(package)_config_opts += -no-feature-testlib

$(package)_config_opts_mingw32 = -xplatform win32-g++
$(package)_config_opts_mingw32 += -device-option CROSS_COMPILE=x86_64-w64-mingw32-
$(package)_config_opts_mingw32 += -no-feature-printer
$(package)_config_opts_mingw32 += -no-feature-printdialog

$(package)_config_opts_linux = -platform linux-g++
$(package)_config_opts_linux += -xcb
$(package)_config_opts_linux += -no-feature-printer
$(package)_config_opts_linux += -no-feature-printdialog

$(package)_config_opts_darwin = -platform macx-clang
$(package)_config_opts_darwin += -no-feature-printer
$(package)_config_opts_darwin += -no-feature-printdialog

ifneq ($(build_os),darwin)
$(package)_config_opts_darwin = -xplatform macx-clang-linux
$(package)_config_opts_darwin += -device-option MAC_SDK_PATH=$(OSX_SDK)
$(package)_config_opts_darwin += -device-option MAC_SDK_VERSION=$(OSX_SDK_VERSION)
$(package)_config_opts_darwin += -device-option CROSS_COMPILE="$(host)-"
$(package)_config_opts_darwin += -device-option MAC_MIN_VERSION=$(OSX_MIN_VERSION)
$(package)_config_opts_darwin += -device-option MAC_TARGET=$(host)
endif

$(package)_build_env  = QT_RCC_TEST=1
$(package)_build_env += QT_RCC_SOURCE_DATE_OVERRIDE=1
endef

define $(package)_fetch_cmds
$(call fetch_file,$(package),$(subst \:,:,$($(package)_download_path_fixed)),$($(package)_download_file),$($(package)_file_name),$($(package)_sha256_hash)) && \
$(call fetch_file,$(package),$(subst \:,:,$($(package)_download_path_fixed)),$($(package)_qttranslations_file_name),$($(package)_qttranslations_file_name),$($(package)_qttranslations_sha256_hash)) && \
$(call fetch_file,$(package),$(subst \:,:,$($(package)_download_path_fixed)),$($(package)_qttools_file_name),$($(package)_qttools_file_name),$($(package)_qttools_sha256_hash))
endef

define $(package)_extract_cmds
  mkdir -p $($(package)_extract_dir) && \
  echo "$($(package)_sha256_hash)  $($(package)_source)" > $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  echo "$($(package)_qttranslations_sha256_hash)  $($(package)_source_dir)/$($(package)_qttranslations_file_name)" >> $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  echo "$($(package)_qttools_sha256_hash)  $($(package)_source_dir)/$($(package)_qttools_file_name)" >> $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  $(build_SHA256SUM) -c $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  mkdir qtbase && \
  tar --strip-components=1 -xf $($(package)_source) -C qtbase && \
  mkdir qttranslations && \
  tar --strip-components=1 -xf $($(package)_source_dir)/$($(package)_qttranslations_file_name) -C qttranslations && \
  mkdir qttools && \
  tar --strip-components=1 -xf $($(package)_source_dir)/$($(package)_qttools_file_name) -C qttools
endef

define $(package)_preprocess_cmds
endef

define $(package)_config_cmds
  export PKG_CONFIG_SYSROOT_DIR=/ && \
  export PKG_CONFIG_LIBDIR=$(host_prefix)/lib/pkgconfig && \
  export PKG_CONFIG_PATH=$(host_prefix)/share/pkgconfig && \
  ./configure $($(package)_config_opts)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

define $(package)_postprocess_cmds
  rm -rf lib/cmake/ && \
  rm -f lib/lib*.la lib/*.prl plugins/*/*.prl
endef