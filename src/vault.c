#include "../include/vault.h"
#include "../include/auth.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

int create_directory(char path[]) {
  int result;
  result = mkdir(path, 0755);
  if (result == -1) {
    if (errno == EEXIST) {
      printf("Directory already exists: %s\n", path);
      return 0;
    }
    printf("Could not create directory: %s\n", path);
    return -1;
  }
  printf("Directory created: %s\n", path);
  return 0;
}

int my_copy(char source[], char destination[]) {
  int inputFd;
  int outputFd;
  inputFd = open(source, O_RDONLY);
  if (inputFd == -1) {
    printf("Error: Could not open source file.\n");
    return -1;
  }
  outputFd = open(destination,O_WRONLY | O_CREAT | O_TRUNC,0644);
  if (outputFd == -1) {
    printf("Error: Could not create destination file.\n");
    close(inputFd);
    return -1;
  }
  char buffer[1024];
  ssize_t bytesRead;
  while ((bytesRead = read(inputFd, buffer, sizeof(buffer))) > 0) {
    ssize_t totalWritten = 0;
    while (totalWritten < bytesRead) {
      ssize_t bytesWritten;
      bytesWritten = write(outputFd,buffer + totalWritten,bytesRead - totalWritten);
      if (bytesWritten == -1) {
        printf("Error: Could not write to destination file.\n");
        close(inputFd);
        close(outputFd);
        return -1;
      }
      totalWritten = totalWritten + bytesWritten;
    }
  }
  if (bytesRead == -1) {
    printf("Error: Could not read source file.\n");
    close(inputFd);
    close(outputFd);
    return -1;
  }
  close(inputFd);
  close(outputFd);
  return 0;
}

int fragment_file(char source[], char vaultPath[]) {
  int inputFd;
  inputFd = open(source, O_RDONLY);
  if (inputFd == -1) {
    printf("Error: Could not open source file.\n");
    return -1;
  }
  char buffer[FRAGMENT_SIZE];
  ssize_t bytesRead;
  int fragmentNumber = 1;
  while ((bytesRead = read(inputFd, buffer, FRAGMENT_SIZE)) > 0) {
    char fragmentName[100];
    char fragmentPath[200];
    snprintf(
        fragmentName,
        sizeof(fragmentName),
        "fragment_%03d",
        fragmentNumber
    );
    snprintf(
        fragmentPath,
        sizeof(fragmentPath),
        "%s/%s",
        vaultPath,
        fragmentName
    );
    int fragmentFd;
    fragmentFd = open(fragmentPath,O_WRONLY | O_CREAT | O_TRUNC,0644);
    if (fragmentFd == -1) {
      printf("Error: Could not create %s.\n", fragmentPath);
      close(inputFd);
      return -1;
    }
    ssize_t totalWritten = 0;
    while (totalWritten < bytesRead) {
      ssize_t bytesWritten;
      bytesWritten = write(
          fragmentFd,
          buffer + totalWritten,
          bytesRead - totalWritten
      );
      if (bytesWritten == -1) {
        printf("Error: Could not write %s.\n", fragmentPath);
        close(fragmentFd);
        close(inputFd);
        return -1;
      }
      totalWritten = totalWritten + bytesWritten;
    }
    close(fragmentFd);
    printf(
        "Created %s (%ld bytes)\n",
        fragmentPath,
        bytesRead
    );
    fragmentNumber++;
  }
  if (bytesRead == -1) {
    printf("Error: Could not read source file.\n");
    close(inputFd);
    return -1;
  }
  close(inputFd);
  return 0;
}

void list_vaults() {
  DIR* dir;
  dir = opendir("vaults");
  if (dir == NULL) {
    printf("No vaults found.\n");
    return;
  }
  struct dirent* entry;
  printf("\nAvailable Vaults:\n");
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    char entryPath[400];
    snprintf(
        entryPath,
        sizeof(entryPath),
        "vaults/%s",
        entry->d_name
    );
    struct stat entryInfo;
    if (stat(entryPath, &entryInfo) == -1) {
      continue;
    }
    if (S_ISDIR(entryInfo.st_mode)) {
      printf("- %s\n", entry->d_name);
    }
  }
  closedir(dir);
}

int extract_file(char vaultName[], char outputFile[]) {
  char vaultPath[300];
  snprintf(vaultPath, sizeof(vaultPath), "vaults/%s", vaultName);
  int outputFd;
  outputFd = open(outputFile,O_WRONLY | O_CREAT | O_TRUNC,0644);
  if (outputFd == -1) {
    perror("Error creating output file");
    return -1;
  }
  int fragmentNumber = 1;
  while (1) {
    char fragmentName[400];
    snprintf(
        fragmentName,
        sizeof(fragmentName),
        "%s/fragment_%03d",
        vaultPath,
        fragmentNumber
    );
    int fragmentFd;
    fragmentFd = open(fragmentName, O_RDONLY);
    if (fragmentFd == -1) {
      break;
    }
    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(fragmentFd,buffer,sizeof(buffer))) > 0) {
      ssize_t bytesWritten;
      bytesWritten = write(outputFd,buffer,bytesRead);
      if (bytesWritten != bytesRead) {
        perror("Error writing to output file");
        close(fragmentFd);
        close(outputFd);
        return -1;
      }
    }
    if (bytesRead == -1) {
      perror("Error reading fragment");
      close(fragmentFd);
      close(outputFd);
      return -1;
    }
    if (close(fragmentFd) == -1) {
      perror("Error closing fragment");
    }
    fragmentNumber++;
  }
  if (close(outputFd) == -1) {
    perror("Error closing output file");
    return -1;
  }
  printf("File extracted successfully!\n");
  return 0;
}

int delete_vault(char vaultName[]) {
  char vaultPath[300];
  snprintf(vaultPath, sizeof(vaultPath), "vaults/%s", vaultName);
  if (!verify_password(vaultName)) {
    printf("Incorrect password. Vault was not deleted.\n");
    return -1;
  }
  DIR* dir = opendir(vaultPath);
  if (dir == NULL) {
    perror("Error opening vault");
    return -1;
  }
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    char filePath[600];
    snprintf(
        filePath,
        sizeof(filePath),
        "%s/%s",
        vaultPath,
        entry->d_name
    );
    if (unlink(filePath) == -1) {
      perror("Error deleting file");
      continue;
    }
    printf("Deleted %s\n", filePath);
  }
  if (closedir(dir) == -1) {
    perror("Error closing vault directory");
    return -1;
  }
  if (rmdir(vaultPath) == -1) {
    perror("Error removing vault directory");
    return -1;
  }
  printf("Vault deleted successfully!\n");
  return 0;
}

void check_fragment_size(char vaultName[]) {
  int fragmentNumber;
  printf("Enter fragment number: ");
  scanf("%d", &fragmentNumber);
  char fragmentPath[600];
  snprintf(
      fragmentPath,
      sizeof(fragmentPath),
      "vaults/%s/fragment_%03d",
      vaultName,
      fragmentNumber
  );
  int fd = open(fragmentPath, O_RDONLY);
  if (fd == -1) {
    printf("Error: Fragment does not exist.\n");
    return;
  }
  off_t size;
  size = lseek(fd, 0, SEEK_END);
  if (size == -1) {
    perror("Error determining fragment size");
    close(fd);
    return;
  }
  printf("Fragment_%03d size: %ld bytes\n",fragmentNumber,(long)size);
  close(fd);
}
