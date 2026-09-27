#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FRAGMENT_SIZE 1024

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

unsigned long simple_hash(char password[]) {
  unsigned long hash = 5381;
  int i = 0;
  while (password[i] != '\0') {
    hash = hash * 33 + password[i];
    i++;
  }
  return hash;
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
    snprintf(fragmentName, sizeof(fragmentName), "fragment_%03d",
             fragmentNumber);
    snprintf(fragmentPath, sizeof(fragmentPath), "%s/%s", vaultPath,
             fragmentName);
    int fragmentFd;
    fragmentFd = open(fragmentPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fragmentFd == -1) {
      printf("Error: Could not create %s.\n", fragmentPath);
      close(inputFd);
      return -1;
    }
    ssize_t totalWritten = 0;
    while (totalWritten < bytesRead) {
      ssize_t bytesWritten;
      bytesWritten =
          write(fragmentFd, buffer + totalWritten, bytesRead - totalWritten);
      if (bytesWritten == -1) {
        printf("Error: Could not write %s.\n", fragmentPath);
        close(fragmentFd);
        close(inputFd);
        return -1;
      }
      totalWritten = totalWritten + bytesWritten;
    }
    close(fragmentFd);
    printf("Created %s (%ld bytes)\n", fragmentPath, bytesRead);
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

int unlock_vault(char vaultName[]) {
  char vaultPath[300];
  snprintf(vaultPath, sizeof(vaultPath), "vaults/%s", vaultName);
  char accessPath[400];
  snprintf(accessPath, sizeof(accessPath), "%s/access.dat", vaultPath);
  int accessFd = open(accessPath, O_RDONLY);
  if (accessFd == -1) {
    printf("Error: Could not open vault access file.\n");
    return -1;
  }
  unsigned long storedHash;
  ssize_t bytesRead = read(accessFd, &storedHash, sizeof(storedHash));
  if (bytesRead != sizeof(storedHash)) {
    printf("Error: Could not read password information.\n");
    close(accessFd);
    return -1;
  }
  close(accessFd);
  char password[100];
  printf("Enter password: ");
  scanf("%99s", password);
  unsigned long enteredHash = simple_hash(password);
  if (enteredHash == storedHash) {
    printf("\nVault unlocked successfully!\n");
    return 1;
  } else {
    printf("\nIncorrect password. Access denied.\n");
    return 0;
  }
}

int extract_file(char vaultName[], char outputFile[]) {
  char vaultPath[300];
  snprintf(vaultPath, sizeof(vaultPath), "vaults/%s", vaultName);
  int outputFd;
  outputFd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (outputFd == -1) {
    printf("Error: Could not create output file.\n");
    return -1;
  }
  int fragmentNumber = 1;
  while (1) {
    char fragmentName[400];
    snprintf(fragmentName, sizeof(fragmentName), "%s/fragment_%03d", vaultPath,
             fragmentNumber);
    int fragmentFd;
    fragmentFd = open(fragmentName, O_RDONLY);
    if (fragmentFd == -1) {
      break;
    }
    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(fragmentFd, buffer, sizeof(buffer))) > 0) {
      ssize_t bytesWritten;
      bytesWritten = write(outputFd, buffer, bytesRead);
      if (bytesWritten != bytesRead) {
        printf("Error: Could not write to output file.\n");
        close(fragmentFd);
        close(outputFd);
        return -1;
      }
    }
    close(fragmentFd);
    fragmentNumber++;
  }
  close(outputFd);
  printf("File extracted successfully!\n");
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
  outputFd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
      bytesWritten =
          write(outputFd, buffer + totalWritten, bytesRead - totalWritten);
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

int main() {
  int choice;
  int vaultUnlocked = 0;
  while (1) {
    printf("\n");
    printf("====================================\n");
    printf("       SECURE FRAGMENT VAULT\n");
    printf("====================================\n");
    printf("1. Create Vault\n");
    printf("2. List Vaults\n");
    printf("3. Unlock Vault\n");
    printf("4. Extract File\n");
    printf("5. Delete Vault\n");
    printf("6. Exit\n");
    printf("====================================\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    switch (choice) {
      case 1: {
        char filename[100];
        char vaultName[100];
        char vaultPath[300];
        char password[100];
        printf("\nEnter file name: ");
        scanf("%99s", filename);
        printf("Enter vault name: ");
        scanf("%99s", vaultName);
        printf("Create password: ");
        scanf("%99s", password);
        snprintf(vaultPath, sizeof(vaultPath), "vaults/%s", vaultName);
        int result;
        result = create_directory("vaults");
        if (result == -1) {
          printf("Could not prepare vault storage.\n");
          break;
        }
        result = create_directory(vaultPath);
        if (result == -1) {
          printf("Could not create vault.\n");
          break;
        }
        unsigned long passwordHash;
        passwordHash = simple_hash(password);
        char accessPath[400];
        snprintf(accessPath, sizeof(accessPath), "%s/access.dat", vaultPath);
        int accessFd;
        accessFd = open(accessPath, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (accessFd == -1) {
          printf("Error: Could not create access file.\n");
          break;
        }
        ssize_t hashWritten;
        hashWritten = write(accessFd, &passwordHash, sizeof(passwordHash));
        if (hashWritten != sizeof(passwordHash)) {
          printf("Error: Could not save password information.\n");
          close(accessFd);
          break;
        }
        close(accessFd);
        result = fragment_file(filename, vaultPath);
        if (result == 0) {
          printf("\nVault created successfully!\n");
          printf("Vault location: %s\n", vaultPath);
        } else {
          printf("\nVault creation failed.\n");
        }
        break;
      }
      case 2:
        printf("\nList Vaults selected.\n");
        break;
      case 3: {
        char vaultName[100];
        printf("\nEnter vault name: ");
        scanf("%99s", vaultName);
        vaultUnlocked = unlock_vault(vaultName);
        break;
      }
      case 4: {
        if (vaultUnlocked == 0) {
          printf("\nAccess denied. Unlock the vault first.\n");
          break;
        }
        char vaultName[100];
        char outputFile[100];
        printf("\nEnter vault name: ");
        scanf("%99s", vaultName);
        printf("Enter output file name: ");
        scanf("%99s", outputFile);
        extract_file(vaultName, outputFile);
        break;
      }
      case 5:
        printf("\nDelete Vault selected.\n");
        break;
      case 6:
        printf("\nExiting Secure Fragment Vault...\n");
        return 0;
      default:
        printf("\nInvalid choice! Please enter a number from 1 to 6.\n");
    }
  }
  return 0;
}
