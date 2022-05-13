# Project: algo
# Makefile created by Steve Jhang
# Date modified: 2018.01.03

CPP       = g++.exe -mno-ms-bitfields
CC        = gcc.exe -mno-ms-bitfields
WINDRES   = windres.exe
OBJ       = main.o menu.o string_match.o data_struct.o utility.o \
			mbr.o gpt.o fat.o evp.o file.o serial.o file_ksoc_array_generation.o \
			list.o sort.o c_test.o tree.o menu_bignum.o
LINKOBJ   = main.o menu.o string_match.o data_struct.o utility.o \
			mbr.o gpt.o fat.o evp.o file.o serial.o file_ksoc_array_generation.o \
			list.o sort.o c_test.o tree.o menu_bignum.o C:/Workspace/gmp-6.2.1/demos/expr/libexpr.a
LIBS      = -L"C:/MinGW/lib" \
			-L"C:/MinGW/mingw32/lib" \
			-L"/usr/local/lib" \
			-lgmp \
			-static-libgcc -m32
INCS      = -I"C:/MinGW/include" \
			-I"C:/MinGW/mingw32/include" \
			-I"C:/MinGW/lib/gcc/mingw32/5.3.0/include" \
			-I"/usr/local/include"
CXXINCS   = -I"C:/MinGW/include" \
			-I"C:/MinGW/mingw32/include" \
			-I"C:/MinGW/lib/gcc/mingw32/5.3.0/include" \
			-I"C:/MinGW/lib/gcc/mingw32/5.3.0/include/c++"


BIN      = algo.exe
CXXFLAGS = $(CXXINCS) -m32
CFLAGS   = $(INCS) -g -m32
RM       = rm.exe -f
BFLAGS   = -g -o

.PHONY: all all-before all-after clean clean-custom

#all: clean all-before $(BIN) all-after
all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) $(BFLAGS) $(BIN) $(LIBS)

main.o: main.c
	$(CC) -c main.c $(BFLAGS) main.o $(CFLAGS)

menu.o: menu.c
	$(CC) -c menu.c $(BFLAGS) menu.o $(CFLAGS)

string_match.o: string_match.c
	$(CC) -c string_match.c $(BFLAGS) string_match.o $(CFLAGS)

data-struct.o: data_struct.c
	$(CC) -c data-struct.c $(BFLAGS) data_struct.o $(CFLAGS)

utility.o: utility.c
	$(CC) -c utility.c $(BFLAGS) utility.o $(CFLAGS)

mbr.o: mbr.c
	$(CC) -c mbr.c $(BFLAGS) mbr.o $(CFLAGS)

gpt.o: gpt.c
	$(CC) -c gpt.c $(BFLAGS) gpt.o $(CFLAGS)

fat.o: fat.c
	$(CC) -c fat.c $(BFLAGS) fat.o $(CFLAGS)

evp.o: evp.c
	$(CC) -c evp.c $(BFLAGS) evp.o $(CFLAGS)

file.o:	file.c
	$(CC) -c file.c $(BFLAGS) file.o $(CFLAGS)

file_ksoc_array_generation.o: file_ksoc_array_generation.c
	$(CC) -c file_ksoc_array_generation.c $(BFLAGS) file_ksoc_array_generation.o $(CFLAGS)

serial.o: serial.c
	$(CC) -c serial.c $(BFLAGS) serial.o $(CFLAGS)

list.o: list.c
	$(CC) -c list.c $(BFLAGS) list.o $(CFLAGS)

sort.o: sort.c
	$(CC) -c sort.c $(BFLAGS) sort.o $(CFLAGS)

c_test.o: c_test.c
	$(CC) -c c_test.c $(BFLAGS) c_test.o $(CFLAGS)

tree.o: tree.c
	$(CC) -c tree.c $(BFLAGS) tree.o $(CFLAGS)

menu_bignum.o: menu_bignum.c
	$(CC) -c menu_bignum.c $(BFLAGS) menu_bignum.o $(CFLAGS)