//
// Created by Will Walker on 2019-05-07.
//

// Imported libraries used for socket communication
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>

// Globally defined constants
#define IP_PROTOCOL 0
#define BUFFER_SIZE 2560
#define FLAG 0
#define ACK_PREFIX "! - ACK:"

// Globally defined variables
char *serverIPAddress;
int clientPortNumber;
int socketDescriptor;
struct sockaddr_in socketAddress;
socklen_t socketAddressLength = sizeof(socketAddress);
char messageBuffer[BUFFER_SIZE];
FILE *downloadedFile;
char fileRequest[BUFFER_SIZE];
char checksum;
struct timeval tv;
int timeout = 1;

// Functions
void checkArguments(int argumentCount, char **arguments);

void createSocket();

void makeRequest();

void waitForResponse();

void generateChecksum();

void sendACK();

/*
 * Check Arguments
 * ---
 * Check the arguments passed into the
 * program to ensure that the proper
 * amount of variables are entered.
 */
void checkArguments(int argumentCount, char **arguments) {
    if (argumentCount < 4) {
        printf("usage: server <server ip> <port number> <window size>\n");
        exit(0);
    }

    serverIPAddress = arguments[1];
    clientPortNumber = atoi(arguments[2]);
    int windowSizeN = atoi(arguments[3]);

    printf("Client program started. Using window size %i. Communicating with %s:%i\n", windowSizeN, serverIPAddress, clientPortNumber);
}

/*
 * Create Socket
 * ---
 * Create a socket using the port
 * number that was passed in as
 * an argument.
 */
void createSocket() {
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(clientPortNumber);
    socketAddress.sin_addr.s_addr = inet_addr(serverIPAddress);
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    setsockopt(socketDescriptor, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (socketDescriptor < 0)
        printf("Socket creation failed!\n");
    else
        printf("Socket created successfully\n");
}

/* Make Request
 * ---
 * Wait for a file request from
 * a prospective client connection.
 * Once a request has been made,
 * log the requested file name.
 */
void makeRequest() {
    memset(messageBuffer, 0, BUFFER_SIZE);
    printf("Enter a file request: ");
    scanf("%s", fileRequest);
    strcpy(messageBuffer, fileRequest);

    sendto(socketDescriptor, messageBuffer, BUFFER_SIZE, FLAG,
           (struct sockaddr *) &socketAddress, socketAddressLength);

    printf("Requested: %s\n", messageBuffer);
}

/* Wait For Response
 * ---
 * Wait for a response
 * from the server.
 */
void waitForResponse() {
    ssize_t bytesReceived;

    printf("Waiting for the server...\n");

    while (true) {
        memset(messageBuffer, 0, BUFFER_SIZE);

        bytesReceived = recvfrom(socketDescriptor, messageBuffer, BUFFER_SIZE, FLAG,
                                 (struct sockaddr *) &socketAddress, &socketAddressLength);

        if (bytesReceived > 0) {
            downloadedFile = fopen(fileRequest, "wb+");
            if (messageBuffer[0] == '!') {
                printf("Server says: %s\n", messageBuffer);
                fclose(downloadedFile);
                remove(fileRequest);
                timeout = 0;
                break;
            } else {
                fwrite(messageBuffer, 1, static_cast<size_t>(bytesReceived), downloadedFile);
                fclose(downloadedFile);
                printf("File downloaded\n");
                generateChecksum();
                sendACK();
                timeout = 0;
                break;
            }
        }

        timeout++;
        if (timeout > 30) {
            makeRequest();
        }
    }
}

/* Verify Response
 * ---
 * Verify the response
 * from the server using
 * the checksum algorithm.
 */
void generateChecksum() {
    checksum = 1;
    FILE *file = fopen(fileRequest, "r");
    while (!feof(file) && !ferror(file)) {
        checksum += fgetc(file);
    }
    printf("File checksum: %d\n", checksum);
    fclose(file);
}

/* Send ACK
 * ---
 * Send an ACK to the server.
 */
void sendACK() {
    memset(messageBuffer, 0, BUFFER_SIZE);
    strcpy(messageBuffer, ACK_PREFIX);
    messageBuffer[sizeof(ACK_PREFIX) + 1] = checksum;

    sendto(socketDescriptor, messageBuffer, BUFFER_SIZE, FLAG,
           (struct sockaddr *) &socketAddress, socketAddressLength);
    printf("ACK Sent\n");
}

/* Main Function
 * ---
 * Sets up the socket connection,
 * listens for file requests,
 * serves up files if they are available.
 */
int main(int argumentCount, char **arguments) {
    checkArguments(argumentCount, arguments);
    createSocket();
    makeRequest();
    while (true) {
        waitForResponse();
    }
}