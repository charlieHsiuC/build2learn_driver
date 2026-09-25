#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DEV_PATH "/dev/build2learn_char"
#define BUF_LEN 128

static int do_write(const char *msg) {
  int fd;
  ssize_t n;

  fd = open(DEV_PATH, O_WRONLY);
  if (fd < 0) {
    perror("open write");
    return 1;
  }

  n = write(fd, msg, strlen(msg));

  if (n < 0) {
    perror("write");
    close(fd);
    return 1;
  }

  printf("wrote %zd bytes: \"%.*s\"\n", n, (int)n, msg);

  close(fd);
  return 0;
}

static int do_read(void) {
  int fd;
  ssize_t n;
  char buf[BUF_LEN];

  fd = open(DEV_PATH, O_RDONLY);
  if (fd < 0) {
    perror("open read");
    return 1;
  }

  printf("read bytes to filled buf at once\n");

  n = read(fd, buf, sizeof(buf));

  if (n < 0) {
    perror("read");
    close(fd);
    return 1;
  }

  printf("first read %zd bytes: \"%.*s\"\n", n, (int)n, buf);

  n = read(fd, buf, sizeof(buf));
  if (n < 0) {
    perror("read again");
    close(fd);
    return 1;
  }
  printf("second read: %zd bytes (expect 0)\n", n);

  close(fd);
  return 0;
}

static int do_read_byte(void) {
  int fd;
  ssize_t n = 1;
  char buf[BUF_LEN];
  size_t idx = 0;

  fd = open(DEV_PATH, O_RDONLY);
  if (fd < 0) {
    perror("open read");
    return 1;
  }

  printf("read one byte each time\n");
  while (n > 0 && idx < BUF_LEN) {
    n = read(fd, buf + (idx++), 1);
  }

  if (n < 0) {
    perror("read byte");
    close(fd);
    return 1;
  }

  printf("read %zd bytes: \"%.*s\"\n", idx - 1, (int)idx, buf);
  printf("read: %zd bytes at the end (expect 0)\n", n);

  close(fd);
  return 0;
}

int main() {
  const char *msg = "hello from userspace";

  if (do_write(msg)) {
    return 1;
  }

  if (do_read()) {
    return 1;
  }

  if (do_read_byte()) {
    return 1;
  }

  return 0;
}