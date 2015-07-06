################################################################################
#
# rtnet
#
################################################################################

RTNET_VERSION = HEAD
RTNET_SITE = git://github.com/hiddeate2m/rtnet.git
RTNET_DEPENDENCIES = linux xenomai
RTNET_INSTALL_STAGING = YES
RTNET_INSTALL_TARGET = YES
RTNET_CONF_OPT = --with-linux=$(LINUX_DIR) \
                 --with-rtext-config=$(STAGING_DIR)/usr/bin/xeno-config \
                 --with-module-dir=/lib/modules/$(LINUX_VERSION_PROBED)/kernel\
                 --disable-fec\
                 --enable-rtpacket\
		 --enable-ticpsw \
                 --enable-examples\
                 --disable-e1000\
                 --disable-8139\
                 --disable-eepro100\
                 --disable-tdma\
                 --disable-rtcfg\
                 --disable-loopback


# TODO move that fix to xenomai package
define RTNET_FIX_XENO_CONFIG
    cp  $(STAGING_DIR)/usr/bin/xeno-config  $(STAGING_DIR)/usr/bin/xeno-config.orig
    sed -i $(STAGING_DIR)/usr/bin/xeno-config -e 's#"/usr/include/xenomai"#"$(STAGING_DIR)/usr/include/xenomai"#'
endef
RTNET_POST_PATCH_HOOKS += RTNET_FIX_XENO_CONFIG

define RTNET_EXPORT_SYMVERS
    cp $(@D)/Module.symvers $(STAGING_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/rtnet.symvers
endef
RTNET_POST_INSTALL_STAGING_HOOKS += RTNET_EXPORT_SYMVERS

define RTNET_RESTORE_XENO_CONFIG
    mv $(STAGING_DIR)/usr/bin/xeno-config.orig  $(STAGING_DIR)/usr/bin/xeno-config
endef

RTNET_POST_INSTALL_STAGING_HOOKS += RTNET_RESTORE_XENO_CONFIG

define RTNET_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 755 $(BR2_EXTERNAL)/package/rtnet/S51rtnet \
		$(TARGET_DIR)/etc/init.d/S51rtnet
endef

# TBC
#RTNET_DEPENDENCIES = host-libtool

$(eval $(autotools-package))
