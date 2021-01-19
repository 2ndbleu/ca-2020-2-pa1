//---------------------------------------------------------------
//
//  4190.308 Computer Architecture (Fall 2020)
//
//  Project #1: Compressing Data with Huffman Coding
//
//  September 9, 2020
//
//  Injae Kang (abcinje@snu.ac.kr)
//  Sunmin Jeong (sunnyday0208@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//---------------------------------------------------------------

int placeinside(const char* tab, int lentab, int val);

int encode(const char* inp, int inbytes, char* outp, int outbytes)
{
	if (!inbytes)
		return 0;

	int tabnbocc[16] = {0}; 	// table of the number of occurrence of each value defined by its position in the table
	unsigned char tabsort[16] = {0}; // table of correspondence between value and encoded value
	int lenghttabsort = 0;
	int tabvalueshuffman[16] = {
		0b000, 0b001, 0b010, 0b011, 0b1000, 0b1001,
		0b1010, 0b1011, 0b11000, 0b11001, 0b11010,
		0b11011, 0b11100, 0b11101, 0b11110, 0b11111 }; // table of correspondence between value and the huffman code


	for (int i = 0; i < inbytes; ++i)
	{
		tabnbocc[inp[i] >> 4 & 0xF]++;
		tabnbocc[inp[i] & 0xF]++;
	}

	// count the number of occurrences for each symbol and sort them accordingly

	for (int j = 0; j <= 7; ++j)
	{
		int highestValue = 0;
		int flag = 0;
		for (int i = 0; i <= 15; ++i)
		{
			if (tabnbocc[i] > highestValue)
			{
				tabsort[j] = i;
				highestValue = tabnbocc[i];
				flag = 1;
			}
		}
		if (flag)
			lenghttabsort++;
		tabnbocc[tabsort[j]] = 0;
	}

	for (int i = 0; i <= 15; ++i)
	{
		if (!placeinside((const char*)tabsort, lenghttabsort, i))
		{
			tabsort[lenghttabsort] = i;
			lenghttabsort++;
		}
	}

	int tabtmp[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	if (lenghttabsort < 8)
	{
		for (int k = 0; k < lenghttabsort; ++k)
		{
			for (int l = 0; l < 8; ++l)
			{
				if (tabsort[k] == tabtmp[l])
					tabtmp[l] = -1;
			}
		}

		// Data compression with the table

		for (int k = lenghttabsort; k < 8; ++k)
		{
			int l = 0;
			while (tabtmp[l] == -1)
				l++;
			tabsort[k] = tabtmp[l];
			tabtmp[l] = -1;
		}
	}


// Data formatting
	unsigned char tabdata[inbytes * 2];
	for (int i = 0; i < inbytes; ++i)
	{
		tabdata[i * 2] = inp[i] >> 4 & 0xF;
		tabdata[i * 2 + 1] = inp[i] & 0xF;
	}

	for (int i = 0; i < inbytes * 2; ++i)
	{
		int place;
		if ((place = placeinside((const char*)tabsort, 16, tabdata[i])))
			tabdata[i] = tabvalueshuffman[place - 1];
	}


	// rank table (32 bits)
	unsigned int ranktable = 0;
	unsigned char buffer[outbytes];
	for (int i = 0; i < outbytes; ++i)
		buffer[i] = 0;

	for (int k = 0; k <= 7; ++k)
		ranktable += (tabsort[k] << (28u - 4u * k)); // 32-4-4*k

	for (int k = 0; k <= 3; ++k)
		buffer[k] = ranktable >> (24u - 8u * k);

	// Encode data
	unsigned int indexBuffer = 4;		// we let 32 bits (4*8) for the rank table
	unsigned int indexBit = 4; 		// we let 4 bits for the end info

	for (unsigned int k = 0; k <= inbytes * 2 - 1; ++k)
	{
		unsigned int length = 0;

		if (tabdata[k] <= 3)								// if 3 bits
			length = 3;
		else if (tabdata[k] >= 6 && tabdata[k] <= 11) 		// if 4 bits
			length = 4;
		else if (tabdata[k] >= 24)							// if 5 bits
			length = 5;
															//indexBit + length = placetaken, 8 - indexBit = placeleft
		if (indexBit + length <= 8)
		{
			buffer[indexBuffer] = buffer[indexBuffer] | (tabdata[k] << (8 - indexBit - length));
			indexBit += length;
		}
		else
		{
			buffer[indexBuffer] = buffer[indexBuffer] | (tabdata[k] >> (length - 8 + indexBit));
			indexBuffer++;
			buffer[indexBuffer] = (tabdata[k] << (8 - length + 8 - indexBit));
			indexBit = length - 8 + indexBit;
		}
	}
	indexBuffer++;

	// End info
	unsigned int padding = 0;
	if (8 - indexBit != 0)
		padding = 8 - indexBit;
	buffer[4] = buffer[4] | padding << 4u;


	if (indexBuffer > outbytes)
		return -1;

	for (int k = 0; k < indexBuffer; ++k)
	{
		outp[k] = (char) buffer[k];
	}
	return (int) indexBuffer;
}

int placeinside(const char* tab, int lentab, int val)
{ // return 0 if not in the tab, or place + 1 if inside (useful for if condition trust me)
	for (int i = 0; i < lentab; ++i)
	{
		if (val == tab[i])
			return i + 1;
	}
	return 0;
}
