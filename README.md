# Operating System Course Projects

- [CA1: Socket Programming](#ca1-socket-programming)
- [CA2: Map-Reduce Pipes](#ca2-map-reduce-pipes)
- [CA3: Threding](#ca3-threding)

## CA1: Socket Programming
There are two types of socket used in this program:
- UDP
- TCP

there is a UDP port for all grouped members* and each member has two tcp ports, one for sending and one for receving data(this policy is chosen to have parallel send/receive for each member)

*we can have more than one groups in parallel.

and there are three roles:
- Customer
- Restaurant
- Supplier

the logs will be saved in `./logs` folder.

to run:
```text
make all
./<role> <UDP port>
``` 
TCP port will be assigned automatically and the starting tcp port can be written in `port.txt`.

## CA2: Map-Reduce Pipes
Two types of pipes is used in this project:
- Unnamed/Ordinary pipes(for parent-child)
- Named pipes(also known as FIFO)

the main project create a bill handler and a process for each building(map), then each building create a process for its resources(map), there are ordinary pipes between all father-child process which we named. Each recource process also has a named pipe with bill handler to transfer data, and bill handler send bill to the main process(reduce). Also all resoucre send data to buildings(reduce) and finally buildings send information to main process(reduce).

It can be mentioned that all resources are design using inheritance.

to run:
```text
make all
./main.out
``` 

- the data will be read from `csv` files placed in `./buildings`
- pipes will be stored in `./pipes`
- exec files will be stored in `./exec`
- there will be a log file named as `logs.txt` which can be showed in terminal at the end of program.

## CA3: Threding

There are some image processing methods implemented in both serial and parallel way. paralleliztion is done using threading.

to run parallel:
```text
make all
./ImageFilters.out <input> <n_threads>
```
and to run serial:
```text
make all
./ImageFilters.out <input> <n_threads>
```

It is obvious that the value `n_thread` is limited to your cpu threads(from harware POV).

the report of comparison is in `./report` folder.