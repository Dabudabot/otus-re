/*

date: 05.05.2019
author: Daulet

Keygen for crackme1

*/

#include <stdio.h>
#include <string.h>

#define MAX_STR_LEN 30

/**
  Checking password for matching value.
  Print sequence if match

  @param  sequence         sequence to be checked
  @param  sequenceLen	   lenght of the sequence
  @param  value			   hash to match with

  @retval 1 - in case of match
  @retval 0 - in case of miss
**/
int checkPass(char* sequence, int sequenceLen, int value)
{
	int result = 0;
	for (int i = 0; i < sequenceLen; i++)
	{
		result = result + (sequence[i] ^ 0x99);
	}

	if ((result & 0xff) == value)
	{
		printf("password: %s\n", sequence);
		system("pause");
		return 1;
	}

	return 0;
}

/**
  Moving to the next symbol. Valid symbols 0-9, A-Z, a-z

  @param  sym         symbol to be incremented

  @retval 0 - in case of success
  @retval 1 - in case of symbols overflow (error)
**/
int incSymbol(char* sym)
{
	(*sym)++;

	if ((*sym) == '9' + 1)
	{
		(*sym) = 'A';
	}

	if ((*sym) == 'Z' + 1)
	{
		(*sym) = 'a';
	}

	if ((*sym) == 'z' + 1)
	{
		return 1;
	}

	return 0;
}

/**
  Moving to the next combination

  @param  seq         sequence of characters
  @param  len		  lenght of the sequence

  @retval 0 - in case of success
  @retval 2 - if all avaliable combinations reached
  @retval 1 - in case of wrong combination (error)
**/
int nextSequence(char* seq, int len)
{
	int j = len - 1;

	//	did we try`em all?
	while (seq[j] == 'z' && j >= 0) j--;

	//	dead end
	if (j < 0) return 2;

	if (incSymbol(&seq[j])) return 1;

	//	we just changed last symbol its ok
	if (j == len - 1) return 0;

	//	we changed not last symbol, need to drop last
	for (int k = j + 1; k < len; k++)
	{
		seq[k] = '0';
	}
}

/**
  Calculating all valid passwords

  @param  value         login hash that should match password

  @retval 0 - in case of success
**/
int calcPass(int value)
{
	//	check all avaliable lenght
	for (int len = 1; len <= MAX_STR_LEN; len++)
	{
		char* sequence = (char*)malloc(sizeof(char) * (len + 1));

		//	iterate over each char in seq to set defaults
		for (int c = 0; c < len; c++)
		{
			sequence[c] = '0';
		}
		sequence[len] = '\0';
		
		//	iterating over all possible combinations
		while (nextSequence(sequence, len) != 2)
		{
			//	if combination matches value then print
			checkPass(sequence, len, value);
		}

		free(sequence);
	}

	return 0;
}

/**
  Program takes name as input login, calculate its hash
  Finally generate from hash sequences of valid passwords

  @param  argc         must be 2 (name of the program and input name)
  @param  argv         contains input name (must be no longer than MAX_STR_LEN)

  @retval 0 - in case of success
**/
int main(
	_In_ int argc,
	_In_ char** argv
)
{
	//	check input
	if (argc != 2)
	{
		printf("%s\n", "Usage: CrackmeKeygen <Name>");
		return 1;
	}
	
	char* login = argv[1];

	short loginLen = strlen(login);

	//	check len
	if (loginLen > MAX_STR_LEN || loginLen == 0)
	{
		printf("%s %d\n", "Wrong length of the login. Minimum login lenght: 1, Maximum name length: ", MAX_STR_LEN);
		return 1;
	}

	//  check valid symbols
	for (short i = 0; i < loginLen; i++)
	{
		if (login[i] < '0' 
			|| (login[i] > '9' && login[i] < 'A')
			|| (login[i] > 'Z' && login[i] < 'a')
			|| (login[i] > 'z'))
		{
			printf("%s\n", "Unacceptable symbols used");
			return 1;
		}
	}

	//	calc login
	int loginHash = 0xffffffff;
	for (short i = 0; i < loginLen; i++)
	{
		loginHash = loginHash ^ login[i];
		for (int j = 7; j >= 0; j--)
		{
			int temp = loginHash;
			_asm 
			{
				and temp, 1
				neg temp
				shr loginHash, 1
				and temp, 0xedb88320
			}
			loginHash = loginHash ^ temp;
		}
	}
	_asm
	{
		not loginHash;
		and loginHash, 0x0ff;
	}

	//
	//  Calculating pass
	//
	calcPass(loginHash);

	return 0;
}


