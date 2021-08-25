#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules
export LIBWIFI := $(DEVKITARM)/libwifi
export LIBFAT := $(DEVKITARM)/libfat
export LIBFILESYSTEM := $(DEVKITARM)/libfilesystem

export GAME_TITLE		:=	BlackTigerDS
export GAME_SUBTITLE1	:=	Black Tiger Arcade Emulator
export GAME_SUBTITLE2	:=	www.ndsretro.com
export GAME_ICON		:=	$(CURDIR)/Capcom.bmp
export TARGET			:=	$(shell basename $(CURDIR))
export TOPDIR			:=	$(CURDIR)


.PHONY: arm7/$(TARGET).elf arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).nds	:	arm7/$(TARGET).elf arm9/$(TARGET).elf
	@ndstool -c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf -b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)"
	@echo built ... $(notdir $@)

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).arm7 $(TARGET).arm9 $(TARGET).nds
