#include "Wifi.h"
//#include "Arduino.h"

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

int fileDescriptor;
int localFileDescriptor;
struct sockaddr_in remoteAddress;       //dont send to these
socklen_t          remoteAddressLength; //
unsigned short     remotePort;          //

//struct sockaddr_in sendAddress;       //dont send to these
//socklen_t          sendAddressLength; //
//unsigned short     sendPort;          //

#define false 0
#define true (!false)


int bindPort(int fd, char* ipAddress, int port);

/*-----------------------------------------------------*/
void wiSetup()
{
  int port = 12000;
  int error;

  //if(fileDescriptor != -1) this->Disconnect();

  error = fileDescriptor = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(error == -1) perror("UDPCommunication.c - unable to create a UDP socket\n");

  if(error != -1)
    {
      remotePort = port;
      error = bindPort(fileDescriptor, (char*)"0.0.0.0", port);
      if(error == -1) perror("UDPCommunication.c - unable to bind\n"); 
      else
        {
          int b = 1;
          error = setsockopt(fileDescriptor, SOL_SOCKET, SO_BROADCAST, &b, sizeof(b));
        }
    }
    
  //return (error == -1) ? false : true;
}  

/*-----------------------------------------------------*/
int wiWrite(char* buffer, int bufferSize)
{
  /*
  printf("WI_WRITE\n");
  int i;
  for(i=0; i<bufferSize; i++)
    {
      printf("%i: %c\t%02X\t%hhu\n", i, (char)buffer[i], (unsigned char)buffer[i], (unsigned char)buffer[i]);
    }
  */
  //remoteAddress.sin_port = htons(7400);
  
  remoteAddressLength = sizeof(remoteAddress);
  memset(&remoteAddress, 0, remoteAddressLength);
  remoteAddress.sin_family      = AF_INET;
  remoteAddress.sin_port        = htons(12000);
  //remoteAddress.sin_addr.s_addr = inet_addr("255.255.255.255");
  remoteAddress.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  
  int error = sendto(fileDescriptor, buffer, bufferSize, 0, (struct sockaddr *)&remoteAddress, remoteAddressLength);
  if(error == -1) perror("UDPCommunication.c - unable to send");
  return error;
}

/*-----------------------------------------------------*/

int wiRead(char* buffer, int bufferSize, int minBytes)
{
  return recvfrom(fileDescriptor, buffer, bufferSize, 0, (struct sockaddr *)&remoteAddress, &remoteAddressLength);
}

int bindPort(int fd, char* ipAddress, int port)
{
  struct sockaddr_in thisAddress;
  memset(&thisAddress, 0, sizeof(thisAddress));
  thisAddress.sin_family      = AF_INET;
  thisAddress.sin_port        = htons(port);
  thisAddress.sin_addr.s_addr = htonl(inet_addr(ipAddress));
  
  int error = bind(fd, (struct sockaddr*)&thisAddress, sizeof(thisAddress));
  return error;
}

