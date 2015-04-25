/*
ANSI X9.31 Appendix A.2.4 Using AES

Let ede*X(Y) represent the AES encryption of Y under the key *X.
	For AES 128-bit key, let *K be a 128 bit key.

This *K is reserved only for the generation of pseudo random numbers.
Let V be a 128-bit seed value which is also kept secret, and XOR be the exclusive-or
operator. Let DT be a date/time vector which is updated on each iteration. I is a
intermediate value. A vector R is generated as follows (Note for AES implementations
DT, I, and R are 128-bits each.):
	I = ede *K(DT)
	R = ede *K(I XOR V) and a new V is generated by V = ede*K(R Xor I). 
*/

#include "TI_aes_128.h"
#include "prng.h"
#include "time.h"


//128-bit Seed value
static unsigned char V[]= { 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
							   0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0x00};

//KEY K is reserved only for the generation of pseudo random numbers.
static unsigned char K[]={0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
							   0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0x00};
							   
							   
void generatePRN(unsigned char *prn){
	
	int e = 0;
	
	//TODO get DT external Seed
	// DT date/time which is updated on each iteration
	
	time_t timer;
	struct tm y2k = {0};
	double seconds;

	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100;   y2k.tm_mon = 0; y2k.tm_mday = 1;
 
	time(&timer);  // get current time

	seconds = difftime(timer,mktime(&y2k));

	//printf ("%f ", seconds);

	int aux = (int) seconds;
	//printf("%d ",aux);
	
	char DT[16];
	memset(DT,'0',16);
	
	int i = 0;	
	while(aux) {
		DT[i++] = aux % 16 + '0';
		aux /= 16;
	}
	
	//printf("\n hex \n");
	//for(e = 0; e < BLOCK_SIZE; e++){
	//	printf("%02x, ",DT[e] & 0xff);
	//	}
	
	unsigned char I[16];
	
	//Auxiliar vectors for aes
	unsigned char state[BLOCK_SIZE];
	unsigned char keyAux[BLOCK_SIZE];
							   
	memcpy(state,DT, BLOCK_SIZE);
	memcpy(keyAux,K, BLOCK_SIZE);
	
	//I = AES K(DT)	
	aes_enc_dec(state,keyAux,0);
	memcpy(I,state,BLOCK_SIZE);
	
	//I XOR V 
	for (e=0;e<BLOCK_SIZE;e++){
		state[e]= state[e] ^ V[e];
		}
	
	memcpy(keyAux ,K ,BLOCK_SIZE);
	//R = AES K(I xor V)
	aes_enc_dec(state,keyAux,0);
	
	
	//R XOR I 
	for (e=0;e<BLOCK_SIZE;e++){
		state[e]= state[e] ^ I[e];
		}
	
	memcpy(keyAux ,K ,BLOCK_SIZE);	
	// V = AES K(R xor I)
	aes_enc_dec(state,keyAux,0);
	
	memcpy(prn,state,BLOCK_SIZE);
	
	}
