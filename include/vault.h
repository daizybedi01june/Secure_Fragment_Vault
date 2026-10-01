#ifndef VAULT_H
#define VAULT_H

#define FRAGMENT_SIZE 1024

int create_directory(char path[]);
int my_copy(char source[], char destination[]);
int fragment_file(char source[], char vaultPath[]);
void list_vaults();
int extract_file(char vaultName[], char outputFile[]);
int delete_vault(char vaultName[]);
void check_fragment_size(char vaultName[]);

#endif
