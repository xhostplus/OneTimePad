#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

void printHelp(char *_programName)
{
	printf("Usage: %s [options]\n", _programName);
	
	printf("Options:\n");
	printf("  -k <length>                  Generates a <length> byte secret key in hex.\n");
	printf("  -e <message>.txt <key>.txt   Encrypt contents of <message> file with contents of <key> file using OTP.\n");
	printf("  -m <message>.txt             Create an ASCII hex representation of contents of <message> file.\n");
}

// Converts a hex character to it's integer equivelant.  Uppercase letters only.
int hex_to_int (char c)
{
	int first = c / 16 - 3;
	int second = c % 16;
	int result = first * 10 + second;
	if (result > 9)
        {
        	result--;
        }
        return result;
}

// Takes a hex byte (0xccdd) and converts it to the integer equivelant.
int hex_to_ascii (char c, char d)
{
	int high = hex_to_int(toupper(c)) * 16;
	int low = hex_to_int(toupper(d));
	return high+low;
}


void generateSecretKey(int _length)
{
	// Generates a secret key in HEX of _length
	int i = 0;

	for (i = 0; i < _length; i++)
	{
		printf("%02X", rand() % 256);
	}
}

void performOTP(char *_message, char *_key)
{
	FILE *m = fopen(_message, "r");
	FILE *k = fopen(_key, "r");

	if (m == NULL)
	{
		fprintf (stderr, "Could not open file for reading: %s\n", _message);
		exit(1);
	}
	if (k == NULL)
	{
		fprintf (stderr, "Could not open file for reading: %s\n", _key);
		exit(1);
	}

	// Make sure the filesizes are the same
	int i = 0;

	char buffer_m[4096];
	char buffer_k[4096];

	fgets(buffer_k, 4096, k);

	while (fgets(buffer_m, 4096, m))
	{
		i = 0;
		while (buffer_m[i] != '\0')
		{
			unsigned int m1, k1;
			sscanf(&(buffer_m[i]), "%2X", &m1);
			sscanf(&(buffer_k[i]), "%2X", &k1);
			printf("%02X", m1 ^ k1);

			i += 2;
		}
		printf("\n");
	}

	fclose(m);
	fclose(k);
}

void convertStringToHex(char *_filename)
{
	// Converts to ASCII Hex values
	int i = 0;
	char buffer[4096];

	FILE *f = fopen(_filename, "r");

	if (f == NULL)
	{
		fprintf (stderr, "Could not open file for reading: %s\n", _filename);
		exit(1);
	}

	while (fgets(buffer, sizeof(buffer), f))
	{
		i = 0;
		while (buffer[i] != '\0')
		{
			if ((buffer[i] != 0x0A) && (buffer[i] != 0x0D))
			{
				printf ("%02X", buffer[i]);
			}
			
			i++;
		}
		printf("\n");
	}

	fclose(f);
}

int main(int argc, char *argv[])
{
	int opt = 0;

	if (argc == 1)
	{
		printHelp(argv[0]);
		exit(1);
	}

	// Seed the PRNG
	srand(time(0) * getpid());

	// Parse the options
	while ((opt = getopt(argc, argv, "k:e:m:")) != -1)
	{
		switch (opt)
		{
			case 'k':
			generateSecretKey(atoi(optarg));
			break;

			case 'e':
			{
				// Get the message argument
				int index = optind - 1;
				char *message = NULL;
				char *key = NULL;
				if (index < argc)
				{
					message = strdup(argv[index]);
					index ++;
					key = strdup(argv[index]);

					performOTP(message, key);

					// Free memory created in strdup
					free(message);
					free(key);

					// Update the parsing index for next getopt call
					optind = index;
				}
			}
			break;

			case 'm':
			convertStringToHex(optarg);
			break;

			default:
			printHelp(argv[0]);
			exit(1);
		}
	}

	return 0;
}

