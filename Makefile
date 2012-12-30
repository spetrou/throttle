
MF=	Makefile

CC=	g++

CFLAGS=-I$(HOME)/local_software/include
LFLAGS=-L$(HOME)/local_software/lib -llog4cpp \
	   -lboost_date_time -lboost_thread -lboost_system
PREPROC=

EXE=	prog
SRC=	main.t.cpp util_fix_throttle.cpp

OBJ=	$(SRC:.cpp=.o)

INC=	util_fix_throttle.h

.SUFFIXES:
.SUFFIXES:	.cpp .o

all:	$(EXE)

.cpp.o:
	$(CC) $(CFLAGS) $(PREPROC) -c $<

$(EXE):	$(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LFLAGS) 

$(OBJ):	$(SRC) $(INC) $(MF)

clean:
	rm -f *.o $(EXE) core

run:
	./prog

