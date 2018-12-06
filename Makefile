OBJS      = main.o processes.o queue.o psm.o semaphores.o sharedMemory.o
SOURCE    = main.c processes.c queue.c psm.c semaphores.c sharedMemory.c
HEADER    = processes.h queue.h psm.h semaphores.h sharedMemory.h
OUT       = a.out
CC        = gcc
FLAGS     = -g -c

$(OUT): $(OBJS)
	@$(CC) -g -o $(OUT) $(OBJS)
	@echo Object files linked, $(OUT) created.

main.o: main.c psm.o
	@$(CC) $(FLAGS) main.c
	@echo main.o created.

processes.o: processes.c processes.h queue.o psm.o
	@$(CC) $(FLAGS) -o processes.o processes.c
	@echo processes.o created.

psm.o: psm.c psm.h semaphores.o sharedMemory.o
	@$(CC) $(FLAGS) -o psm.o psm.c
	@echo psm.o created

queue.o: queue.c queue.h
	@$(CC) $(FLAGS) -o queue.o queue.c
	@echo queue.o created

semaphores.o: semaphores.c semaphores.h
	@$(CC) $(FLAGS) -o semaphores.o semaphores.c
	@echo semaphores.o created

sharedMemory.o: sharedMemory.c sharedMemory.h
	@$(CC) $(FLAGS) -o sharedMemory.o sharedMemory.c
	@echo sharedMemory.o created

clean:
	@rm -f $(OBJS) $(OUT)
	@echo Object files and \'$(OUT)\' removed.

count:
	@wc $(SOURCE) $(HEADER)

touch:
	@touch $(SOURCE)
