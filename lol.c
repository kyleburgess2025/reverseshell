#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <errno.h>

int main() {
    system("curl --output sound.wav https://www.wavsource.com/snds_2020-10-01_3728627494378403/sfx/ahem_x.wav");
    system("afplay sound.wav");
    return 1;
}