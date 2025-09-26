#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void
runexec(char *cmdargs[], int ncmd, char *file)
{
  char *argv[MAXARG];
  int i;
  for (i = 0; i < ncmd; i++)
    argv[i] = cmdargs[i];
  argv[i++] = file;
  argv[i] = 0;

  if (fork() == 0) {
    exec(argv[0], argv);
    fprintf(2, "find: exec %s failed\n", argv[0]);
    exit(1);
  }
  wait(0);
}

void
find(char *path, char *target, char *cmdargs[], int ncmd)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type == T_FILE) {
    // extract last component
    char *last = path;
    for (char *q = path; *q; q++) {
      if (*q == '/')
        last = q+1;
    }
    if (strcmp(last, target) == 0) {
      if (ncmd > 0)
        runexec(cmdargs, ncmd, path);
      else
        printf("%s\n", path);
    }
  }

  if (st.type == T_DIR) {
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("find: path too long\n");
      close(fd);
      return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if (stat(buf, &st) < 0) {
        printf("find: cannot stat %s\n", buf);
        continue;
      }

      if (st.type == T_DIR) {
        // recurse first
        find(buf, target, cmdargs, ncmd);
      } else {
        if (strcmp(de.name, target) == 0) {
          if (ncmd > 0)
            runexec(cmdargs, ncmd, buf);
          else
            printf("%s\n", buf);
        }
      }
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "Usage: find <path> <name> [-exec <cmd> ...]\n");
    exit(1);
  }

  char *cmdargs[MAXARG];
  int ncmd = 0;

  if (argc > 3 && strcmp(argv[3], "-exec") == 0) {
    // collect command arguments after -exec
    for (int i = 4; i < argc && i-4 < MAXARG-2; i++) {
      cmdargs[ncmd++] = argv[i];
    }
    cmdargs[ncmd] = 0;
  }

  find(argv[1], argv[2], cmdargs, ncmd);
  exit(0);
}
