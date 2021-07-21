#ifndef SYSTEM_INFO
#define SYSTEM_INFO

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <time.h>

#define IP_SIZE NI_MAXHOST

struct rate_t{
  long totalDownload;
  long totalUpload;
  double download;
  double upload;
};

class SystemInfo
{
public:
  double cpuUsage();
  int cpuTemp();
  double memUsage();
  void interfaceIp(char * name, char * buffer);
  struct rate_t interfaceIORates(char * name);
private:
  long _cpuLastSum = 0;
  long _cpuLastIdle = 0;
  struct cpu_t
  {
    char name[4];
    long user;
    long nice;
    long system;
    long idle;
  } _cpu;
  struct mem_t
  {
    long total;
    long free;
    long avail;
    long buf;
    long cached;
  } _mem;
  struct rate_t _rates;
  struct timespec _time;
};

#endif