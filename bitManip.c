/*
 * bitManip.c
 *
 *  Created on: May 28, 2016
 *      Author: Brandon Chambers
 *
 *      MATCH FORMATTING TO THAT SHOWN IN HW PDF!!!
 *
 * Take user input of a float number, convert it into a 32-bit
 * representation, then break those 32 bits into the three separate fields
 * (sign, exponent, and fraction) that make up an IEEE floating-point number.
 * Generate the fraction one bit at a time, applying the exponent one multi-
 * plication or division at a time.
 *
 * Program may consist of one dotC file.  Organize into sensible set of
 * functions.
 *
 * Goal of assignment:  MANIPULATING BITS with shift operators (<<, >>) and
 * 		bitwise operators (&, |, ~, ^)
 *
 * Use unsigned! (otherwise you will run into sign extension)
 *
 * You may use Unions and/or Bit Fields
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef union union_tag {
	float f;
	unsigned i;
} Union;

/* function prototypes */
void output32bits(Union u, char **exp, char **frac, int *val, int *sign);
void extractFraction(Union u, float *radix);
void extractExponent(int *val, float *radix);

int main(void) {
	//for eclipse console debug
	setvbuf(stdout, NULL, _IONBF, 0);

	char c = 'r';

	for (int i = 0; i < 10; i++) {
		Union _union;
		printf("\nEnter a float: \n");
		scanf("%f", &(_union.f));
		printf("Your float was read as: %f\n", _union.f);
		int sign, value = 0;
		char *exponent = malloc(32);
		char *fraction = malloc(32);

		output32bits(_union, &exponent, &fraction, &value, &sign);

		printf("\nSign: %d\n", sign);
		printf("Exponent: %s\n", exponent);
		printf("Fraction: %s\n\n", fraction);

		float base;

		extractFraction(_union, &base);

		extractExponent(&value, &base);

		if (_union.f < 0)
			printf("\nFinal Answer:\t-%f\n", base);
		else
			printf("\nFinal Answer:\t %f\n", base);

		printf("\nTo Exit the Program Loop, enter 'q', or...\n");
		scanf("%c", &c);
		if (c == 'q' || c == 'Q') {
			printf("\nGOOD BYE!!!");
			exit(0);
		}

	}

	/************************************************************************
	 *
	 * DEV TEST
	 * leave commented for deployment
	 */
//	  printf("float size in bytes: %d\n", (int)sizeof(float));
//    float float1 = 10111110000010101111010011110001;
//    float float2 = 10111110000010101111010011110001;
//    int z = 0;
//    if(float1 == float2){
//    	z = 1;
//    }
//    printf("\n\n32 bits COMPARATOR: %d\n\n", z);
	/*
	 *  END DEV TEST
	 *//*********************************************************************/
	return 0;
}

/**
 *
 * 	@param
 * 		u Union representation
 * 	@param
 * 		*radix Pointer to float (base)
 */
void extractFraction(Union u, float *radix) {
	int i;
	unsigned long mask = 0x80000000;
	float pwr2 = 0.5; //powers of two
	printf("Creating the fraction: \n");
	*radix = 1.0;
	for (i = 0; i < 32; i++) {
		if (i < 9) {
			if (i == 0)
				printf("fraction = %f (the implicit 1)\n", *radix);
			//shift left 1 bit
			u.i <<=  1;
		} else {
			if (u.i & mask) {
				*radix += pwr2;
				printf("fraction = %f, after adding %f\n", *radix, pwr2);
			} else
				printf("fraction = %f, after skipping %f\n", *radix, pwr2);
			//shift left one bit
			u.i <<=  1;
			pwr2 /= 2;
		}
	}
	printf("\n");
}

/**
 *	Show the unbiased exponent then apply it, times two
 *	up to the original value.
 *
 *  @param
 *  	*val Pointer to int (value)
 *  @param
 *  	*radix Pointer to float (base)
 */
void extractExponent(int *val, float *radix) {
	int i = 0;
	//actual value after removing the offset of 127 bias
	int unbiasedExponent = *val - 127;

	printf("Applying the exponent:\n"
			"Unbiased exponent = %d\n", unbiasedExponent);
	while (i != unbiasedExponent) {
		if (unbiasedExponent >= 0) {
			*radix *= 2;
			printf("times 2 = %f\n", *radix);
		} else {
			*radix /= 2;
			printf("divided by 2 = %f\n", *radix);
		}
		if (unbiasedExponent < 0)
			i--;
		else
			i++;
	}
}

/**
 * Given a float entered by user, convert into a 32-bit representation
 * then output to console
 *
 *  @param
 * 		u Union reference
 *  @param
 * 		**exp Pointer to char pointer (exponent)
 * 	@param
 * 		**frac Pointer to char pointer (fraction)
 * 	@param
 * 		*val Pointer to int (value)
 * 	@param
 * 		*sign Pointer to int (sign bit)
 *
 */
void output32bits(Union u, char **exp, char **frac, int *val, int *sign)
{
	int i;
	unsigned long mask = 0x80000000;
	printf("Your float in 32 bits: ");
	//each iteration of i converts a single bit, hence i < 32
	for (i = 0; i < 32; i++)
	{

		if (u.i & mask)
		{
			if (i == 0)
				*sign = 1;
			else if (i == 1)
			{
				//Convert binary --> float ( 1 )
				*val *= 2;
				*val += 1;
				//replace 'garbage' in uninitialized char
				strcpy(*exp, "1");
			}
			else if (i > 1 && i < 9)
			{
				strcat(*exp, "1");
				*val *= 2;
				*val += 1;
			}
			else
			{
				if (i == 9)
					strcpy(*frac, "1");
				else
					strcat(*frac, "1");
			}
			printf("1");
		}
		// ( !u.i & mask )
		else
		{
			if (i == 0)
				*sign = 0;
			else if (i == 1)
			{
				//Convert binary --> float ( 0 )
				*val *= 2;
				//replace 'garbage' in uninitialized char
				strcpy(*exp, "0");
			}
			else if (i < 9 && i > 1)
			{
				strcat(*exp, "0");
				*val *= 2;
			}
			else
			{
				if (i == 9)
					strcpy(*frac, "0");
				else
					strcat(*frac, "0");
			}
			printf("0");
		}
		u.i <<= 1;
	}
}

