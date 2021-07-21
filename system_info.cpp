#include "system_info.h"

double SystemInfo::cpuUsage()
{

  FILE *fp = fopen("/proc/stat", "r");
  int status = fscanf(fp, "%s %ld %ld %ld %ld", _cpu.name, &_cpu.user, &_cpu.nice, &_cpu.system, &_cpu.idle);
  if(status == 0){
    fprintf(stderr,"Fail to open /proc/stat\n");
    return 0.0;
  }
  fclose(fp);
  // sum all of the values
  long sum = _cpu.user + _cpu.nice +
             _cpu.system + _cpu.idle;

  // collect the difference between sum and the last sum
  long cpu_delta = sum - _cpuLastSum;
  // collect idle time
  long cpu_idle = _cpu.idle - _cpuLastIdle;
  // delta minus ide time
  long cpu_used = cpu_delta - cpu_idle;
  // percentage of utilization
  double result = (100.0 * cpu_used) / cpu_delta;

  _cpuLastSum = sum;
  _cpuLastIdle = _cpu.idle;

  return result;
}

double SystemInfo::memUsage()
{
  FILE *fp = fopen("/proc/meminfo", "r");
  int status = fscanf(fp, "MemTotal: %ld kB\nMemFree: %ld kB\nMemAvailable: %ld kB\nBuffers: %ld kB\nCached: %ld kB", &_mem.total, &_mem.free, &_mem.avail, &_mem.buf, &_mem.cached);
  if(status == 0){
    fprintf(stderr,"Fail to open /proc/meminfo\n");
    return 0.0;
  }
  fclose(fp);
  double result = (float)(_mem.free + _mem.buf + _mem.cached) / _mem.total * 100;
  return result;
}

int SystemInfo::cpuTemp()
{
  FILE *fp = fopen("/sys/devices/virtual/thermal/thermal_zone0/temp", "r");
  long temperature = 0;
  int status = fscanf(fp, "%ld", &temperature);
  if(status == 0){
    fprintf(stderr,"Fail to open /sys/devices/virtual/thermal/thermal_zone0/temp\n");
    return 0;
  }
  fclose(fp);
  int result = temperature / 1000;
  return result;
}

void SystemInfo::interfaceIp(char *name, char * buffer)
{
  struct ifaddrs *ifaddr;
  if (getifaddrs(&ifaddr) == -1)
  {
    fprintf(stderr,"Fail to get network interfaces list!\n");
    return;
  }

  bool found = false;
  for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
  {
    if (ifa->ifa_addr == NULL || (ifa->ifa_addr->sa_family != AF_INET && ifa->ifa_addr->sa_family != AF_INET6))
      continue;
    if (strcmp(ifa->ifa_name, name) == 0)
    {
      ifaddr = ifa;
      found = true;
      break;
    }
  }
  if (!found){
    fprintf(stderr,"Fail to found the specified network interface!\n");
    return;
  }
  
  int s = getnameinfo(ifaddr->ifa_addr,
                      (ifaddr->ifa_addr->sa_family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                      buffer, NI_MAXHOST,
                      NULL, 0, NI_NUMERICHOST);
  
  if (s != 0)
  {
    fprintf(stderr, "Fail to get interface IP: %s\n", gai_strerror(s));
    return;
  }
}

struct rate_t SystemInfo::interfaceIORates(char * name){
  struct ifaddrs *ifaddr;
  if (getifaddrs(&ifaddr) == -1)
  {
    fprintf(stderr,"Fail to get network interfaces list!\n");
    return _rates;
  }

  bool found = false;
  for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
  {
    if (ifa->ifa_addr == NULL || (ifa->ifa_addr->sa_family != AF_PACKET))
      continue;
    if (strcmp(ifa->ifa_name, name) == 0)
    {
      ifaddr = ifa;
      found = true;
      break;
    }
  }
  if (!found){
    fprintf(stderr,"Fail to found the specified network interface\n");
    return _rates;
  }

  struct rtnl_link_stats *stats = (struct rtnl_link_stats *)ifaddr->ifa_data;
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC_RAW,&now);
  uint32_t passed = (now.tv_sec - _time.tv_sec) * 1000 + (now.tv_nsec - _time.tv_nsec)/1000000;

  _rates.download = stats->rx_bytes-_rates.totalDownload;
  _rates.upload = stats->tx_bytes-_rates.totalUpload;
  _rates.download /= 0.001 * passed;
  _rates.upload /= 0.001 * passed;
  _rates.download /= 1024;
  _rates.upload /= 1024;

  _rates.totalDownload = stats->rx_bytes;
  _rates.totalUpload = stats->tx_bytes;

  memcpy(&_time, &now, sizeof(struct timeval));
  return _rates;
}