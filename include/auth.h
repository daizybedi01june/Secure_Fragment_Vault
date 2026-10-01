#ifndef AUTH_H
#define AUTH_H

unsigned long simple_hash(char password[]);
int unlock_vault(char vaultName[]);
int verify_password(char vaultName[]);

#endif
