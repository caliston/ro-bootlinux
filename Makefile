TARGET  := LinuxBoot,ffa
SRCS    := module.c linux.c
ASSRCS	:= boot.s
OBJS	:= ${SRCS:.c=.o}
ASOBJS	:= ${ASSRCS:.s=.o}
DEPS    := ${SRCS:.c=.dep} 
XDEPS   := $(wildcard ${DEPS}) 

CCFLAGS = -O2 -Wall -mlibscl -mmodule -mhard-float
#-Werror 
CMHGFLAGS = -tgcc -apcs 3/32/fpe3
LDFLAGS = -mlibscl -mmodule -mhard-float
ASFLAGS	=
LIBS    = 

CC	=	gcc
AS	=	asasm
CMHG	=	cmunge

.PHONY: all clean distclean 

all:: ${TARGET} 

ifneq (${XDEPS},) 
include ${XDEPS} 
endif 

${TARGET}: ${OBJS} ${ASOBJS} modhdr.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.c %.dep 
	${CC} ${CCFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.c Makefile 
	${CC} ${CCFLAGS} -MM $< > $@ 

${ASOBJS}: %.o: %.s
	${AS} ${ASFLAGS} -o $@ $< 

modhdr.o:	cmhg/modhdr
	${CMHG} ${CMHGFLAGS} -o $@ $<

clean:: 
	-rm -f *~ *.o ${TARGET} 

distclean:: clean
