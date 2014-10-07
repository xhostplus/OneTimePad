#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

int hex_to_ascii (char c, char d)
{
   int high = hex_to_int (toupper (c)) * 16;
   int low = hex_to_int (toupper (d));
   return high + low;
}

int main (int argc, char *argv [])
{
   char message [1024];
   char cipher [1024];
   char cipher2 [1024];
   int key [1024];
   int len;
   int i;

   // arg1 = Encrypted Key file
   // arg2 = Encrypted Ciphertexts file

   FILE *k = fopen (argv [1], "r");
   fgets (cipher, 1024, k);
   cipher [strlen (cipher) - 1] = '\0';
   fclose (k);


   printf ("Enter Decoded Secret Message\n");
   fgets (message, sizeof (message), stdin);

   //  printf ("CIPHER1\n");
   //  fgets (cipher, sizeof(cipher), stdin);

   len = strlen (cipher);

   printf ("KEY=");
   for (i = 0; i < len; i += 2)
   {
      int val = hex_to_ascii (cipher [i], cipher [i + 1]);

      printf ("%d ", (val ^ message [i / 2]));

      key [i / 2] = (val ^ message [i / 2]);
   }

   printf ("\n\n");

   FILE *f = fopen (argv [2], "r");
   fgets (cipher2, 1024, f);

   while (fgets (cipher2, sizeof (cipher2), f))
   {
      for (i = 0; i < len; i += 2)
      {
         int val = hex_to_ascii (cipher2 [i], cipher2 [i + 1]);

         int letter = val ^ key [i / 2];

         if (letter >= 0x20 && letter <= 0x7A)
            printf ("%c", val ^ key [i / 2]);
         else
            printf (".");
      }
      printf ("\n");
   }
}
