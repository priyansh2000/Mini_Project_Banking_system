#include <stdio.h>      // For printf, perror, etc.
#include <errno.h>      // For errno variable
#include <fcntl.h>      // For fcntl
#include <unistd.h>     // For system calls
#include <sys/types.h>  // For socket functions
#include <sys/socket.h> // For socket functions
#include <netinet/ip.h> // For sockaddr_in structure
#include <string.h>     // For string manipulation

void connection_handler(int sockFD); // Function to handle communication with server

int main()
{
    int socketFileDescriptor, connectStatus;
    struct sockaddr_in serverAddress;

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0); // Create socket
    if (socketFileDescriptor == -1)
    {
        perror("Error while creating server socket!");
        return 1;
    }

    serverAddress.sin_family = AF_INET;                // IPv4
    serverAddress.sin_port = htons(8081);              // Port number
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to all available interfaces

    // Attempt to connect to the server
    connectStatus = connect(socketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectStatus == -1)
    {
        perror("Error while connecting to server!");
        close(socketFileDescriptor);
        return 1;
    }

    // Handle communication with the server
    connection_handler(socketFileDescriptor);

    // Close the socket after the session
    close(socketFileDescriptor);
    return 0;
}






void connection_handler(int sockFD)
{
    char readBuffer[1000], writeBuffer[1000]; // Buffers for reading and writing
    ssize_t readBytes, writeBytes;            // Bytes read from/written to the socket
    char tempBuffer[1000];

    do
    {
        // Clear buffers
        bzero(readBuffer, sizeof(readBuffer));
        bzero(tempBuffer, sizeof(tempBuffer));

        // Read from socket
        readBytes = read(sockFD, readBuffer, sizeof(readBuffer) - 1); // -1 to leave space for null-terminator
        if (readBytes == -1)
        {
            perror("Error while reading from server socket!");
            break;
        }
        else if (readBytes == 0)
        {
            printf("Connection closed by server.\n");
            break;
        }

        readBuffer[readBytes] = '\0'; // Null-terminate the read buffer

        if (strstr(readBuffer, "type ok") != NULL)
        {
            strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 1);
            printf("%s\n", tempBuffer);
            fgets(writeBuffer, sizeof(writeBuffer), stdin); // Read user input
            writeBytes = write(sockFD, writeBuffer, strlen(writeBuffer)); // Write to server
            if (writeBytes == -1)
            {
                perror("Error while writing to server socket!");
                break;
            }
        }
        else if (strchr(readBuffer, '^') != NULL)
        {
            strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 1);
            printf("%s\n", tempBuffer);
            writeBytes = write(sockFD, "^", strlen("^")); // Send dummy data
            if (writeBytes == -1)
            {
                perror("Error while writing to server socket!");
                break;
            }
        }
        else if (strchr(readBuffer, '$') != NULL)
        {
            strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 2); // Exclude special characters
            printf("%s\n", tempBuffer);
            printf("Closing connection to the server now!\n");
            break;
        }
        else
        {
            bzero(writeBuffer, sizeof(writeBuffer)); // Clear write buffer

            if (strchr(readBuffer, '#') != NULL)
            {
                strcpy(writeBuffer, getpass(readBuffer)); // For password input
            }
            else
            {
                printf("%s\n", readBuffer);               // Show server message
                fgets(writeBuffer, sizeof(writeBuffer), stdin);
                writeBuffer[strcspn(writeBuffer, "\n")] = 0;

            }

            writeBytes = write(sockFD, writeBuffer, strlen(writeBuffer)); // Write to server
            if (writeBytes == -1)
            {
                perror("Error while writing to server socket!");
                printf("Closing connection to the server now!\n");
                break;
            }
        }
    } while (readBytes > 0);

    close(sockFD); // Close the socket at the end
}   

// void connection_handler(int sockFD)
// {
//     char readBuffer[1000], writeBuffer[1000]; // Buffers for reading and writing
//     ssize_t readBytes, writeBytes;            // Bytes read from/written to the socket
//     char tempBuffer[1000];

//     do
//     {
//         // Clear buffers
//         bzero(readBuffer, sizeof(readBuffer));
//         bzero(tempBuffer, sizeof(tempBuffer));

//         // Read from socket
//         readBytes = read(sockFD, readBuffer, sizeof(readBuffer) - 1); // -1 to leave space for null-terminator
//         if (readBytes == -1)
//         {
//             perror("Error while reading from server socket!");
//             break;
//         }
//         else if (readBytes == 0)
//         {
//             printf("Connection closed by server.\n");
//             break;
//         }

//         readBuffer[readBytes] = '\0'; // Null-terminate the read buffer

//         if (strstr(readBuffer, "type ok") != NULL)
//         {
//             strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 1);
//             printf("%s\n", tempBuffer);
//             fgets(writeBuffer, sizeof(writeBuffer), stdin); // Read user input
//             writeBytes = write(sockFD, writeBuffer, strlen(writeBuffer)); // Write to server
//             if (writeBytes == -1)
//             {
//                 perror("Error while writing to server socket!");
//                 break;
//             }
//         }
//         else if (strchr(readBuffer, '^') != NULL)
//         {
//             strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 1);
//             printf("%s\n", tempBuffer);
//             writeBytes = write(sockFD, "^", strlen("^")); // Send dummy data
//             if (writeBytes == -1)
//             {
//                 perror("Error while writing to server socket!");
//                 break;
//             }
//         }
//         else if (strchr(readBuffer, '$') != NULL)
//         {
//             strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 2); // Exclude special characters
//             printf("%s\n", tempBuffer);
//             printf("Closing connection to the server now!\n");
//             break;
//         }
//         else
//         {
//             bzero(writeBuffer, sizeof(writeBuffer)); // Clear write buffer

//             if (strchr(readBuffer, '#') != NULL)
//             {
//                 strcpy(writeBuffer, getpass(readBuffer)); // For password input
//             }
//             else
//             {
//                 printf("%s\n", readBuffer);               // Show server message
//                 fgets(writeBuffer, sizeof(writeBuffer), stdin);
//                 writeBuffer[strcspn(writeBuffer, "\n")] = 0;

//             }

//             writeBytes = write(sockFD, writeBuffer, strlen(writeBuffer)); // Write to server
//             if (writeBytes == -1)
//             {
//                 perror("Error while writing to server socket!");
//                 printf("Closing connection to the server now!\n");
//                 break;
//             }
//         }
//     } while (readBytes > 0);


//     close(sockFD); // Close the socket at the end
// }
