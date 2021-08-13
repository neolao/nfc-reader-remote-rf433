
#include "./rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
     
RCSwitch mySwitch;

int main(int argc, char *argv[]) {
  
     // This pin is not the first pin on the RPi GPIO header!
     // Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
     // for more information.
     int PIN = 2;
     
     if(wiringPiSetup() == -1) {
       printf("wiringPiSetup failed, exiting...");
       return 0;
     }

     int pulseLength = 0;
     if (argv[1] != NULL) pulseLength = atoi(argv[1]);

     mySwitch = RCSwitch();
     if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
     mySwitch.enableReceive(PIN);  // Receiver on interrupt 0 => that is pin #2
     

     // states:
     // 0 => idle
     // 1 => start message
     // 2 => waiting message part
     // 3 => message part saved
     // 4 => message complete
     int state = 0;
     int message[10];
     int messagePartIndex = 0;

     while(1) {
      if (mySwitch.available()) {
        int value = mySwitch.getReceivedValue();
        unsigned int* rawData = mySwitch.getReceivedRawdata();
    
        if (value == 0) {
          printf("Unknown encoding\n");
        } else {    
          long value = mySwitch.getReceivedValue();   
          //printf("Value:   %i\n", value);

          switch (value) {
            case 555:
              // start
              if (state == 0 || state == 4) {
                std::fill( std::begin( message ), std::end( message ), 0 );
                messagePartIndex = 0;
                state = 1;
                printf("START ");
              }
              break;
       
            case 556:
              // end
              if (state == 3) {
                state = 4;
                
                char messageString[100];
                for (int i = 0; i < messagePartIndex; i++) {
                  char hexa[32];
                  sprintf(hexa, "%02x", message[i]);

                  if (i == 0) {
                    strcpy(messageString, hexa);
                  } else {
                    strcat(messageString, hexa);
                  }
                  printf("%s ", hexa);
                } 
                //printf("%s END\n", messageString);
                printf("END\n");

                char command[150];
                sprintf(command, "curl -d '%s' http://sound-box.local:8080/", messageString);
                printf("Command: %s\n", command);
                system(command);
                usleep(50000);
              }
              break;

            case 553:
              if (state == 1 || state == 3) {
                state = 2;
              }
              break;

            default:
              if (state == 2) {
                message[messagePartIndex] = value;
                messagePartIndex++;
                state = 3;
              }
          }


          /*
          char hexa[32];
          if ( value % 2 == 0 ) {
            sprintf(hexa, "%x", value);
          } else {
            sprintf(hexa, "0%x", value);
          }
          printf("Hexa:    %s\n", hexa);

          char command[150];
          sprintf(command, "curl -d '%s' http://sound-box.local:8080/", hexa);
          printf("Command: %s\n", command);
          //system(command);
          //usleep(50000);
          */
        }
    
        fflush(stdout);
        mySwitch.resetAvailable();
      }
      usleep(100); 
  }

  exit(0);
}
