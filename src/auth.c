#include "../include/auth.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

unsigned long simple_hash(char password[]) {
  unsigned long hash = 5381;
  int i = 0;
  while (password[i] != '\0') {
    hash = hash * 33 + password[i];
    i++;
  }
  return hash;
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
  ssize_t bytesRead = read(
      accessFd,
      &storedHash,
      sizeof(storedHash)
  );
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

int verify_password(char vaultName[]) {
  char vaultPath[300];
  snprintf(vaultPath, sizeof(vaultPath), "vaults/%s", vaultName);
  char accessPath[400];
  snprintf(accessPath, sizeof(accessPath), "%s/access.dat", vaultPath);
  int accessFd;
  accessFd = open(accessPath, O_RDONLY);
  if (accessFd == -1) {
    printf("Error: Could not open vault access file.\n");
    return 0;
  }
  unsigned long storedHash;
  ssize_t bytesRead;
  bytesRead = read(accessFd, &storedHash, sizeof(storedHash));
  close(accessFd);
  if (bytesRead != sizeof(storedHash)) {
    printf("Error: Could not read password information.\n");
    return 0;
  }
  char password[100];
  printf("Enter password: ");
  scanf("%99s", password);
  unsigned long enteredHash = simple_hash(password);
  if (enteredHash == storedHash) {
    return 1;
  }
  return 0;
}
