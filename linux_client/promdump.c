/*  Arduino PROMdump - Console client
 *  Nino MegaDriver - nino@nino.com.br
 *  https://megadriver.com.br
 *
 *  GPL v3.0
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <getopt.h>

// START: CRC32 Routines
uint32_t crc32_for_byte(uint32_t r) {
  for(int j = 0; j < 8; ++j)
    r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
  return r ^ (uint32_t)0xFF000000L;
}

void crc32(const void *data, size_t n_bytes, uint32_t* crc) {
  static uint32_t table[0x100];
  if(!*table)
    for(size_t i = 0; i < 0x100; ++i)
      table[i] = crc32_for_byte(i);
  for(size_t i = 0; i < n_bytes; ++i)
    *crc = table[(uint8_t)*crc ^ ((uint8_t*)data)[i]] ^ *crc >> 8;
}
// END: CRC32 Routines

int serial_connected = -1;    // Serial connection latch

// Common arduino ports to try connecting to..
const char* serial_ports[6] = {
	"/dev/ttyUSB0",
	"/dev/ttyUSB1",
	"/dev/ttyUSB2",
	"/dev/ttyACM0",
	"/dev/ttyACM1",
	"/dev/ttyACM2",
};
int serial_port    = 0;
int serial_error   = 0;

// Trigger an error message then quit
void trigger_error(const char* m){
    serial_error = 1;
    if(errno > 0) printf("ERROR %s %s (%d)\n",m, strerror(errno), errno);
    else  printf("ERROR %s\n",m);
    exit(1);
}

// Connect to the Arduino via Serial Port
void serial_connect(){
  int retry = 0;

  // Try known Serial ports
  while(serial_connected == -1 && retry <6){
    serial_port = retry;
    serial_connected = open(serial_ports[retry], O_RDWR);
    retry++;
  }

  if(serial_connected == -1){
    serial_port = -1;
    printf("ERROR: arduino not found.\nTried: \n");
    for(int i=0; i<4; i++) printf("%s\n", serial_ports[i]);
    exit(0);
  }

  printf("Connected to \"%s\"\n", serial_ports[retry-1]);

  struct termios tty;
  int i = tcgetattr(serial_connected, &tty);
  if(i<0){ trigger_error("[1]:"); return;}
  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag |= CS8; // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
  tty.c_lflag &= ~ICANON; // Disable Cannonical Mode
  tty.c_lflag &= ~ECHO; // Disable echo
  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  tty.c_cc[VMIN]  = 1; // Ativa Blocking
  tty.c_cc[VTIME] = 0; // Ativa Blocking
  int j = cfsetispeed(&tty, B115200);
  if(j<0){ trigger_error("[2]:"); return;}
  int k = cfsetospeed(&tty, B115200);
  if(k<0){ trigger_error("[3]:"); return;}
  int l = tcsetattr(serial_connected, TCSANOW, &tty);
  if(l<0) {trigger_error("[4]:"); return;}
}

// Show usage information then quit
void usage(char *fname){
  printf("PROMdump v1.0 - nino@nino.com.br\n");
  printf("For combined use with arduino_promdump.ino\n\n");
  printf("Usage:\n%s -p [prom] -f [file to save dump to] -t [dump type: bin|fuse]\n\n", fname);
  printf("Ex:\n");
  printf("%s -p 82s129 -f mydump.bin -t bin\n", fname);
  printf("%s -p 82s141 -f mydump.txt -t fuse\n\n", fname);
  printf("Supported PROMs:\n");
  printf("82s123, 82s129, 82s141, 6348, 6349, mb7054, mb7123\n\n");
  exit(0);
}

// Main
int main(int argc, char **argv) {
    FILE *fileptr;
    int r=0, col=0;
    char cmd[100];
    unsigned char c[1];
    char dumpfile[100];
    char prom[20];
    int promsize=0;
    uint32_t crc = 0;
    sprintf(dumpfile, "%c", 0x00);

    if (argc==1) {
        usage(argv[0]);
    }


    int option_index = 0, opt;
    static struct option loptions[] = {
        {"help",       no_argument,        0, 'h'},
        {"prom",       required_argument,  0, 'p'},
        {"file",       required_argument,  0, 'f'},
        {"type",       required_argument,  0, 't'},
        {NULL,         0,                  0,  0}
    };


    while(1) {
        opt = getopt_long (argc, argv, "hp:f:t:", loptions, &option_index);
        if (opt==-1) break;
        switch (opt) {
      	  case 'h':
      	      usage(argv[0]);
      	      break;
      	  case 'p':
              strcpy(prom, optarg);
      	      break;
      	  case 'f':
              strcpy(dumpfile, optarg);
              break;
          case 't':
              // If you added more PROMs to arduino_promdump.ino
              // configure all the sizes here
              if     (strcmp(prom,"82s123") == 0) promsize = 32;
              else if(strcmp(prom,"82s129") == 0) promsize = 256;
              else if(strcmp(prom,"82s141") == 0) promsize = 512;
              else if(strcmp(prom,"mb7123") == 0) promsize = 512;
              else if(strcmp(prom,"6348")   == 0) promsize = 512;
              else if(strcmp(prom,"6349")   == 0) promsize = 512;
              else if(strcmp(prom,"mb7054") == 0) promsize = 1024;
              if(promsize == 0) trigger_error("[5]: Unknown prom...");
              printf("Selected PROM \"%s\", size %d bytes...\n", prom, promsize);
              if(strcmp(dumpfile,"") == 0) trigger_error("[6]: You must specify a file to save dump to...");
              printf("Will save dump to file: %s\n", dumpfile);
              if(strcmp(optarg,"bin") == 0) sprintf(cmd, "bin %s\n", prom);
              else if(strcmp(optarg,"fuse") == 0) sprintf(cmd, "fuse %s\n", prom);
              else trigger_error("[7]: You must specify the dump type, either \"bin\" or \"fuse\"");
              // Ok, let's start the Serial Tasks
              serial_connect();
      	      sleep(2); // Allow some time to connect
              fileptr = fopen(dumpfile, "w");
              if(!fileptr) trigger_error("7: ");
              c[0] = 0x01;
              while(c[0] != 0x00){ // Read Welcome message, until sync char = 0x00
                r = read(serial_connected, &c, 1);
                printf("%c", c[0]);
              }
              int address = 0;
              col   = 0;
              crc = 0;
              printf("==> %s",cmd); // Print the cmd for the user to see
              r = write(serial_connected, &cmd, 100);
              sleep(2);
              // Dumping starts...
              while(1){
                r = read(serial_connected, &c, 1);
                if(r < 0) trigger_error("[8]:");
                else if(r == 0) trigger_error("[9]:");
                else if(r == 1) {
                  crc32(c, 1, &crc);
                  fwrite(c , 1 ,1 , fileptr );
                  if(strcmp(optarg,"bin") == 0) {
                      if(col==0) printf("0x%06X:", address);
                      printf("0x%02x ", c[0]);
                      col++;
                      if(col>15) {
                        printf("\n");
                        col=0;
                      }
                      address++;
                      if(address >= promsize){
                        if(col<16) printf("\n");
                        printf("CRC32: %08X\n", crc);
                        exit(0);
                      }
                  }else if(strcmp(optarg,"fuse") == 0) {
                      if(c[0] == 0x21) {
                        printf("!\n");
                        exit(0);
                      }
                      else printf("%c", c[0]);
                  }
                }
              }
              break;
            }
    }
    return 0;
}
