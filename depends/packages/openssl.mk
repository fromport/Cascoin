package=openssl
$(package)_version=3.4.1
$(package)_download_path=https://www.openssl.org/source
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=002a2d6b30b58bf4bea46c43bdd96365aaf8daa6c428782aa4feee06da197df3
# $(package)_patches=fix_installdata_pm.patch  # Not needed for OpenSSL 3.4.1+

define $(package)_set_vars
$(package)_config_env=AR="$($(package)_ar)" RANLIB="$($(package)_ranlib)" CC="$($(package)_cc)" CROSS_COMPILE="" CXX="$($(package)_cxx)" LD="$($(package)_ld)" NM="$($(package)_nm)" STRIP="$($(package)_strip)"
$(package)_build_env=CROSS_COMPILE=""
$(package)_config_opts=--prefix=$(host_prefix) --openssldir=$(host_prefix)/etc/openssl
$(package)_config_opts+=no-camellia
$(package)_config_opts+=no-cast
$(package)_config_opts+=no-comp
$(package)_config_opts+=no-dso
$(package)_config_opts+=no-dtls
$(package)_config_opts+=no-apps
$(package)_config_opts+=no-ec_nistp_64_gcc_128
$(package)_config_opts+=no-gost
$(package)_config_opts+=no-idea
$(package)_config_opts+=no-md2
$(package)_config_opts+=no-mdc2
$(package)_config_opts+=no-rc4
$(package)_config_opts+=no-rc5
$(package)_config_opts+=no-rfc3779
$(package)_config_opts+=no-sctp
$(package)_config_opts+=no-seed
$(package)_config_opts+=no-shared
$(package)_config_opts+=no-ssl-trace
$(package)_config_opts+=no-ssl3
$(package)_config_opts+=no-unit-test
$(package)_config_opts+=no-weak-ssl-ciphers
$(package)_config_opts+=no-whirlpool
$(package)_config_opts+=no-zlib
$(package)_config_opts+=no-zlib-dynamic
$(package)_config_opts+=no-pic
$(package)_config_opts_x86_64_mingw32+=no-PIE
$(package)_cflags_x86_64_mingw32=-fno-pic -fno-PIE
$(package)_config_opts+=$($(package)_cflags) $($(package)_cppflags)
$(package)_config_opts_linux=-fPIC -Wa,--noexecstack
$(package)_config_opts_x86_64_linux=linux-x86_64
$(package)_config_opts_i686_linux=linux-generic32
$(package)_config_opts_arm_linux=linux-generic32
$(package)_config_opts_aarch64_linux=linux-generic64
$(package)_config_opts_mipsel_linux=linux-generic32
$(package)_config_opts_mips_linux=linux-generic32
$(package)_config_opts_powerpc_linux=linux-generic32
$(package)_config_opts_riscv64_linux=linux-generic64
$(package)_config_opts_x86_64_darwin=darwin64-x86_64-cc
$(package)_config_opts_x86_64_mingw32=mingw64
$(package)_config_opts_i686_mingw32=mingw
endef

# Preprocess step removed - patch no longer needed for OpenSSL 3.4.1+
# define $(package)_preprocess_cmds
#   patch -p1 -i $($(package)_patch_dir)/fix_installdata_pm.patch
# endef

define $(package)_config_cmds
  ./Configure $($(package)_config_opts)
endef

define $(package)_build_cmds
  test -s builddata.pm || ( \
    echo "# Fallback builddata.pm for Perl require/use" > builddata.pm && \
    echo "package OpenSSL::safe::installdata;" >> builddata.pm && \
    echo "1;" >> builddata.pm \
  ) && \
  test -s installdata.pm || ( \
    echo "# Fallback installdata.pm for Perl require/use" > installdata.pm && \
    echo "package OpenSSL::safe::installdata;" >> installdata.pm && \
    echo "1;" >> installdata.pm \
  ) && \
  $(MAKE) -j1 build_libs libcrypto.pc libssl.pc openssl.pc
endef

define $(package)_stage_cmds
  $(MAKE) INSTALL_PREFIX=$($(package)_staging_dir) -j1 install_sw && \
  if [ -d $($(package)_staging_dir)$(host_prefix)/lib64 ]; then \
    cp -r $($(package)_staging_dir)$(host_prefix)/lib64/* $($(package)_staging_dir)$(host_prefix)/lib/ ; \
  fi
endef

define $(package)_postprocess_cmds
  rm -rf share bin etc lib64/engines-3 lib64/ossl-modules
endef
