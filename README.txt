-*-*- Compiling and running client -*-*-

    The clientside application can be compiled using make: 'make'

    The resulting binary is 'client.o', and expects 4 arguments: Load balancer ip,
    load balancer port, algorithm number, algorithm argument.

    An example of running the EndFlip algorithm with 1 flip each iteration:
    './client.o 35.184.11.109 5005 3 1'


-*-*- Compiling and running server/load balancer -*-*-

    To compile and launch the load balancer and servers, we have created a script.
    The script expects an argument, which changes the behavior:
    arg = 0:
        Compile and launch the server with no output redirection.

    arg = 1:
        Compile and launch the server with output redirection to log files contained in the logs folder

    arg = 2:
        Compile and launch the load balancer with no output redirection.


-*-*- Counterexamples -*-*-

    The counterexamples we have found are contained in the 'counterexamples' folder.
    The counterexamples are named '{width}.txt', and the biggest one we found was 306.


-*-*- Logs -*-*-

    We have several gigabytes of logs from the servers, but have chosen to include a sample of one of these logs.
