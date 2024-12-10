## Compiler, tools and options

#OBJS  	= AqDS_Chk_Sat2.o
TARGET 	= AqDS AqDS_Chk_Sat AqDS_Chk_Sat2 AqDS_open_everyday lawdata2savedata6 lawdata2savedata6_201106 read_sch rmfiles copy_gif copy_pdf

#FC 	= g77
FC 	= gfortran
CC 	= gcc
CFLAGS 	= -O -Wall
#CFLAGS 	= -O3 -march=i686 -fomit-frame-pointer -malign-double -funroll-loops -Wall
INCDIRS = -I/usr/local/pgplot
LIBDIRS = -L/usr/local/pgplot -L/usr/X11R6/lib
LIBS 	= -lcpgplot -lpgplot -lm -lX11

## Implicit rules

.SUFFIXES: .c .o

## Build rules

all: $(TARGET)

AqDS:	AqDS.c
	$(FC) -o AqDS AqDS.c $(CFLAGS) $(LIBDIRS) $(LIBS) $(INCDIRS)

AqDS_Chk_Sat:	AqDS_Chk_Sat.c
	$(FC) -o AqDS_Chk_Sat AqDS_Chk_Sat.c $(CFLAGS) $(LIBDIRS) $(LIBS) $(INCDIRS)

AqDS_Chk_Sat2:	 AqDS_Chk_Sat2.c
	$(FC) -o AqDS_Chk_Sat2 AqDS_Chk_Sat2.c $(CFLAGS) $(LIBDIRS) $(LIBS) $(INCDIRS)

AqDS_open_everyday:	AqDS_open_everyday.c
	$(CC) -o AqDS_open_everyday AqDS_open_everyday.c
	
lawdata2savedata6:	lawdata2savedata6.c
	$(CC) -o lawdata2savedata6 lawdata2savedata6.c
lawdata2savedata6_201106:	lawdata2savedata6_201106.c
	$(CC) -o lawdata2savedata6_201106 lawdata2savedata6_201106.c
read_sch:	read_sch.c
	$(CC) -o read_sch read_sch.c
rmfiles:	rmfiles.c
	$(CC) -o rmfiles rmfiles.c
copy_gif:	copy_gif.c
	$(CC) -o copy_gif copy_gif.c
copy_pdf:	copy_pdf.c
	$(CC) -o copy_pdf copy_pdf.c

#$(TARGET):
#	$(CC) -o $(TARGET).c $(CFLAGS) $(LIBDIRS) $(LIBS) $(INCDIRS)

## Compile

.c.o: $*.c
	$(CC) -c $*.c $(CFLAGS) $(INCDIRS)

## Clean

clean:
	rm -f $(OBJS) $(TARGET)
