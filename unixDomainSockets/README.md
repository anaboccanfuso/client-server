(a) Included Files:
  - server.cpp: This file is run when a server is created.

  - client.cpp: This file is run when a client is created, and it links to a server socket.

  - socket.h: This file contains the structure for the UnixDomainSocket class.

  - makefile: This file is used to compile and like the server.cpp and client.cpp files.

(b) Their relationships:
  - The most prominent relationship within these files are those including socket.h. Since both the client and server have to use UNIX Domain Socket structure and function, they both require this identical class. Because of this, this class is declared in the socket.h file, making it easy for both server.cpp and client.cpp to include and implement it.

  - The makefile links and compiles both the server and client files, as well as cleans up files made in the process. Therefore, this file is critical to running the server and client files so that they may have a relationship with each other.

  - The client and server source files include all the definitions and functionality. From the socket.h header file, they further define the class. Additionally, each of the source files has its own unique class specific to the functionality it needs to perform.

(c) The classes/functionality each file group provides:
  - server.cpp: This file is run when a server compiles. When "make text-server" is run, server.cpp is linked and compiled. And when "./text-server socketname" is run, this file begins executing with the given socket name. The server establishes a socket with the given name, calculates the maximum number of clients it can sustain, and waits for clients to connect. Once a client connects, the server displays this, and it establishes a separate socket for just them. The server displays the file path and the seeking string. Then it searches through the given file for the phrase that the client inputted. Once the whole file is parsed through, the server sends the lines of text where the search key has been found to the client. Finally, the server outputs the number of bytes that were sent, and the client disconnects. The server continues to wait for more connections.

  - client.cpp: This file is run when a client is connecting to a server to request information. In this case, the information is where a search key appears in a text file. When "make text-client" is run, client.cpp is linked and compiled. And when "./text-client socketname filepath "search key"" is run, this file begins executing with the given socket name, file path, and search key. The client connects to the server's socket, displaying if their connection was accepted, and then it reads the sent lines of text from the server. The client adds line numbers in the beginning of each line, as well as a tab, and prints these lines. Finally, it displays the number of bytes read from the server and disconnects from the socket.

  - socket.h: This file contains the structure for the UnixDomainSocket class. It is included in both the server and client files, and it defines a public explicit function that takes in a constant character array (the socket path). Additionally, it defines the protected variables for the socket address and socket path.

  - makefile: This file is used to compile and like the server.cpp and client.cpp files. In this case, it uses the G++ compiler and sets the flags. It then gives a structure to link and compile each file, when "make text-client" or "make text-server" is run. Then, when "make clean" is run, it cleans up any executable or intermediary build files created by the makefile.
