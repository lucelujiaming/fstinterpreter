#****************************************************************************
#
# Copyright 2013-2014 Altera Corporation. All Rights Reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# 
#****************************************************************************

#
# $Id$
#
GCCPATH := /usr/local/gcc-linaro-arm-linux-gnueabihf-4.8-2014.04_linux/bin
GCCPREFIX := arm-linux-gnueabihf-

C0_SRC   := ./load.c 
C0_SRA   := ./altera_trampoline.s


MULTILIBFLAGS := 
CWERRORFLG := -Werror=implicit-function-declaration -Werror=return-type
CFLAGS  := -g -O0 -Wall -std=c99 $(CWERRORFLG) $(MULTILIBFLAGS) 

ASFLAGS = -D__ASSEMBLY__ 
ASFLAGS += $(CFLAGS) 

CROSS_COMPILE := $(GCCPATH)/$(GCCPREFIX)
CC := $(CROSS_COMPILE)gcc
AS := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)g++
NM := $(CROSS_COMPILE)nm
OD := $(CROSS_COMPILE)objdump
OC := $(CROSS_COMPILE)objcopy

RM := rm -rf
CP := cp -f


C0_ELF ?= $(basename $(firstword $(C0_SRC))).axf
C0_OBJ := $(patsubst %.c,%.o,$(C0_SRC))
C0_SSOB := $(patsubst %.s,%.o,$(C0_SRA))

.PHONY: all
all: $(C0_ELF)

.PHONY: clean
clean:
	$(RM) $(C0_ELF) $(C0_OBJ) $(C0_SSOB) *.objdump *.map 

$(C0_OBJ): %.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@
$(C0_SSOB): %.o: %.s
	$(AS) $(ASFLAGS) -c $< -o $@
	

$(C0_ELF): $(C0_OBJ) $(C0_SSOB)
	$(CC)  $(C0_OBJ) $(C0_SSOB) $(GCC_LIBS) -o $@
	$(OD) -d $@ > $@.objdump
	$(NM) $@ > $@.map

