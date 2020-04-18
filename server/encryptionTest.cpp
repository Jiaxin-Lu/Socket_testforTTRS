#include <cstring>
#include "AES.hpp"
#include <iostream>
#include <cstdio>
using namespace std;

void print(unsigned char* state, int len);
unsigned char input[200];
int main(int argc, char* argv[])
{
	//unsigned char input[] = {84, 104, 105, 115, 32, 105, 115, 32, 97, 32, 116, 101, 115, 116, 33, 84, 104, 105, 115, 32, 105, 115, 32, 97, 32, 116, 101, 115, 116, 33, 84, 104, 105, 115, 32, 105, 115, 32, 97, 46};
	unsigned char iv[]    = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};

	unsigned char key[]   = {'z','h','u','a','n','g','y','o','n','g','h','a','o','t','q','l'};
	unsigned char input[] = {'8','2','3','6','5','9','B','C','3','8','3','D','4','0','E','6','5','F','3','7','A','2','3','3','F','9','1','4','5','7','8','1','9','7','1','E','1','5','9','3','9','D','2','5','B','9','5','E','A','6','4','2','F','D','B','9','6','0','0','0','D','B','B','F'};
	unsigned char output[100] ={0};
    unsigned char temp[100] = {0};
//	unsigned char 
	AESModeOfOperation moo;
	moo.set_key(key);
	moo.set_mode(MODE_CBC);
	moo.set_iv(iv);
    int olen = sizeof input;
	cout << olen << endl;

    memcpy(temp, input, sizeof input);
	int len = moo.Decrypt(temp, olen, output);
	printf("len = %d\n", len);
    printf("output");
	print(output, len);
	printf("\n\nDecrypt----------\n");
	len = moo.Encrypt(output, len, input);
	printf("len = %d\n", len);
    printf("input");
	print(input, len);
	
	char chh[100];
	cin.getline(chh,90);
	return 0;
}

void print(unsigned char* state, int len)
{
	int i;
	for(i=0; i<len; i++)
	{
        if ( i%16 == 0) printf("\n");
//		printf("%s%X ",state[i]>15 ? "" : "0", state[i]);
        printf("%d  ", (int)(state[i] & 0xff));
	}
	printf("\n");
}

