CC	= gcc
#FC	= g77
FC	= gfortran
NKF	= nkf -w
#BINDIR	= ../bin
BINDIR	= ../../
INCDIR	= ../inc

CFLAGS	= -O
INCDIRS = -I${INCDIR} -I/usr/local/pgplot -I/usr/local/include
LIBDIRS = -L/usr/local/pgplot -L/usr/local/lib
CLIBS	= -lcpgplot -lpgplot -lm -lX11
OBJS	= main.o

TARGET	= ${BINDIR}/plot_lv1_ql

${TARGET}: ${OBJS}
	${FC}  -o $@ ${OBJS} ${CFLAGS} -Wall ${INCDIRS} ${LIBDIRS} ${CLIBS}

.c.o:
	${CC} ${CFLAGS} ${INCDIRS} -c $< 

main.o		:
util.o		:

clean:
	rm -f *.o *~ ${TARGET}
