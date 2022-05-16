#Programming Assignment II

Individual programming assignment for CISC450/CPEG419. Assignment covers 'File Transfer Over UDP'. Project written by William Walker.  A table of contents is provided below for your convenience.

[TOC]

##List of Relevant Files

- `./client/client.cpp` - the source file for running the client process
- `./server/server.cpp` - the source file for running the server process
- `./server/computer.jpg` - a picture file available for download by the client
- `./server/message.txt` - a text file available for download by the client
- `./server/email.txt` - a text file available for download by the client

##Compilation Instructions

While under development, I complied this project on macOS 10.14 using the clang++ compiler. The version output of my clang compiler is below: 

```
Apple LLVM version 10.0.1 (clang-1001.0.46.4)
Target: x86_64-apple-darwin18.5.0
Thread model: posix
```

In order to reproduce a working enviroment for demonstration, you must generate both the `client` and `server` executables. Within this project directory, both of these executables have already been genereated using the method below. Of course, if the environment on which this is being run is different than macOS 10.14.4, then it is suggested to rebuild both exectuables. 

With the clang compiler, this can be done by first changing into the `WillWalker` directory, and then: 

1. Changing into the `./client/` directory.
2. Generating the client executable by running: `clang++ -o client client.cpp` 
3. Changing into the `./server/` directory.
4. Generating the client executable by running: `clang++ -o server server.cpp`

## Configuration Files

No configuration files are required to execute this enviroment. 

However, it should be noted that the `computer.jpg` and `message.txt` files in the ./server/ directory are special to this project. As this assignment focuses on file transfer over UDP, the only two files I have specified for demonstration purposes are both smaller than the size specification of the message buffer.

##Running Instructions

The following instructions have been tested on macOS 10.14.4. After each instruction, I have provided an example of the command I ran on my machine. It is important to note that the program will only work for the files provided in the `./server/` directory. When a user wishes to exit the program, they should enter the corresponding hotkey/shortcut combination to exit command-line applications (i.e. for macOS, `Ctrl+C`)

1. Open two separate instances of Terminal (or your OS command-line program)

2. Change directories to the project directory (`WillWalker`) in both windows. 

   ```
   cd /Users/willwalker/Documents/CISC450/WillWalker
   ```

3. Consider one window as the "server process" and the other as the "client process". 

   1. Within the "server process window", change into the server folder (i.e. `cd server/`)
   2. Within the "client process window", change into the client folder (i.e. `cd client/`)

4. In the server window, start the program by providing a port number, error of probability, and window size. The program will return an error if the port you attempt to bind to is currently being used by another process. The following example will start a program on port 555, with a 80% probability of bit errors, and a window size of 1.

   `./server 555 80 1`

   If successful, you should see the following: 

   ```
   There is a 80% chance of bit errors
   Socket created successfully
   Bound to port 555
   Waiting for client…
   ```

5. In the client window, start the program by providing the server ip (in this case the `127.0.0.1` localhost loopback device), port number, and windows size. The following example will connect to a server on port 555 with a window size of 1.

   ```
   ./client 127.0.0.1 555 1
   ```

   If successful, you should see the following: 

   ```
   Client program started. Using window size 1. Communicating with 127.0.0.1:555
   Socket created successfully
   Enter a file request:|
   ```

6. At this point, on the client window, you may request a file from the server. To demonstrate what happens when the client requests a file that **does not** exist, type the following into the client program and then press enter: 

   `document.txt`

   The client process will report the following:

   ```
   Requested: document.txt
   Waiting for the server...
   Server says: ! - File does not exist
   Waiting for the server...
   ```

   The server process will report the following:

   ```
   Client requested: document.txt
   File does not exist!
   Waiting for client...
   ```

   A timeout counter will begin, as the client will wait for confirmation that the server does indeed not have the file available for download.  After 3 seconds, the client process will timeout, and once again ask for the user to enter a filename request for download.

7. In order to demonstrate the correct functionality of this program, we will again repeat the previous step, this time requesting to download `message.txt`.  Once a filename has been entered into the client program, it should return:

   ```
   Requested: message.txt
   Waiting for the server...
   File downloaded
   File checksum: 16
   ACK Sent
   Waiting for the server...
   ```

   A checksum will be generated for the downloaded file, and an acknowledgement sent back to the server.  If the server does not reply within the alloted time frame (in this case 3 seconds), then the client will assume the downloaded file is correct and close the connection.

8. In the event where the downloaded file has corrupted bytes, the server will resend the file.  Here is an example of the output of the client when the server is started with error probability set to 90, and the client requests the `computer.jpg` file from the server. This process will continue until the checksum generated by the client matches the checksum generated by the server.

  ```
  Requested: computer.jpg
  File downloaded
  File checksum: -13
  ACK Sent
  Waiting for the server...
  File downloaded
  File checksum: -102
  ACK Sent
  Waiting for the server...
  File downloaded
  File checksum: -45
  ACK Sent
  Waiting for the server...
  File downloaded
  File checksum: 19
  ACK Sent
  Waiting for the server...
  File downloaded
  File checksum: 19
  ACK Sent
  Waiting for the server...
  File downloaded
  File checksum: -87
  ACK Sent
  Waiting for the server...
  File downloaded
  File checksum: 57
  ......
  ```