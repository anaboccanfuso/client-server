(a) Included Files:

  - writer.cc: This file is run when a server is created.

  - consumer.cc: This file contains the main functions of the server.

  - consumer.h: This file contains the structure for the consumer class.

  - generator.cc: This file is run when a client is created.

  - producer.cc: This file contains the main functions of the client.

  - producer.h: This file contains the structure for a producer class.

  - named_semaphore.cc: This file is used to make named semaphores.

  - named_semaphore.h: This file contains the structure for the named semaphore class.

  - shm_store.h: This file contains the struct for the size and buffer of shared memory.

  - makefile: This file is used to compile and link the server and client files.

(b) Their relationships:
  - The first relationship group is that of the server. The writer.cc class includes the main method of the server, and it includes consumer.h, which is the outline class of consumer.cc. Writer.cc calls methods in consumer.cc. Consumer.cc also includes consumer.h, as it has the structure for the class and its variables. Consumer.h, while containing the structure for the consumer.cc class, also includes named_semaphore.h and shm_store.h. This ensures that the consumer class can use named semaphores by referencing the ,h file, and it also allows for use of the struct for shared memory.
  - The next relationship group is that of the client. The generator.cc class includes the main method of the client, and it includes producer.h, which is the outline class of producer.cc. Generator.cc calls methods in producer.cc. Producer.cc also includes producer.h, as it has the structure for the class and its variables. Producer.h, while containing the structure for the producer.cc class, also includes named_semaphore.h and shm_store.h. This ensures that the producer class cna use named semaphores by referencing the .h file, and it also allows for the use of the struct for shared memory. 

(c) The classes/functionality each file group provides:
  - writer.cc: This file contains the main method when text-server is run. First, it sets the signal handler to destroy the named semaphore. Then, it constructs a Consumer pointer called log_writer to build Consumer with the shared memory name and named semaphore name. Next, it calls Consume() to do the rest of the functionality of the server. To end, it returns zero.

  - consumer.cc: This file contains the main functions of the server. First, when it is called it sets the shared memory name, and each of the named semaphore names. It creates the semaphore with initial value zero and opens it. When Consume() is called, it enters a while loop to continuously connect to clients. It signals the first named semaphore down to wait for the client to connect. Then, once it is unlocked, it opens shared memory. It receives the file name and path from the shared memory, writing it to the terminal. It then opens the next two named semaphores. It opens the file, and if it fails it notifies the client by sending "INVALID FILE" to the shared memory. If the file is valid, it reads line by line and puts a single line at a time into shared memory. Once one line is placed into shared memory, it signals the second named semaphore up to signal the client to read the shared memory. Right after this call, however, it calls the third named semaphore down to wait to send the next line of the file. Once the client has read the shared memory, it signals the third named semaphore up, and the server puts the next line of the file in shared memory. After all lines have been sent, the server sends a line containing "\0" to the shared memory and signals the client. It then closes the file and main memory. In the destructor of this class, the first named semaphore is destroyed.

  - consumer.h: This file contains the structure for the consumer class. It includes several .h files that the consumer class uses, including the named semaphore and shared memory store. It declares the public constructor and destructor,  and it declares the Consume method that does the bulk of the work. Finally, it declares several important variables like the shared memory name, the shared memory store pointer, and three named semaphores.

  - generator.cc: This file is run when a client is created. It declares a Producer with the shared memory name and named semaphore name. First, it ensures that when this class is run, it has two arguments: the file path and search string. Similar to writer.cc, it sets the signal handler to unlink the shared memory and two of the named semaphores by calling the destructor. Then, it calls Produce in the producer.cc class with the file path and search string from the arguments in the terminal. Once this runs, it terminates by returning zero.

  - producer.cc: This file contains the main functions of the client. In the constructor, it sets the shared memory name, as well as the names of the three named semaphores. Then, it creates the shared memory, opens it, sets the size, and gets the copy of mapped memory. It sets the buffer size of the shared memory, and opens the first named semaphore created and locked by the consumer. In the produce method, it sets the search string variable, and sends this to the server through shared memory. Next, it creates and opens the other two named semaphores, setting their value to zero. It then calls up to the first named semaphore to signal the server to read the file name from shared memory. It then enters a while loop to retrieve each line from the file through shared memory. Once it enters the while loop, the second semaphore calls down to wait for the server to put a line of text into shared memory. After this is done, it stores this line in a string. If it is the end of the transmissions, it breaks the while loop. If the entered file name is invalid, it prints invalid file and exits. Otherwise, it adds the line of text to a vector and signals the third semaphore up to allow the server to write to shared memory again. After all lines are transmitted, it creates four threads using the create command, which calls the function findMatch. This function declares a long to represent the thread id, which comes from the void pointer. It then declares a size_t  begin to only copy the string from the first character. Then, depending on the thread, it searches one-fourth of the lines. If the line has the search string, it adds it to a vector of that thread. Once this is completed, it writes all lines of text that have the search string to stdout by printing each string in the four vectors. Then, it releases the copy of mapped memory and returns zero.

  - producer.h: This file contains the structure for a producer class. It includes several .h files including that for the named semaphore and shared memory store, as well as that for p threads. The class declares the public constructor with the shared memory name and named semaphore name, the public destructor, and the public method Produce that takes in a file path and search string. It also declares several private variables. These include a pointer to the shared memory name, a pointer to the shared memory store, and three named semaphores.  

  - named_semaphore.cc: This file is used to make named semaphores. It declares a method create to make a new named semaphore, and it takes in a count. This method creates a new semaphore with a name. It also includes a method called open to open an existing semaphore.

  - named_semaphore.h: This file contains the structure for the named semaphore class. It declares 2 public constructors: one for a constant character name and another for a string name. Both of these declare the name and semaphore pointer variables. Next, it declares the Create method that returns void and takes in an integer. It also declares a destroy method to unlink the named semaphore. It states the up method and down method to unlock/lock the semaphore. Finally, it states the private variables: a string for the name and a sem_t pointer to represent the semaphore pointer.

  - shm_store.h: This file contains the struct for the size and buffer of shared memory. It defines the struct to include a size_t buffer size and a character array buffer with size 4096 minus the size of a size_t.

  - makefile: This file is used to compile and link the server and client files.
