(a) Included files
  - writer.cc: This file is run when a server is created.

  - consumer.cc: This file contains the main functions of the server.

  - consumer.h: This file contains the structure for the consumer class.

  - generator.cc: This file is run when a client is created.

  - producer.cc: This file contains the main functions of the client.

  - producer.h: This file contains the structure for a producer class.

  - named_semaphore.cc: This file is used to make named semaphores.

  - named_semaphore.h: This file contains the structure for the named semaphore class.

  - shm_store.h: This file contains the struct for the size and buffer of shared memory.

  - transfer_file: This file is used to transfer the file name and path from the client to server.

  - makefile: This file is used to compile and link the server and client files.

(b) Their relationships
  - The first relationship group is that of the server. The writer.cc class includes the main method of the server, and it includes consumer.h, which is the outline class of consumer.cc. Writer.cc calls methods in consumer.cc. Consumer.cc also includes consumer.h, as it has the structure for the class and its variables. Consumer.h, while containing the structure for the consumer.cc class, also includes named_semaphore.h and shm_store.h. This ensures that the consumer class can use named semaphores by referencing the ,h file, and it also allows for use of the struct for shared memory.

  - The next relationship group is that of the client. The generator.cc class includes the main method of the client, and it includes producer.h, which is the outline class of producer.cc. Generator.cc calls methods in producer.cc. Producer.cc also includes producer.h, as it has the structure for the class and its variables. Producer.h, while containing the structure for the producer.cc class, also includes named_semaphore.h. This ensures that the producer class can use named semaphores by referencing the .h file.

(c) Classes/functionality each file group provides
// check this section !
  - writer.cc: This file contains the main method when text-server is run. First, it sets the signal handler to destroy the named semaphore. Then, it constructs a Consumer pointer called log_writer to build Consumer. Next, it calls Consume() to do the rest of the functionality of the server. To end, it returns zero.

  - consumer.cc: This file contains the main functions of the server. First, when it is called it sets each of the named semaphore names in the constructor. It creates the first semaphore with initial value zero and opens it. When Consume() is called, it enters a while loop to continuously connect to clients. It signals the first named semaphore down to wait for the client to connect. Then, once it is unlocked, it opens the transfer file by calling OpenFile with the name of the file and read/write permissions. When OpenFile is called, it retrieves a file descriptor by calling open, and prints invalid file if it fails. It gets the size of the file and returns. Back to the Consume method, it maps to transfer file pages in page cache. Then, it gets the file name that is stored in the transfer file and empties the transfer file. It opens the file that was stored in the transfer file, same as how the transfer file was opened. Then, it maps to file pages in the page cache. It closes the file, and opens the second semaphore, calling up. Finally, it releases the copy of mapped memory for the transfer and log files. In the destructor of this class, the first named semaphore is destroyed.

  - consumer.h: This file contains the structure for the consumer class. It includes several .h files that the consumer class uses, including the named semaphore and shared memory store. It declares the public constructor and destructor,  and it declares the Consume method that does the bulk of the work. Privately, it declares the OpenFile method and the HandleError method. Finally, it declares several important variables like the two named semaphores with their names, as well as the transfer file name.

  - generator.cc: This file is run when a client is created. It declares a Producer. First, it ensures that when this class is run, it has one argument: the file name. It calls Produce in the producer.cc class with the file path and name from the arguments. Once this runs, it terminates by returning zero.

  - producer.cc: This file contains the main functions of the client. In the constructor, it sets the names of the two named semaphores. Then, it opens the first named semaphore created and locked by the consumer. In the produce method, it opens the transfer file with the same OpenFile method and ensures that this file is empty. It then increases the size of the transfer file to add the file name, and it closes the file. It puts the file name in the transfer file, and calls msync to update the transfer file. It signals the first named semaphore up, and releases the copy of mapped memory in transfer file. While the server runs, this class creates the second semaphore with initial value zero, opens it, and calls down to wait for the server. Once this lock is released, it opens the given file and maps to its file pages in the page cache. It then makes four p-threads, calling create with reference to the method upperCase. For each thread, depending on the thread id it searches through each character. If the character's ascii value indicates that it is lowercase, it subtracts 32 from this value to make it uppercase. This method returns the thread id, and once all threads run this, the Produce method calls join, and the file is updated. The file is then closed, and it releases its copy of mapped memory from the file. It terminates by returning zero.

  - producer.h: This file contains the structure for a producer class. It includes several .h files including that for the named semaphores. The class declares the public constructor and the public method Produce that takes in a file path. It also declares several private variables. These include two named semaphores with their names, as well as the name of the transfer file.  

  - named_semaphore.cc: This file is used to make named semaphores. It declares a method create to make a new named semaphore, and it takes in a count. This method creates a new semaphore with a name. It also includes a method called open to open an existing semaphore.

  - named_semaphore.h: This file contains the structure for the named semaphore class. It declares 2 public constructors: one for a constant character name and another for a string name. Both of these declare the name and semaphore pointer variables. Next, it declares the Create method that returns void and takes in an integer. It also declares a destroy method to unlink the named semaphore. It states the up method and down method to unlock/lock the semaphore. Finally, it states the private variables: a string for the name and a sem_t pointer to represent the semaphore pointer.

  - shm_store.h: This file contains the struct for the size and buffer of shared memory. It defines the struct to include a size_t buffer size and a character array buffer with size 4096 minus the size of a size_t.

  - transfer_file: This file is used to transfer the file path and name from the client to the server. 

  - makefile: This file is used to compile and link the server and client files.
