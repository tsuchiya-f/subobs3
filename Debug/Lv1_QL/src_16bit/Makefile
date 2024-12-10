CC	= gcc
#FC	= g77
FC	= gfortran
NKF	= nkf -w
BINDIR	= ../../
INCDIR	= ../inc

CFLAGS	= -O
INCDIRS = -I${INCDIR} -I/usr/local/pgplot
LIBDIRS = -L/usr/local/pgplot
CLIBS	= -lcpgplot -lpgplot -lm -lX11
OBJS	= main.o

TARGET	= ${BINDIR}/plot_lv1_16bit

${TARGET}: ${OBJS}
	${FC}  -o $@ ${OBJS} ${CFLAGS} -Wall ${INCDIRS} ${LIBDIRS} ${CLIBS}

.c.o:
	${CC} ${CFLAGS} ${INCDIRS} -c $< 

main.o		:
util.o		:

clean:
	rm -f *.o *~ ${TARGET}
