#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <iostream>
#include <time.h>
//#include <values.h>

#define mask 0xFFF
#define bitnum 12
#define emask 0x7FFFFF

/* endianness testing */
const int EndianTest = 0x04030201;
#define LITTLE_ENDIAN() (*((const char *) &EndianTest) == 0x01)
/* extract nth LSB from object stored in lvalue x */
#define GET_BIT(x, n) ((((const char *) &x)[LITTLE_ENDIAN() ? (n) / CHARBITS : sizeof(x) - (n) / CHARBITS - 1] >> ((n) % CHARBITS)) & 0x01)

#define PUT_BIT(x, n) (putchar(GET_BIT((x), (n)) ? '1' : '0'))

#define not_real_number printf("Not real number\n"); \
                  exit(1);

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 generator(rd()); //Standard mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<int>  RandomExponent(0, 254);
std::uniform_int_distribution<int>  RandomMantissa(0, 0x7FFFFF);
std::uniform_int_distribution<int>  RandomSign(0, 1);


int nnn = 1;
int checknum = 0;


//Union 사용
typedef union {
	float f;
	struct {
		unsigned int mantissa : 23;
		unsigned int exponent : 8;
		unsigned int sign : 1;
	} parts;
} float_cast;

float_cast makeFP();
float_cast FPAdder(float_cast a, float_cast b, int case_num);
float_cast makeFP() {
	//exponet => 127(7F) ~ -128(80)
	//mantissa => 524287(7FFFF) ~ 0(0)
	float_cast num;
	num.parts.exponent = RandomExponent(generator); //0~255 -> 2^(exponent - 126) * 1.(mantissa)
										//if exponent == 255 -> inf
	num.parts.mantissa = RandomMantissa(generator);
	num.parts.sign = RandomSign(generator);
	return num;
}

int exp_cal(unsigned int x, unsigned int y)
{
	int expnum = 4, exmask = 0xF0;
	int sub = 0;

	if (x > y)
		sub = (x & exmask) - (y & exmask);
	else
		sub = (y & exmask) - (x & exmask);

	exmask >>= expnum;

	sub += (x & exmask) ^ (y & exmask);

	return sub;
}

void mantissa_cal(float_cast &z, float_cast &x, float_cast &y, int subEx) {
	z.parts.exponent = x.parts.exponent;

	if (subEx > 23)   //shift 가 mantissa의 23비트 넘어서면 0으로 초기화!
		y.parts.mantissa = 0;
	else {
		//a.parts.mantissa >>= abs(subEx);
		if (y.parts.exponent != 0)
			y.parts.mantissa = (y.parts.mantissa >> 1) | 0x400000;

		if (subEx > 1)
			y.parts.mantissa >>= subEx - 1;
	}
}


unsigned int LOA(unsigned int a, unsigned int b)
{
	unsigned int m, n, sum;
	int carry;

	m = a & mask;
	n = b & mask;

	sum = m | n;
	carry = (m >> bitnum - 1) & (n >> bitnum - 1);

	sum += (a - m) + (b - n) + (carry << bitnum);

	return sum;
}

unsigned int ETA1(unsigned int a, unsigned int b)
{
	unsigned int M, N, m, n, inaccuratePart = 0, r, sum = 0;
	int carry, imask = 0x800;

	M = a & mask;
	N = b & mask;

	while (1) {
		m = a & imask;
		n = b & imask;
		r = m ^ n;
		inaccuratePart += r;
		if (m == imask && n == imask) {
			inaccuratePart += imask - 1;
			break;
		}
		imask /= 2;
		if (imask == 0)
			break;
	}

	sum = (a - M) + (b - N) + inaccuratePart;

	return sum;
}

void extbit_cal(float_cast x, int subEx, int *e)
{
	unsigned int m = emask;
	unsigned int temp = x.parts.mantissa;

	if (x.parts.exponent == 0)
		subEx--;

	if (subEx >= 25) {
		if (x.parts.exponent != 0) {
			m |= 0x800000; temp |= 0x800000;
		}

		e[0] = (temp & m) ? 1 : 0; // sticky bit

		// round bit
		if (subEx == 25 && x.parts.exponent != 0)
			e[1] = 1;
		else
			e[1] = 0;

		e[2] = 0; // guard bit -> 0
	}
	else if (subEx == 24) {
		e[0] = (temp & (m >> 1)) ? 1 : 0; // sticky bit
		e[1] = (temp & (1 << 22)) ? 1 : 0; // round bit

		// guard bit
		if (x.parts.exponent != 0)
			e[2] = 1;
		else
			e[2] = 0;

	}
	else {
		m >>= (23 - subEx);
		temp &= m;
		if (subEx >= 2) {
			e[0] = (temp & (m >> 2)) ? 1 : 0; // sticky bit
			e[1] = (temp & (1 << (subEx - 2))) ? 1 : 0; // round bit
		}
		e[2] = (temp & (1 << (subEx - 1))) ? 1 : 0; // guard bit
	}
}

float_cast AXAdder(float_cast a, float_cast b, int caseNum) {

	//먼저 두 값이 real number인지 판단해야한다. (inf, -inf, 0, -0, NAN)
	//0 FF 000000 -> inf, 1 FF 000000 -> -inf, 00000 -> 0, 100000 -> -0

	//입력값이 INF일 수 가있나?
	if (a.parts.exponent == 0xFF && a.parts.sign == 0)  //a is inf 
	{
		not_real_number
	}
	else if (a.parts.exponent == 0xFF && a.parts.sign == 1)//a is -inf 
	{
		not_real_number
	}

	if (b.parts.exponent == 0xFF && b.parts.sign == 0)  //b is inf 
	{
		not_real_number
	}
	else if (b.parts.exponent == 0xFF && b.parts.sign == 1)//b is -inf 
	{
		not_real_number
	}

	float_cast z; //return 값
 	z.parts.sign = 0;
	unsigned int sum = 0;
	int ext_bit[3] = { 0, }; // guard, round, sticky bit
	int subEx_tmp = a.parts.exponent - b.parts.exponent;
	int subEx = abs(subEx_tmp);
	if (subEx != 0) {//exponents equal
		checknum = 1;
		if (a.parts.exponent > b.parts.exponent) {// a's exponent > b's exponent  => shift mantissa right
							//b.parts.exponent = a.parts.exponent;
			extbit_cal(b, subEx, ext_bit);
			mantissa_cal(z, a, b, subEx);
			z.parts.sign = a.parts.sign;
		}
		else {// a's exponent < b's exponent => shift mantissa right
			 //a.parts.exponent = b.parts.exponent;
			extbit_cal(a, abs(subEx), ext_bit);
			mantissa_cal(z, b, a, subEx);
			z.parts.sign = b.parts.sign;
		}
	}
	else
		z.parts.exponent = a.parts.exponent;

	switch (caseNum) {
	case 1: //LOA
		sum = LOA(a.parts.mantissa, b.parts.mantissa);
		break;
	case 2: //ETA1
		sum = ETA1(a.parts.mantissa, b.parts.mantissa);
		break;
	}
	

	//mantissa + mantissa가 23비트가 넘어가버리면 자동으로 잘라버림! (왜냐면 union이니깐)
	//따라서 우리가 직접 넘어가는 carry값을 처리해줘야한다.
	if (sum > 0x7FFFFF) {
		if (subEx == 0) {

			if (z.parts.exponent != 0) {
				z.parts.mantissa = sum >> 1;
				ext_bit[0] = ext_bit[0] | ext_bit[1]; // sticky bit = sum[1] | sum[0]
				ext_bit[1] = ext_bit[2]; // round bit
				ext_bit[2] = (sum & 1) ? 1 : 0; // guard bit
			}
			else {
				//z.parts.mantissa = sum & 0x3FFFFF;
				z.parts.mantissa = sum;
			}
			z.parts.exponent++;
		}
		else {
			ext_bit[0] = ext_bit[0] | ext_bit[1]; // sticky bit = sum[1] | sum[0]
			ext_bit[1] = ext_bit[2]; // round bit
			ext_bit[2] = (sum & 1) ? 1 : 0; // guard bit

			z.parts.mantissa = (sum >> 1) & 0x3FFFFF;
			z.parts.exponent++;
		}
	}
	else {
		if (subEx == 0) {
			if (a.parts.sign == b.parts.sign) {
				if (z.parts.exponent != 0) {
					ext_bit[0] = ext_bit[1]; // round bit
					ext_bit[1] = ext_bit[2]; // round bit
					ext_bit[2] = (sum & 1) ? 1 : 0; // guard bit
					z.parts.mantissa = sum >> 1;
					z.parts.exponent++;
				}
				else
					z.parts.mantissa = sum;
			}
			else {
				int cnt = 1;
				if (z.parts.exponent != 0) {
					for (cnt = 1; sum & 0x400000 ? 0 : 1; cnt++) {
						if (z.parts.exponent - cnt == 0)
							break;
						sum <<= 1;
					}
					if (z.parts.exponent - cnt == 0)
						z.parts.mantissa = sum;
					else
						z.parts.mantissa = (sum << 1) & 0x7FFFFF;
					z.parts.exponent -= cnt;
				}
				else
					z.parts.mantissa = sum;
			}
		}
		else {
			z.parts.mantissa = sum;
		}
	}


	//overflow!
	if (z.parts.mantissa == 0 && (z.parts.exponent >= 0xFF)) {
		//printf("\noverflow!\n");
		z.parts.mantissa = 1 << 23;
		z.parts.exponent = 255;
		return z;
		//z.parts.mantissa <<= 1;
		//z.parts.exponent--;
		//z.parts.mantissa += ext_bit[2];
		//ext_bit[2] = ext_bit[1];
		//ext_bit[1] = 0;
	}
	//underflow!
	else if (z.parts.exponent >= 0xFF) {
		//printf("\nunderflow!\n");
		z.parts.mantissa = 1 << 23;
		z.parts.exponent = 255;
		return z;
		//ext_bit[2] = z.parts.mantissa & 1;
		//ext_bit[1] = ext_bit[2];
		//ext_bit[0] = ext_bit[0] | ext_bit[1];
		//z.parts.mantissa >>= 1;
		//z.parts.exponent++;
	}
	// guard && (round bit | sticky | z_m[0])
	if (ext_bit[2] && (ext_bit[1] | ext_bit[0] | (z.parts.mantissa & 1))) {
		z.parts.mantissa++;
		if (z.parts.mantissa >= 0xffffff)
			z.parts.exponent++;
	}

	if (z.parts.mantissa == 0) {
		z.f = 0;
	}
	return z;
}


int main(void) {
	float_cast A, B;
	float_cast orgAns, loa, eta1;
	//FILE* input = fopen("Errorlist.txt", "r");
	//FILE* output = fopen("Errorlist.txt", "w");
	int cnt = 0;
	printf("A\t\t+\t\tB\t  =\torgANS\t\tLOA\t\tETA1\n");
	printf("**********************************************************************\n");

	while (nnn<=100) {
		//fscanf(input, "%f %f ", &A.f, &B.f);

		//A, B 직접 지정
		/*A.f = 4.011746e-35;
		B.f = 3.558128e-35;*/
		do {
			A = makeFP();
			B = makeFP();
		} while (A.parts.sign != 0 || B.parts.sign != 0);
		orgAns.f = A.f + B.f;
		loa = AXAdder(A, B, 1);
		eta1 = AXAdder(A, B, 2);

		if (checknum == 0) {
			printf("%d: %e    +    %e    =    %e,   %e,   %e\n", nnn, A.f, B.f, orgAns.f, loa.f, eta1.f);
			printf("\n\n******************************\n");
		}
		else {
			checknum = 0;
			printf("++%d: %e    +    %e    =    %e,   %e,   %e\n", nnn, A.f, B.f, orgAns.f, loa.f, eta1.f);
			printf("\n\n******************************\n");
		}

		//if(orgAns.parts.exponent != loa.parts.exponent)
		//	fprintf(output, "%e\t%e\n", A.f, B.f);
		nnn++;
	}
	//fclose(output);
	//fclose(input);
}