COMPILE=arm-linux-gnueabihf-
GCC=$(COMPILE)gcc
LD=$(COMPILE)ld
OBJCOPY=$(COMPILE)objcopy
OBJDUMP=$(COMPILE)objdump


IMXDIR=$(shell cd .. && pwd)

LIBPATH := -L /usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/4.9.4 -lgcc

#获取所有.h路径
INCLUDES =  imx6ul \
			gpio \
			bsp/clk \
			bsp/delay \
			bsp/led   \
			bsp/key   \
			bsp/beep   \
			bsp/exti   \
			bsp/epit \
			bsp/uart \
			bsp/int  \
			stdio/include \
			stdio/lib 
INCLUDE = $(patsubst %, -I %, $(INCLUDES))


#获取.c .s
SRCDIRS =   project \
			gpio \
			bsp/clk \
			bsp/delay \
			bsp/led  \
			bsp/key \
			bsp/beep   \
			bsp/exti   \
			bsp/epit \
			bsp/uart \
			bsp/int \
			stdio/include \
			stdio/lib 

SRC_S = $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.s))
SRC_C = $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

#获取.o  1/去除路径信息 2、将后缀换为.o
OBJS_FILE_S = $(notdir $(SRC_S))
OBJS_FILE_C = $(notdir $(SRC_C))
SOBJS = $(patsubst %, obj/%, $(OBJS_FILE_S:.s=.o))
COBJS = $(patsubst %, obj/%, $(OBJS_FILE_C:.c=.o))
OBJS = $(SOBJS)$(COBJS)


CFLAGS = -g
#CFLAGS+=-O2
CFLAGS += -Wall
#不需要系统自带的启动文件 不加这个会用系统的启动文件 ===不连接系统标准启动文件和标准库文件，只把指定的文件传递给连接器。
CFLAGS += -nostdlib
CFLAGS += $(INCLUDE)
#不使用C语言的内建库 可以跟其相同名字
CFLAGS += -fno-builtin 
CFLAGS += -O2
CFLAGS += -Wa,-mimplicit-it=thumb

.PHONY:clean

RM=rm -rf
CP=cp -rf
MV= mv

target=Out

#当 make 需要去找寻文件的依赖关系时 添加VPATH
VPATH := $(SRCDIRS)

build:$(OBJS)
#链接.o为elf
	$(LD) -Timx6ull.lds $(OBJS)  $(LIBPATH) -o $(target).elf
	$(OBJCOPY) -O binary -S -g  $(target).elf $(target).bin
	$(OBJDUMP) -D $(target).elf > $(target).dis

#生成imx
	#$(CP) $(IMXDIR)/imxdownload ./ 
	./imxdownload $(target).bin /dev/sdb

	$(MV) *.dis *.bin *.elf *.imx  ./obj

#编译
$(SOBJS) : obj/%.o : %.s
	$(GCC) -c $< -o $@ $(CFLAGS)

$(COBJS) : obj/%.o : %.c
	$(GCC) -c $< -o $@ $(CFLAGS)


clean:
	$(RM) ./obj/*.o ./obj/*.bin ./obj/*.dis ./obj/*.imx ./obj/*.elf


print:
	@echo $(OBJS)
	@echo $(SOBJS)
	@echo $(COBJS)
