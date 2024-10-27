#include <stdio.h>
#include <errno.h> // Import for `errno` variable

#include <fcntl.h>      // Import for `fcntl` functions
#include <unistd.h>     // Import for `fork`, `fcntl`, `read`, `write`, `lseek, `_exit` functions
#include <sys/types.h>  // Import for `socket`, `bind`, `listen`, `accept`, `fork`, `lseek` functions
#include <sys/socket.h> // Import for `socket`, `bind`, `listen`, `accept` functions
#include <netinet/ip.h> // Import for `sockaddr_in` stucture

#include <string.h>  // Import for string functions
#include <stdbool.h> // Import for `bool` data type
#include <stdlib.h>  // Import for `atoi` function

#include "./functions/server-constants.h"
#include "./functions/admin.h"
#include "./functions/customer.h"
#include "./functions/manager.h"
#include "./functions/employee.h"


void connection_handler(int connFD);                // Handles the communication with the client

void main()
{
    int socketFileDescriptor, socketBindStatus, socketListenStatus, connectionFileDescriptor;
    struct sockaddr_in serverAddress, clientAddress;

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor == -1)
    {
        perror("Error while creating server socket!");
        _exit(0);
    }

    serverAddress.sin_family = AF_INET;                // IPv4
    serverAddress.sin_port = htons(8081);              // Server will listen to port 8080
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Binds the socket to all interfaces

    socketBindStatus = bind(socketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (socketBindStatus == -1)
    {
        perror("Error while binding to server socket!");
        _exit(0);
    }

    socketListenStatus = listen(socketFileDescriptor, 10);
    if (socketListenStatus == -1)
    {
        perror("Error while listening for connections on the server socket!");
        close(socketFileDescriptor);
        _exit(0);
    }

    int clientSize;
    while (1)
    {
        clientSize = (int)sizeof(clientAddress);
        connectionFileDescriptor = accept(socketFileDescriptor, (struct sockaddr *)&clientAddress, &clientSize);
        if (connectionFileDescriptor == -1)
        {
            perror("Error while connecting to client!");
            close(socketFileDescriptor);
        }
        else
        {
            if (!fork())
            {
                // Child will enter this branch
                connection_handler(connectionFileDescriptor);
                close(connectionFileDescriptor);
                _exit(0);
            }
        }
    }

    close(socketFileDescriptor);
}



void connection_handler(int connectionFileDescriptor) {
    printf("Client has connected to the server!\n");

    char readBuffer[1000], writeBuffer[1000];
    ssize_t readBytes, writeBytes;
    int userChoice;

    while (1) { // Loop to keep prompting for actions until the connection is terminated
        writeBytes = write(connectionFileDescriptor, INITIAL_PROMPT, strlen(INITIAL_PROMPT));
        if (writeBytes == -1) {
            perror("Error while sending first prompt to the user!");
            break; // Exit if there's an error sending the prompt
        }

        bzero(readBuffer, sizeof(readBuffer));
        usleep(100000);
        readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
        if (readBytes == -1) {
            perror("Error while reading from client");
            break; // Exit on read error
        } else if (readBytes == 0) {
            printf("No data was sent by the client");
            break; // Exit if no data is sent
        } else {
            readBuffer[readBytes] = '\0'; 
            userChoice = atoi(readBuffer);
            switch (userChoice) {
                case 1:
                    if (!admin_operation_handler(connectionFileDescriptor)) {
                        return; 
                    }
                    break;
                case 2:
                    if (!customer_operation_handler(connectionFileDescriptor)) {
                        return; 
                    }
                    break;
                case 3:
                    if (!manager_operation_handler(connectionFileDescriptor)) {
                        continue;
                    }
                    break;
                case 4:
                    if (!employee_operation_handler(connectionFileDescriptor)) {
                        continue;
                    }
                    break;
                case 5:
                    printf("Exiting the application. Goodbye! 👋\n");
                    continue;  
                default:
                    printf("Invalid choice or exit.\n");
                    break;
            }
        }
    }
    printf("Terminating connection to client!\n");
}



// void connection_handler(int connectionFileDescriptor)
// {
//     printf("Client has connected to the server!\n");

//     char readBuffer[1000], writeBuffer[1000];
//     ssize_t readBytes, writeBytes;
//     int userChoice;

//     writeBytes = write(connectionFileDescriptor, INITIAL_PROMPT, strlen(INITIAL_PROMPT));
//     if (writeBytes == -1)
//         perror("Error while sending first prompt to the user!");
//     else
//     {
//         bzero(readBuffer, sizeof(readBuffer));
//         readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
//         if (readBytes == -1)
//             perror("Error while reading from client");
//         else if (readBytes == 0)
//             printf("No data was sent by the client");
//         else
//         {
//             userChoice = atoi(readBuffer);
//             switch (userChoice)
//             {
//             case 1:
//                 // Admin
//                 admin_operation_handler(connectionFileDescriptor);
//                 break;
//             case 2:
//                 // Customer
//                 customer_operation_handler(connectionFileDescriptor);
//                 break;
//             case 3:
//                 manager_operation_handler(connectionFileDescriptor);
//                 break;
//             case 4:
//                 employee_operation_handler(connectionFileDescriptor);
//                 break;
//             default:
//                 // Exit
//                 break;
//             }
//         }
//     }
//     printf("Terminating connection to client!\n");
// }