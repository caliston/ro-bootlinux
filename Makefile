TARGET  := LinuxBoot,ffa
SRCS    := module.c
OBJS	:= ${SRCS:.c=.o}
DEPS    := ${SRCS:.c=.dep} 
XDEPS   := $(wildcard ${DEPS}) 

CCFLAGS = -O2 -Wall -mlibscl -mmodule -mhard-float
#-Werror 
CMHGFLAGS = -tgcc -apcs 3/32/fpe3
LDFLAGS = -mlibscl -mmodule -mhard-float
LIBS    = 

CC	=	gcc
AS	=	asasm
CMHG	=	cmunge

.PHONY: all clean distclean 

all:: ${TARGET} 

ifneq (${XDEPS},) 
include ${XDEPS} 
endif 

${TARGET}: ${OBJS} crhdr.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.c %.dep 
	${CC} ${CCFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.c Makefile 
	${CC} ${CCFLAGS} -MM $< > $@ 

#${ASOBJS}: %.o: %.s
#	${AS} ${ASFLAGS} -o $@ -c $< 

crhdr.o:	cmhg/crhdr
	${CMHG} ${CMHGFLAGS} -o $@ $<

clean:: 
	-rm -f *~ *.o ${TARGET} 

distclean:: clean
