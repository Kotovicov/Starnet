I did this project for an essay at the university. The server and client parts of the program are presented here.

The server is implemented in python. The client is in C++ and used Visual Studio with ISO C++17 for compilation.

The server is running while waiting for a connection from the client. As soon as the client is connected, the server sends a chain of commands that must be executed on the client.

Currently, the client has the functionality to collect device characteristics and keystrokes from the keyboard.



Inside the StarNet_client directory:

- StarNet_client.cpp - main, main program loop, event calls.

- Command.cpp - edits the command line to communicate with the server.

- Keylogger.cpp - currently not working properly, buffer overflow.

The names of the other programs are self-explanatory.


