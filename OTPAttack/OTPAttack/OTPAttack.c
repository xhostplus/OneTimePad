#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

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

int isLetter (char c)
{
   if ((c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A))
      return 1;

   return 0;
}

void main (int argc, char *argv [])
{
   char decode [1024];     // Buffer to hold the secret message to decode
   char buffer [1024];     // Buffer used to read in the ciphers texts
   int num_ciphers = 0;    // The number of ciphertexts to read
   char **ciphers;         // Array of buffers to hold the ciphertexts
   int **spaces;           // Array of buffers to hold if byte in ciphertext is a space
   int *len;               // Length of each ciphertext

   FILE *f = NULL;
   int i, j, k;

   // arg1 contains the filename of the secret message
   // arg2 contains the filename of the list of ciphertexts

   f = fopen (argv [1], "r");
   fgets (decode, sizeof (decode), f);
   fclose (f);

   // Open the file containing the ciphertexts
   f = fopen (argv [2], "r");

   // Get the number of cipher texts in the file
   fgets (buffer, sizeof (buffer), f);
   sscanf (buffer, "%d", &num_ciphers);

   // Allocate arrays
   ciphers = malloc (sizeof (char *) * num_ciphers);
   spaces = malloc (sizeof (char *) * num_ciphers);
   len = malloc (sizeof (int) * num_ciphers);

   // Read the cipher texts from the file
   for (i = 0; i < num_ciphers; i++)
   {
      fgets (buffer, sizeof (buffer), f);

      len [i] = strlen (buffer) - 1;
      ciphers [i] = malloc (sizeof (char) * len [i]);

      // Half as many spaces allocated since two characters 
      // of ciphertext is one number
      spaces [i] = malloc (sizeof (int) * len [i] / 2);
      memset (spaces [i], 0, sizeof (int) * len [i] / 2);
      memcpy (ciphers [i], buffer, len [i]);
   }

   // Find the spaces in all the cipher texts
   // Loop through each ciphertext and xor each character with with all the other ciphertexts.
   // If the xor contains a printable character, then either one of the ciphertexts contains a space.
   // If the xor contains a non-printable character, then either one doesn't contain a space.
   // With this space information, use that to decode the printable characters in the secret message
   for (i = 0; i < num_ciphers; i++)
   {
      for (j = 0; j < num_ciphers; j++)
      {
         if (i != j)
         {
            int count, len1, len2, s = 0;
            len1 = len [i];
            len2 = len [j];

            // Which cipher text is smaller, use that one
            count = len2;
            if (len1 < len2)
            {
               count = len1;
            }

            for (k = 0; k < count; k += 2)
            {
               // Get both the hex values from the cipher texts
               int v1 = hex_to_ascii (ciphers [i] [k], ciphers [i] [k + 1]);
               int v2 = hex_to_ascii (ciphers [j] [k], ciphers [j] [k + 1]);

               int xor = v1 ^ v2;

               // If we have a displayable letter, then
               // either cipher texts contain a space
               // If we have encounted a space before, then 
               // in fact, this is a space
               if ((xor > 0x20) && (xor <= 0x7E))
               {
                  //printf ("%c", xor);
                  spaces [i] [k / 2]++;
                  spaces [j] [k / 2]++;
               }
               else
               {
                  //printf (".");
                  spaces [i] [k / 2]--;
                  spaces [j] [k / 2]--;
               }

            }

         }
      }
   }

   // find smallest cipher text length
   int smallest = strlen (decode);

   for (i = 0; i < num_ciphers; i++)
   {
      for (j = 0; j < smallest; j += 2)
      {
         // If we have detected a potential space in this ciphertext, process it
         if (spaces [i] [j / 2] > 0)
         {
            // i cipher contains a space at (j/2)
            if (j < strlen (decode))
            {
               int d = hex_to_ascii (decode [j], decode [j + 1]);
               int m = hex_to_ascii (ciphers [i] [j], ciphers [i] [j + 1]);
               int letter = d ^ m;

               // This is potentially a space
               if (letter == 0)
                  printf ("_");
               else
               {
                  // If we have a displayable character, display it
                  if (isLetter(letter))
                  {
                     printf ("%c", letter);
                  }
                  else
                     printf (".");
               }
            }
         }
         else
            printf (".");
      }
      printf ("\n");
   }
}
