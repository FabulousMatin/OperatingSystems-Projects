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
./<role> <UDP port>
``` 
tcp port will be assigned automatically and the starting tcp port can be written in `port.txt`.

## CA2: Map-Reduce Pipes
## CA3: Threding