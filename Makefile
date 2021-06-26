# Project: algorithm_ansi_c 
# Makefile created by Steve Jhang
# Date modified: 2018.01.03

CPP      = g++.exe -mno-ms-bitfields
CC       = gcc.exe -mno-ms-bitfields
WINDRES  = windres.exe
OBJ      = main.o menu.o string_match.o data_struct.o utility.o mbr.o gpt.o fat.o evp.o file.o serial.o file_ksoc_array_generation.o
LINKOBJ  = main.o menu.o string_match.o data_struct.o utility.o mbr.o gpt.o fat.o evp.o file.o serial.o file_ksoc_array_generation.o

LIBS     = -L"C:/MinGW/lib" \
           -L"C:/MinGW/mingw32/lib" \
           -static-libgcc -m32
INCS     = -I"C:/MinGW/include" \
           -I"C:/MinGW/mingw32/include" \
           -I"C:/MinGW/lib/gcc/mingw32/5.3.0/include"
CXXINCS  = -I"C:/MinGW/include" \
           -I"C:/MinGW/mingw32/include" \
           -I"C:/MinGW/lib/gcc/mingw32/5.3.0/include" \
           -I"C:/MinGW/lib/gcc/mingw32/5.3.0/include/c++"
#LIBS     = -L"C:/MinGW64/mingw64/lib" -L"C:/MinGW64/mingw64/lib/gcc/x86_64-w64-mingw32/lib" -static-libgcc -m32
#INCS	  = -I"C:/MinGW64/mingw64/include -I"C:/MinGW64/mingw64/lib/gcc/x86_64-w64-mingw32/7.1.0/include"
#LIBS     = -L"C:/Program Files (x86)/Dev-Cpp/MinGW64/lib32" -L"C:/Program Files (x86)/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib32" -static-libgcc -m32
#INCS     = -I"C:/Program Files (x86)/Dev-Cpp/MinGW64/include" -I"C:/Program Files (x86)/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include" -I"C:/Program Files (x86)/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include"
#CXXINCS  = -I"C:/Program Files (x86)/Dev-Cpp/MinGW64/include" -I"C:/Program Files (x86)/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include" -I"C:/Program Files (x86)/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include" -I"C:/Program Files (x86)/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include/c++"

BIN      = algo.exe
CXXFLAGS = $(CXXINCS) -m32
CFLAGS   = $(INCS) -m32
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
