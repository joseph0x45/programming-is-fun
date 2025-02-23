#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#define MAX_LINE_SIZE 30
#define MAX_CPU_INFO_LINE_SIZE 51
#define MAX_DIGIT_COUNT 9

typedef struct CPUInfo {
  int user;
  int nice;
  int system;
  int idle;
  int iowait;
  int irq;
  int softirq;
  int steal;
  int guest;
  int guest_nice;
} cpu_info_t;

void print_cpu_info(cpu_info_t *i) {
  printf("user: %d\n", i->user);
  printf("nice: %d\n", i->nice);
  printf("system: %d\n", i->system);
  printf("idle: %d\n", i->idle);
  printf("iowait: %d\n", i->iowait);
  printf("irq: %d\n", i->irq);
  printf("softirq: %d\n", i->softirq);
  printf("steal: %d\n", i->steal);
  printf("guest: %d\n", i->guest);
  printf("guest_nice: %d\n", i->guest_nice);
}

int get_mem_info() {
  FILE *fd = fopen("/proc/meminfo", "r");
  int mem_free;
  if (fd == NULL) {
    perror("Failed to read /proc/meminfo:");
    return -1;
  }
  char result[MAX_LINE_SIZE];
  while (fgets(result, sizeof(result), fd) != NULL) {
    if (strstr(result, "MemFree: ") != NULL) { // line contains MemFree
      char mem_free_str[MAX_DIGIT_COUNT];
      mem_free_str[8] = '\0';
      size_t len = strlen(result);
      int found_digits = 0;
      for (int i = 0; i < len; i++) {
        if (isdigit(result[i])) {
          mem_free_str[found_digits] = result[i];
          found_digits++;
        }
      }
      mem_free = atoi(mem_free_str);
    }
  }
  fclose(fd);
  return mem_free;
}

int get_cpu_info() {
  FILE *fd = fopen("/proc/stat", "r");
  if (fd == NULL) {
    perror("failed to read cpu info:");
    return -1;
  }
  cpu_info_t info_t0;
  cpu_info_t info_t1;
  int num_matched;
  char result[MAX_CPU_INFO_LINE_SIZE];
  fgets(result, sizeof(result), fd);
  printf("Line read: %s\n", result);
  num_matched = sscanf(
      result, "cpu %d %d %d %d %d %d %d %d %d %d", &info_t0.user, &info_t0.nice,
      &info_t0.system, &info_t0.idle, &info_t0.iowait, &info_t0.irq,
      &info_t0.softirq, &info_t0.steal, &info_t0.guest, &info_t0.guest_nice);
  sleep(1);
  rewind(fd);
  fgets(result, sizeof(result), fd);
  printf("Line read: %s\n", result);
  num_matched = sscanf(
      result, "cpu %d %d %d %d %d %d %d %d %d %d", &info_t1.user, &info_t1.nice,
      &info_t1.system, &info_t1.idle, &info_t1.iowait, &info_t1.irq,
      &info_t1.softirq, &info_t1.steal, &info_t1.guest, &info_t1.guest_nice);
  print_cpu_info(&info_t0);
  printf("\n\n");
  print_cpu_info(&info_t1);
  int t0_idle = info_t0.idle + info_t0.iowait;
  int t0_non_idle = info_t0.user + info_t0.nice + info_t0.system + info_t0.irq +
                    info_t0.softirq + info_t0.steal;
  int t0_total = t0_idle + t0_non_idle;
  int t1_idle = info_t1.idle + info_t1.iowait;
  int t1_non_idle = info_t1.user + info_t1.nice + info_t1.system + info_t1.irq +
                    info_t1.softirq + info_t1.steal;
  int t1_total = t1_idle + t1_non_idle;

  int total_difference = t1_total - t0_total;
  int idle_difference = t1_idle - t0_idle;
  float cpu_percentage =
      (float)(total_difference - idle_difference) / total_difference;
  printf("CPU USAGE: %f\n", cpu_percentage * 100);
  return 0;
}

int main() {
  pid_t pid = fork();
  if (pid == -1) {
    perror("failed to create child process:");
    return -1;
  };
  FILE *fd;
  if (pid == 0) {
    // inside the child process
    while (1) {
      int mem_free = get_mem_info();
      printf("Free Memory: %d Kb\n", mem_free);
      int cpu_info = get_cpu_info();
      printf("CPU usage: %d %%\n", cpu_info);
      fd = fopen("./stop", "r");
      if (fd) {
        fclose(fd);
        break;
      }
    }
  }
  // inside the parent process
  printf("Waiting for child");
  wait(NULL);
  return 0;
}
