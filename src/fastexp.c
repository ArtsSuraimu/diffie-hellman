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

#include <inttypes.h>

#include "fastexp.h"


uint64_t modadd(uint64_t a, uint64_t b, uint64_t mod)
{
 	a += b;
 	if (a < b)
 	   	a -= mod;
  	return a % mod;
}

uint64_t modshl(uint64_t a, unsigned int s, uint64_t mod)
{
  	while (s--)
 	   	a = modadd(a, a, mod);
  	return a;
}

uint64_t modmul(uint64_t a, uint64_t b, uint64_t mod)
{
 	uint64_t ah, al, bh, bl;

 	ah = a >> 32;
 	bh = b >> 32;
   	al = a & UINT64_C(0x00000000FFFFFFFF);
  	bl = b & UINT64_C(0x00000000FFFFFFFF);
  	
 	return modadd(modadd(modshl((ah * bh) % mod, 64, mod),
   	  	 	modshl(modadd((ah * bl) % mod,
 	 	   	(al * bh) % mod, mod),
  	  	 	32, mod), mod),
   	  	 	(al * bl) % mod, mod);
}

uint64_t fastexp(uint64_t base, uint64_t exp, uint64_t mod)
{
 	int i;
 	uint64_t result;

   	result = UINT64_C(1);
  	i = 64;
  	while (!(exp & UINT64_C(0x8000000000000000)) && i--)
 	   	exp <<= 1;
  	while (i--) {
 	 	result = modmul(result, result, mod);
 	   	if (exp & UINT64_C(0x8000000000000000))
  	  	 	result = modmul(result, base, mod);
   	  	exp <<= 1; 	
 	}
 	return result;
}
