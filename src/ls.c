#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void printPermissionsOfFile(const char *file) {
  struct stat fileStat;
  if (stat(file, &fileStat) < 0) {
    printf("Error: could not stat file\n");
  } else {
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
  }
}

void printOwnerOfFile(const char *file) {
  struct stat fileStat;
  if (stat(file, &fileStat) < 0) {
    printf("Error: could not stat file\n");
  } else {
    struct passwd *pw = getpwuid(fileStat.st_uid);
    struct group *gr = getgrgid(fileStat.st_gid);
    printf(" %2ld %s %s", (long)fileStat.st_nlink, pw->pw_name, gr->gr_name);
  }
}

void printTimeOfFile(const char *file) {
  struct stat fileStat;
  if (stat(file, &fileStat) < 0) {
    printf("Error: could not stat file\n");
  } else {
    char timebuf[80];
    struct tm *tm_info = localtime(&fileStat.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);
    printf(" %s", timebuf);
  }
}

void printSizeOfFile(const char *file) {
  struct stat fileStat;
  if (stat(file, &fileStat) < 0) {
    printf("Error: could not stat file\n");
  } else {
    printf(" %6lld", (long long)fileStat.st_size);
  }
}

void printFile(const char *file) {
  printPermissionsOfFile(file);
  printOwnerOfFile(file);
  printSizeOfFile(file);
  printTimeOfFile(file);
  printf(" %s\n", file);
}

int compare(const void *a, const void *b) {
  const struct dirent *dirA = *(const struct dirent **)a;
  const struct dirent *dirB = *(const struct dirent **)b;
  return strcmp(dirA->d_name, dirB->d_name);
}

void listDirectory(const char *dir) {
  DIR *dp;
  struct dirent *entry;
  struct stat statbuf;
  struct dirent **entries = NULL;
  int count = 0;
  int capacity = 10;

  if ((dp = opendir(dir)) == NULL) {
    fprintf(stderr, "cannot open directory: %s\n", dir);
    return;
  }

  entries = malloc(capacity * sizeof(struct dirent *));
  if (entries == NULL) {
    perror("malloc");
    closedir(dp);
    return;
  }

  while ((entry = readdir(dp)) != NULL) {
    if (count >= capacity) {
      capacity *= 2;
      entries = realloc(entries, capacity * sizeof(struct dirent *));
      if (entries == NULL) {
        perror("realloc");
        closedir(dp);
        return;
      }
    }
    entries[count] = entry;
    count++;
  }

  closedir(dp);

  qsort(entries, count, sizeof(struct dirent *), compare);

  for (int i = 0; i < count; i++) {
    lstat(entries[i]->d_name, &statbuf);
    if (S_ISDIR(statbuf.st_mode)) {
      if (strcmp(".", entries[i]->d_name) == 0 ||
          strcmp("..", entries[i]->d_name) == 0) {
        continue;
      }
    }
    printFile(entries[i]->d_name);
  }

  free(entries);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s <dir>\n", argv[0]);
    return 1;
  }
  listDirectory(argv[1]);
  return 0;
}
