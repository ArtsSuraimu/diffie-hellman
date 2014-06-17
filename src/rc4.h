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
#include <stdint.h>
#ifndef RC4_H
#define RC4_H

/* Initializes the RC4 random number generator with a secret key. */
int rc4_init(uint64_t key);

/* Frees memory allocated when rc4_init() is called */
void rc4_clean();

/* Resets the RC4 random number generator. */
void rc4_reset();

/* Get 1 byte of keystream from an initialized RC4 random number generator. */
uint8_t rc4_nextkey();

/* Encrypts/decrypts a data stream of length len in place. */
void rc4_crypt(unsigned char *stream, int len);

#endif /* RC4_H */
