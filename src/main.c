#include <stdio.h>

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
      case 1:
        printf("\nCreate Vault selected.\n");
        break;
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
