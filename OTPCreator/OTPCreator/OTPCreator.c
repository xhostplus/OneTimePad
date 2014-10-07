#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
#include <sys/stat.h>
#include <time.h>
#include "getopt.h"


void printHelp (char *_programName)
{
   printf ("Usage: %s [options]\n", _programName);

   printf ("Options:\n");
   printf ("  -k <length>                 \n\tGenerates a <length> byte secret key in hex.\n");
   printf ("  -e <message>.txt <key>.txt  \n\tEncrypt contents of <message> file with contents of <key> file using OTP.\n");
   printf ("  -m <message>.txt            \n\tCreate an ASCII hex representation of contents of <message> file.\n");
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
   int high = hex_to_int (toupper (c)) * 16;
   int low = hex_to_int (toupper (d));
   return high + low;
}

// Generates a random secret key in HEX of _length
void generateSecretKey (int _length)
{
   int i = 0;

   for (i = 0; i < _length; i++)
   {
      printf ("%02X", rand () % 256);
   }
}

// Performs a OTP operation on file _message with file _key
// The contents of these files must be HEX encoded
// The _message file can contain multiple messages per line
// The _key file must contain a key that is as long or longer than the longest message
void performOTP (char *_message, char *_key)
{
   // Open the files and check they are valid
   FILE *m = fopen (_message, "r");
   FILE *k = fopen (_key, "r");

   if (m == NULL)
   {
      fprintf (stderr, "Could not open file for reading: %s\n", _message);
      exit (1);
   }
   if (k == NULL)
   {
      fprintf (stderr, "Could not open file for reading: %s\n", _key);
      exit (1);
   }
   int i = 0;

   char buffer_m [1024];
   char buffer_k [1024];

   memset (buffer_m, 0, sizeof (buffer_m));
   memset (buffer_k, 0, sizeof (buffer_k));

   // Read in the key
   fgets (buffer_k, 1024, k);
   buffer_k [strlen (buffer_k) - 1] = '\0';

   // Loop, reading in each message
   while (fgets (buffer_m, 1024, m))
   {
      buffer_m [strlen (buffer_m) - 1] = '\0';
      i = 0;
      while (buffer_m [i] != '\0')
      {
         // Perform the OTP
         unsigned int m1, k1;
         sscanf (&(buffer_m [i]), "%2X", &m1);
         sscanf (&(buffer_k [i]), "%2X", &k1);
         printf ("%02X", m1 ^ k1);

         i += 2;
      }
      printf ("\n");
   }

   fclose (m);
   fclose (k);
}

//HEX encodes the text in file _filename
void convertStringToHex (char *_filename)
{
   // Converts to ASCII Hex values
   int i = 0;
   char buffer [4096];

   FILE *f = fopen (_filename, "r");

   if (f == NULL)
   {
      fprintf (stderr, "Could not open file for reading: %s\n", _filename);
      exit (1);
   }

   // Loop through each line of text
   while (fgets (buffer, sizeof (buffer), f))
   {
      i = 0;
      while (buffer [i] != '\0')
      {
         // Convert to HEX
         if ((buffer [i] != 0x0A) && (buffer [i] != 0x0D))
         {
            printf ("%02X", buffer [i]);
         }

         i++;
      }
      printf ("\n");
   }

   fclose (f);
}

// Main program entry
int main (int argc, char *argv [])
{
   int opt = 0;

   if (argc == 1)
   {
      printHelp (argv [0]);
      exit (1);
   }

   // Seed the PRNG
   srand (time (0) * _getpid ());

   // Parse the options
   while ((opt = getopt (argc, argv, "k:e:m:")) != -1)
   {
      switch (opt)
      {
         case 'k':
            generateSecretKey (atoi (optarg));
            break;

         case 'e':
         {
            // Get the message  and key arguments
            int index = optind - 1;
            char *message = NULL;
            char *key = NULL;
            if (index < argc)
            {
               message = _strdup (argv [index]);
               index ++;
               key = _strdup (argv [index]);

               performOTP (message, key);

               // Free memory created in strdup
               free (message);
               free (key);

               // Update the parsing index for next getopt call
               optind = index;
            }
         }
            break;

         case 'm':
            convertStringToHex (optarg);
            break;

         default:
            printHelp (argv [0]);
            exit (1);
      }
   }

   return 0;
}