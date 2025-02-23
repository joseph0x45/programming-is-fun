#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void memory_get() {

  FILE *fp = fopen("/proc/meminfo", "r");
  if (fp == NULL) {
    perror("/proc/meminfo");
    return;
  }

  char sline[64];

  while (fgets(sline, sizeof sline, fp) != NULL) {
    char *p = strstr(sline, "MemFree");
    if (p == NULL) {
      perror("strtsr");
      continue;
    }
    printf("tentative de récupératon du memfree\n");
    while (!isspace(*p) && *p) {
      p++;
    }
    if (*p) {
      unsigned long memfree = strtoul(p, NULL, 10);
      printf("memfree = %lu kB\n", memfree);
    }
    break;
  }
  fclose(fp);
  fp = NULL;
}

float CPU_get() {
  int pre_sommeI;
  int sommeI;
  int pre_sommeUNSI;
  int sommeUNSI;
  int diff_sommeI;
  int diff_sommeUNSI;
  int calcul_utilisation_cpu;
  int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  int user1, nice1, system1, idle1, iowait1, irq1, softirq1, steal1, guest1,
      guest_nice1;
  FILE *fp = fopen("/proc/stat", "r");
  if (!fp) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  fscanf(fp, "cpu %d %d %d %d %d %d %d %d %d %d", &user, &nice, &system, &idle,
         &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

  printf("cpu %d %d %d %d %d %d %d %d %d %d\n", user, nice, system, idle,
         iowait, irq, softirq, steal, guest, guest_nice);

  rewind(fp);
  sleep(1);

  fscanf(fp, "cpu %d %d %d %d %d %d %d %d %d %d", &user1, &nice1, &system1,
         &idle1, &iowait1, &irq1, &softirq1, &steal1, &guest1, &guest_nice1);

  printf("cpu %d %d %d %d %d %d %d %d %d %d\n", user1, nice1, system1, idle1,
         iowait1, irq1, softirq1, steal1, guest1, guest_nice1);

  int time_idle = idle + iowait;
  int time_non_idle = user + nice + system + irq + softirq + steal;
  int time_total = time_idle + time_non_idle;

  int time_idle1 = idle1 + iowait1;
  int time_non_idle1 = user1 + nice1 + system1 + irq1 + softirq1 + steal1;
  int time_total1 = time_idle1 + time_non_idle1;

  int total_diff = time_total1 - time_total;
  int idle_time = time_idle1 - time_idle;
  float percentage = (float)(total_diff - idle_time) / total_diff;
  return percentage * 100;
}

int main() {
  pid_t pid = fork();

  if (pid == -1) {
    printf("Erreur fork\n");
    exit(1);
  }

  if (pid == 0) {
    /* memory_get(); */
    float v = CPU_get();
    printf("%f", v);
    exit(0);
  } else {
    wait(NULL);
  }
}
