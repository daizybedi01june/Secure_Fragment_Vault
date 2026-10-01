#include "../include/auth.h"
#include "../include/vault.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void handle_sigint(int signal) {
  printf("\n\nCtrl+C detected.\n");
  printf("Secure Fragment Vault is shutting down safely...\n");
  exit(0);
}

int main() {
  signal(SIGINT, handle_sigint);
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
    printf("6. Check Fragment Size\n");
    printf("7. Exit\n");
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
      case 2: {
        list_vaults();
        break;
      }
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
      case 5: {
        char vaultName[100];
        printf("\nEnter vault name: ");
        scanf("%99s", vaultName);
        delete_vault(vaultName);
        break;
      }
      case 6: {
        char vaultName[100];
        printf("\nEnter vault name: ");
        scanf("%99s", vaultName);
        check_fragment_size(vaultName);
        break;
      }
      case 7:
        printf("\nExiting Secure Fragment Vault...\n");
        return 0;
      default:
        printf("\nInvalid choice! Please enter a number from 1 to 6.\n");
    }
  }
  return 0;
}
