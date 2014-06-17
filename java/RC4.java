

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

public class RC4
{
 	private byte k[];
 	private byte S[];
 	private byte ki, kj;

   	public RC4(long key)
  	{
  	 	k = new byte[8];
   	  	k[0] = (byte)(key);
 	 	k[1] = (byte)(key >> 8);
 	   	k[2] = (byte)(key >> 16);
  	  	k[3] = (byte)(key >> 24);
 	   	k[4] = (byte)(key >> 32);
  	 	k[5] = (byte)(key >> 40);
 	 	k[6] = (byte)(key >> 48);
   	  	k[7] = (byte)(key >> 56);
  	 	S = new byte[256];
   	  	reset();
 	}

 	public void reset()
 	{
   	  	for (int i = 0; i < 256; i++)
  	 	   	S[i] = (byte)i;
  	 	byte j = 0;
 	 	for (int i = 0; i < 256; i++) {
   	  	  	j = (byte)(j + S[i] + k[i % 8]);
 	   	  	S[i] ^= S[j & 0xFF];
 	 	 	S[j & 0xFF] ^= S[i];
   	  	  	S[i] ^= S[j & 0xFF];
 	   	}
  	 	ki = kj = 0;
 	}

 	public byte nextKey()
   	{
  	  	ki++;
 	   	kj += S[ki & 0xFF];
  	 	S[ki & 0xFF] ^= S[kj & 0xFF];
 	 	S[kj & 0xFF] ^= S[ki & 0xFF];
   	  	S[ki & 0xFF] ^= S[kj & 0xFF];
  	 	byte k = (byte)(S[ki & 0xFF] + S[kj & 0xFF]);
   	  	return S[k & 0xFF];
 	}

 	public byte[] crypt(byte stream[], int len)
 	{
   	  	for (int i = 0; i < len; i++)
  	 	   	stream[i] ^= nextKey();
  	 	return stream;
 	}
}
