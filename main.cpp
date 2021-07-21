#include <stdio.h>
#include <signal.h>
#include "SSD1306.h"
#include "system_info.h"
#include <wiringPi.h>
#include <time.h>

#define BUTTON_PIN 1
#define SCREEN_ON_TIME 10

volatile sig_atomic_t stop = false;
struct sigaction action;

SSD1306 oled((char *)"/dev/i2c-0", 0x3C);


void sig_handler(int signo);
void lightUpScreen();

int main()
{
  printf("Starting...\n");
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = sig_handler;
  // Intercept SIGHUP and SIGINT
  if (sigaction(SIGINT, &action, NULL) == -1)
  {
    perror("Error: cannot handle SIGINT"); // Should not happen
  }
  if (sigaction(SIGTERM, &action, NULL) == -1)
  {
    perror("Error: cannot handle SIGTERM"); // Should not happen
  }

  if (wiringPiSetup() == -1)
  {
    perror("Can't inicialize WirinfPi!");
    exit(EXIT_FAILURE);
  }

  pinMode(BUTTON_PIN, INPUT);
  pullUpDnControl(BUTTON_PIN, PUD_UP);


  lightUpScreen();
  while (!stop)
  {
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      lightUpScreen();
    }else{
      usleep(300000);
    }
  }
  oled.stop();
  printf("\rExiting...\n");
  return 0;
}

void sig_handler(int sig)
{
  stop = true;
  printf("%d\rTurning off the screen...\n",sig);
}

void lightUpScreen()
{
  time_t timeOn = time(NULL) + SCREEN_ON_TIME;
  SystemInfo info;
  oled.turnOn();
    printf("Turn On...\n");
  while (time(NULL) < timeOn && !stop)
  {
    oled.clearScreen();
    char buff[IP_SIZE + 7];
    sprintf(buff, "CPU:  %.2f%%  %dC", info.cpuUsage(), info.cpuTemp());
    oled.drawString(0, 0, buff, 1, WHITE);
    sprintf(buff, "MEM:  %.2f%%", info.memUsage());
    oled.drawString(0, 9, buff, 1, WHITE);
    char ip[IP_SIZE];
    info.interfaceIp((char *)"wlan0", ip);
    sprintf(buff, "IP:   %s", ip);
    oled.drawString(0, 27, buff, 1, WHITE);
    rate_t rates = info.interfaceIORates((char *)"wlan0");
    sprintf(buff, "UP: %.2f  DOW: %.2f", rates.upload, rates.download);
    oled.drawString(0, 36, buff, 1, WHITE);
    oled.refresh();
    usleep(500000);
  }
  printf("Turn Off...\n");
  oled.turnOff();
}