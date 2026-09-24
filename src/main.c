#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  int choice;
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

        printf("\nEnter file name: ");
        scanf("%99s", filename);

        int inputFd = open(filename, O_RDONLY);

        if (inputFd == -1) {
          printf("Error: Could not open the input file.\n");
          break;
        }

        int outputFd = open("copy.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (outputFd == -1) {
          printf("Error: Could not create the output file.\n");
          close(inputFd);
          break;
        }

        char buffer[1024];
        ssize_t bytesRead;

        while ((bytesRead = read(inputFd, buffer, sizeof(buffer))) > 0) {
          ssize_t bytesWritten = write(outputFd, buffer, bytesRead);

          if (bytesWritten == -1) {
            printf("Error: Could not write to the output file.\n");
            break;
          }
        }

        if (bytesRead == -1) {
          printf("Error: Could not read the input file.\n");
        } else {
          printf("File copied successfully.\n");
        }

        close(inputFd);
        close(outputFd);

        break;
      }
      case 2:
        printf("\nList Vaults selected.\n");
        break;
      case 3:
        printf("\nUnlock Vault selected.\n");
        break;
      case 4:
        printf("\nExtract File selected.\n");
        break;
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
