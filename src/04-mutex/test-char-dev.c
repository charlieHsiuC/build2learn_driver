#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define DEV_PATH "/dev/build2learn_char"
#define W_BUF_LEN 16
#define R_BUF_LEN 16
#define WRITER_CNT 8
#define LOOP_CNT 20

static void writer(char c) {
  char buf[W_BUF_LEN];
  int fd;
  ssize_t n;

  fd = open(DEV_PATH, O_WRONLY);
  if (fd < 0) {
    perror("write open fail");
    exit(1);
  }

  memset(buf, c, sizeof(buf));
  n = write(fd, buf, sizeof(buf));
  close(fd);

  if (n != sizeof(buf)) {
    perror("write fail");
    exit(1);
  } else {
    exit(0);
  }
}

static int reader() {
  char buf[R_BUF_LEN];
  int fd;
  ssize_t n;
  int is_good = 1;

  for (int i = 0; i < LOOP_CNT; i++) {
    fd = open(DEV_PATH, O_RDONLY);
    if (fd < 0) {
      perror("read open fail");
      return -1;
    }

    n = read(fd, buf, sizeof(buf));
    close(fd);

    if (n < 0) {
      perror("read fail");
      return -1;
    }

    int bad = 0;
    for (int j = 0; j < n - 1; j++) {
      bad |= (buf[j] != buf[j + 1]);
    }

    if (bad) {
      printf("%.*s\n", (int)n, buf);
    }

    is_good &= !bad;
  }

  return is_good;
}

int main() {
  for (int i = 0; i < WRITER_CNT; i++) {
    if (fork() == 0) {
      writer('A' + i);
    }
  }

  int is_good = reader();

  while (wait(NULL) > 0)
    ;

  if (is_good < 0) {
    perror("read problem");
    return -1;
  }

  printf("%s\n", is_good ? "PASS\n" : "FAIL\n");

  return is_good == 0;
}