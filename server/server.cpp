//
// Created by Will Walker on 2019-05-07.
//

// Imported libraries used for socket communication
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <random>

// Globally defined constants
#define IP_PROTOCOL 0
#define BUFFER_SIZE 2560
#define FLAG 0
#define NO_FILE "! - File does not exist"
#define ACK_PREFIX "! - ACK:"

// Globally defined variables
int serverPortNumber;
int socketDescriptor;
struct sockaddr_in socketAddress;
socklen_t socketAddressLength = sizeof(socketAddress);
char messageBuffer[BUFFER_SIZE];
char fileRequest[BUFFER_SIZE];
char ACK;
FILE *requestedFile;
char checksum;
int errorProbability;

// Functions
void flipBits();

void checkArguments(int argumentCount, char **arguments);

void createSocket();

void bindSocket();

void waitForRequest();

void sendResponse();

void generateChecksum();

bool compareChecksum();

/* Flip Bits
 * ---
 * Before a packet is sent to the client, you flip a random coin.
 * With probability p, you flip some bits in the packet, where
 * p ∈ (0, 10)
 */
void flipBits() {
    bool flipBits = (rand() % 100) < errorProbability;
    if (flipBits) {
        messageBuffer[rand() % sizeof(requestedFile)] = '&';
        messageBuffer[rand() % sizeof(requestedFile)] = '&';
        messageBuffer[rand() % sizeof(requestedFile)] = '&';
    }
}

/*
 * Check Arguments
 * ---
 * Check the arguments passed into the
 * program to ensure that the proper
 * amount of variables are entered.
 */
void checkArguments(int argumentCount, char **arguments) {
    if (argumentCount < 4) {
        printf("usage: server <port number> <error probability> <window size>\n");
        exit(0);
    }

    serverPortNumber = atoi(arguments[1]);
    errorProbability = atoi(arguments[2]);
    int windowSizeN = atoi(arguments[3]);
    printf("Server program started. Using window size %i. Listening on port %i\n", windowSizeN, serverPortNumber);
    printf("There is a %i%% chance of bit errors\n", errorProbability);
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
    socketAddress.sin_port = htons(serverPortNumber);
    socketAddress.sin_addr.s_addr = INADDR_ANY;

    socketDescriptor = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);

    if (socketDescriptor < 0)
        printf("Socket creation failed!\n");
    else
        printf("Socket created successfully\n");
}

/* Bind Socket
 * ---
 * Bind the newly created socket to the
 * system and allow communication to take
 * place.
 */
void bindSocket() {
    if (bind(socketDescriptor, (struct sockaddr *) &socketAddress, socketAddressLength) == 0)
        printf("Bound to port %i\n", serverPortNumber);
    else
        printf("Binding Failed!\n");
}

/* Wait For Request
 * ---
 * Wait for a file request from
 * a prospective client connection.
 * Once a request has been made,
 * log the requested file name.
 */
void waitForRequest() {
    int bytesReceived;

    while (true) {
        memset(messageBuffer, 0, BUFFER_SIZE);
        printf("Waiting for client...\n");

        bytesReceived = static_cast<int>(recvfrom(socketDescriptor, messageBuffer, BUFFER_SIZE, FLAG,
                                                  (struct sockaddr *) &socketAddress, &socketAddressLength));

        if (bytesReceived > 0) {
            if (messageBuffer[0] == '!') {
                ACK = messageBuffer[sizeof(ACK_PREFIX) + 1];
                printf("Received ACK from client\n");
                if (compareChecksum()) {
                    printf("Client received file successfully!\n");
                    break;
                } else {
                    printf("ACK is garbled, resending...\n");
                    sendResponse();
                }
            } else {
                printf("Client requested: %s\n", messageBuffer);
                strcpy(fileRequest, messageBuffer);
                sendResponse();
                break;
            }
        }
    }
}

/* Send Response
 * ---
 * Send the client a response
 * in reply to their request for
 * a file download.
 */
void sendResponse() {
    requestedFile = fopen(fileRequest, "rb+");
    int fileSize;
    if (requestedFile == nullptr) {
        printf("File does not exist!\n");
        fclose(requestedFile);
        strcpy(messageBuffer, NO_FILE);
        sendto(socketDescriptor, messageBuffer, strlen(NO_FILE),
               FLAG,
               (struct sockaddr *) &socketAddress, socketAddressLength);
    } else {
        printf("File opened successfully\n");
        generateChecksum();
        while ((fileSize = static_cast<int>(fread(messageBuffer, 1, BUFFER_SIZE, requestedFile))) > 0) {
            flipBits();
            sendto(socketDescriptor, messageBuffer, static_cast<size_t>(fileSize),
                   FLAG,
                   (struct sockaddr *) &socketAddress, socketAddressLength);
        }
        printf("File sent\n");
        fclose(requestedFile);
    }
}

/* Generate Checksum
 * ---
 * Generate a file checksum via
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

/* Compare Checksum
 * ---
 * Takes checksum from client
 * and compares with locally
 * generated checksum to confirm
 * delivery.
 */
bool compareChecksum() {
    printf("Comparing checksum values...\n");
    printf("Client's checksum: %d\n", ACK);
    return (ACK == checksum);
}

/* Main Function
 * ---
 * Sets up the socket connection,
 * listens for file requests,
 * serves up files if they are available.
 */
int main(int argumentCount, char **arguments) {
    srand(static_cast<unsigned int>(time(nullptr)));
    checkArguments(argumentCount, arguments);
    createSocket();
    bindSocket();

    while (true) waitForRequest();
}