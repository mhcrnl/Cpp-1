/*
 * 12.09 File Encryption Filter
 *
 * Takes a file as a command line parameter, then prompts the user for an
 * encryption passphrase.  The phrase is hashed and the hash is used to
 * encrypt the file.  The encrypted file name will be the same as the original
 * with the extension of ".encrypted".
 */

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <termios.h>
#include <unistd.h>
#include <openssl/sha.h>

using namespace std;

string getPassPhrase()
{
	// This code was found at http://bit.ly/1LXA6Hn
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    string s;
    getline(cin, s);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return s;
}

string sha512(const char* input)
{
	// This code was found at
	// http://www.askyb.com/cpp/openssl-sha512-hashing-example-in-cpp/
    unsigned char digest[SHA512_DIGEST_LENGTH];

    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, input, strlen(input));
    SHA512_Final(digest, &ctx);

    char mdString[SHA512_DIGEST_LENGTH*2+1];
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    return string(mdString);
}

int incHashIndex(int index, int max)
{
	return (index + 1 == max) ? 0 : index + 1;
}

void makeEncryptedFile(string fileName, string hash)
{
	int hashIndex = 0;
	fstream inFile(fileName, ios::in);
	fstream outFile(fileName + ".encrypted", ios::out | ios::binary);
	if (!inFile || !outFile)
	{
		cout << "Error opening files." << endl;
		return;
	}

	char ch;
	while (true)
	{
		inFile.get(ch);
		if (inFile.eof())
		{
			break;
		}
		ch += hash[hashIndex];
		hashIndex = incHashIndex(hashIndex, hash.size());
		outFile.put(ch);
	}

	inFile.close();
	outFile.close();
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "Must provide file name to encrypt." << endl;
		return 1;
	}

	cout << "Enter Passphrase: ";
	string phrase = getPassPhrase();
	string hash = sha512(phrase.c_str());
	cout << endl;
	makeEncryptedFile(string(argv[1]), hash);
    return 0;
}

