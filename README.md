# Production Line Simulation in C using POSIX Semaphores and Shared Memory
- During a execution, the program forks to create three producer/constructor processes that create new parts (each producer/constructor creates a different type of part), one painter process that 'paints' the parts, three part-checking processes and a single assembler.
- There are a total of three shared memory segments, each big enough to host only one part at a time. There is one shared memory segment 'between' the producers and the painter, another one between the painter and the checkers and a final one between the checkers and the assembler.
- POSIX Semaphores are being used to protect the access to each shared memory segment.
- The assembler process uses a simple queue data structure to store the parts it receives until it has enough parts to assemble the final product. It needs one of each time of parts to assemble the final product.
- The output of this program is two number. '''a''' is the average waiting time for each part to enter the painter and '''b''' is the average time each part takes from its initial production until its assemled.

## Dependencies
- In order to run this code you only need to have ```make``` and the ```gcc``` compiler installed on your machine.
- If by any chance those are not installed and you are using a Debian derivative OS (like Ubuntu), you can install them as such:
```
sudo apt-get install make
sudo apt-get install gcc
```

## Compile

- To compile, execute:
```
make
```
from the main directory.

## Execute

-  To execute, run the executable created from the compilation and include a command line argument that determines the number of parts you want to create:

```
./a.out 1000
```
