
SBU_REV= "0.1"


INCLUDE = -I$(PROJ_DIR)..\.. -I$(PROJ_DIR)..\..\..\..\framework\platform\windows -I$(PROJ_DIR)..\..\..\..\framework\rpcWindows -I$(PROJ_DIR)..\..\..\..\framework\mt -I$(PROJ_DIR)..\..\..\..\framework\mt\Af -I$(PROJ_DIR)..\..\..\..\framework\mt\Zdo -I$(PROJ_DIR)..\..\..\..\framework\mt\Sys -I$(PROJ_DIR)..\..\..\..\framework\mt\Sapi

CC= gcc.exe
#CC=\usr\local\angstrom\arm\bin\arm-angstrom-linux-gnueabi-gcc

CFLAGS= -Wall -g -Wall  -c
LIBS =
DEFS += -D_OBJC_NO_COM
PROJ_DIR=

Debug: servDisc.exe

servDisc.exe: main.o servDisc.o queue.o semaphore.o rpc.o mtParser.o mtZdo.o mtSys.o mtAf.o mtSapi.o dbgPrint.o hostConsole.o rpcTransport.o
	$(CC) $(LIBS) -o servDisc.exe main.o servDisc.o queue.o semaphore.o rpc.o mtParser.o mtZdo.o mtSys.o mtAf.o mtSapi.o dbgPrint.o hostConsole.o rpcTransport.o

# rule for file "main.o".
main.o: $(PROJ_DIR)main.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)main.c

# rule for file "servDisc.o".
servDisc.o: $(PROJ_DIR)..\..\servDisc.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\servDisc.c

# rule for file "rpcTransport.o".
queue.o: $(PROJ_DIR)..\..\..\..\framework\platform\windows\queue.h $(PROJ_DIR)..\..\..\..\framework\platform\windows\queue.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\platform\windows\queue.c

# rule for file "rpcTransport.o".
semaphore.o: $(PROJ_DIR)..\..\..\..\framework\platform\windows\semaphore.h $(PROJ_DIR)..\..\..\..\framework\platform\windows\semaphore.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\platform\windows\semaphore.c

# rule for file "rpc.o".
rpc.o: $(PROJ_DIR)..\..\..\..\framework\rpc\rpc.h $(PROJ_DIR)..\..\..\..\framework\rpcWindows\rpc.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\rpcWindows\rpc.c

# rule for file "mtParser.o".
mtParser.o: $(PROJ_DIR)..\..\..\..\framework\mt\mtParser.h $(PROJ_DIR)..\..\..\..\framework\mt\mtParser.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\mt\mtParser.c

# rule for file "mtZdo.o".
mtZdo.o: $(PROJ_DIR)..\..\..\..\framework\mt\Zdo\mtZdo.h $(PROJ_DIR)..\..\..\..\framework\mt\Zdo\mtZdo.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\mt\Zdo\mtZdo.c

# rule for file "mtSys.o".
mtSys.o: $(PROJ_DIR)..\..\..\..\framework\mt\Sys\mtSys.h $(PROJ_DIR)..\..\..\..\framework\mt\Sys\mtSys.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\mt\Sys\mtSys.c

# rule for file "mtAf.o".
mtAf.o: $(PROJ_DIR)..\..\..\..\framework\mt\Af\mtAf.h $(PROJ_DIR)..\..\..\..\framework\mt\Af\mtAf.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\mt\Af\mtAf.c

# rule for file "mtSapi.o".
mtSapi.o: $(PROJ_DIR)..\..\..\..\framework\mt\Sapi\mtSapi.h $(PROJ_DIR)..\..\..\..\framework\mt\Sapi\mtSapi.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\mt\Sapi\mtSapi.c

# rule for file "dbgPrint.o".
dbgPrint.o: $(PROJ_DIR)..\..\..\..\framework\platform\windows\dbgPrint.h $(PROJ_DIR)..\..\..\..\framework\platform\windows\dbgPrint.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\platform\windows\dbgPrint.c

# rule for file "hostConsole.o".
hostConsole.o: $(PROJ_DIR)..\..\..\..\framework\platform\windows\hostConsole.h $(PROJ_DIR)..\..\..\..\framework\platform\windows\hostConsole.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\platform\windows\hostConsole.c





# rule for file "rpcTransport.o".
rpcTransport.o: $(PROJ_DIR)..\..\..\..\framework\platform\windows\rpcTransport.h $(PROJ_DIR)..\..\..\..\framework\platform\windows\rpcTransport.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)..\..\..\..\framework\platform\windows\rpcTransport.c

# rule for cleaning files generated during compilations.
cleanDebug:
	-del servDisc.exe *.o
	echo Clean done
