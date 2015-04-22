# AkiComm
Project History - March 2011

This project marks the first time I ever worked with multithreading, as well as the first time I ever worked with socket programming. This is a chat program that runs through the command prompt capable of connecting via TCP/IP with multiple people. It runs as a server/client with the server managing the entire session. It supports various commands such as getting a list of users, or (for the server) kicking a client.

Multithreading was used to manage the clients. Every time a client joined, a new thread would be launched to manage the connection with that client. Whenever a client quit, that thread would die. Client messages would come in and join a message queue that the server would process by broadcasting it back out to all the other clients.

I also wrote my own linked list for this, for whatever reason.

Demonstration video: <br>
https://www.youtube.com/watch?v=IubLoz3YubA
