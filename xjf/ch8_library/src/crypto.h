#ifndef _CRYPTO_H_
#define _CRYPTO_H_

/*  Description: This function used to encrypto message in $plaintext, and put           
 *  ciphertext into $ciphertext
 *  Return value: 0: Success <0: Failure
 */ 
extern int encrypt(char *plaintext, char *ciphertext, int size);
 
/*  Description: This function used to decrypto message in $ciphertext, and put          
 *  plaintext into $plaintext
 *  Return value: 0: Success <0: Failure 
 */ 
extern int decrypt(char *ciphertext, char *plaintext, int size); 

#endif /*  ----- #ifndef _CRYPTO_H_ ----- */ 

