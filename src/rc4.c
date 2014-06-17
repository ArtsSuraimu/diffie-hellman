/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#include "rc4.h"


#define swap(a, b) {\
 	a ^= b; \
 	b ^= a; \
 	a ^= b; }


static uint64_t k;
static uint8_t *S;
static uint8_t _i;
static uint8_t _j;

int rc4_init(uint64_t key)
{
   	if (!(S = malloc(256)))
  	  	return -ENOMEM;
 	k = key;
   	rc4_reset();
  	return 0;
}

void rc4_clean()
{
 	free(S);
}

void rc4_reset()
{
 	int i;
 	uint8_t j, *key;

   	for (i = 0; i < 256; i++)
  	  	S[i] = (uint8_t)i;
 	j = 0;
   	key = (uint8_t *)&k;
  	for (i = 0; i < 256; i++) {
 	 	j = j + S[i] + key[i % 8];
 	   	swap(S[i], S[j]);
  	}

  	_i = 0;
 	_j = 0;
}

// Fixme: i,j are not reset wen rc4_reset() is called
uint8_t rc4_nextkey()
{ 
   	uint8_t k;

  	_i++;
 	_j += S[_i];
 	swap(S[_i], S[_j]);
 	k = S[_i] + S[_j];
   	return S[k];
}

void rc4_crypt(unsigned char *stream, int len)
{
  	while (len--)
  	 	*(stream++) ^= rc4_nextkey();
}

