/* PADDING_POWER	FREQUENCY	ERROR TO 60Hz	*/
/*	0	5.8593750000000000e+01	-2,34375%	*/
/*	1	5.9765625000000000e+01	-0,39063%	*/
/*	2	6.0058593750000000e+01	 0,09766%	*/
/*	3	6.0058593750000000e+01	 0,09766%	*/
/*	4	5.9985351562500000e+01	-0,02441%	*/
/*	5	5.9985351562500000e+01	-0,02441%	*/
/*	6	6.0003662109375000e+01	 0,00610%	*/
/*	7	6.0003662109375000e+01	 0,00610%	*/
/*	8	5.9999084472656250e+01	-0,00153%	*/
/*	10	5.9999084472656250e+01	-0,00153%	*/

// https://www.cs.unm.edu/~brayer/vision/fourier.html
// https://www.mathworks.com/help/matlab/ref/fft.html

/********************************** Headers ***********************************/

/* ------------------------ Inclusion of Std Headers ------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <string.h>


/*********************** Directives of C Pre-processor ************************/

/************************** Conditional Compilation ***************************/

#define DEBUGGING                 /* Use directive when debugging some lines  */



/**************************** Symbolic  Constants *****************************/

/* ---------------------- Constants to Exit Functions ----------------------- */

#define SUCCEED              1    /* Succeeded ending in function execution   */
#define FAIL                 0    /* Failed    ending in function execution   */

/* ---------------------- Analysis in Frequency Domain ---------------------- */

#define FORWARD              1    /* Forward direction for FFT computation    */
#define REVERSE              0    /* Reverse direction for FFT computation    */

/* -------- Default Values for Iteration in Time & Frequency Domain --------- */

#define MAX_TIME_SAMPLES     1024  /* Testing default value for time_samples */
#define PADDING_POWER        2                /* Default padding power factor */
#define MAX_FREQ_SAMPLES     MAX_TIME_SAMPLES<<PADDING_POWER  /* freq_samples */



/************************** Definition of Data Types **************************/

/* ---------------------------- Complex Numbers ----------------------------- */

typedef struct {
	float real; /* Real part of type complex number      */
	double imag; /* Imaginary part of type complex number */
} COMPLEX_T;



/************************** Prototypes of Functions ***************************/

/* ---------------------------- Public Functions ---------------------------- */

int cplx_Swap( COMPLEX_T *, COMPLEX_T * );

COMPLEX_T cplx_Multiply( COMPLEX_T, COMPLEX_T );
COMPLEX_T cplx_Subtract( COMPLEX_T, COMPLEX_T );
COMPLEX_T cplx_Add     ( COMPLEX_T, COMPLEX_T );
COMPLEX_T cplx_Scale   ( COMPLEX_T, long );

double cplx_Magnitude( COMPLEX_T );
double cplx_Phase    ( COMPLEX_T );
double cplx_Ratio    ( COMPLEX_T, COMPLEX_T );

int cplx_Initiate_Vector( COMPLEX_T *, int );

int Compute_FFT( int, int, COMPLEX_T * );

int Find_Power( long );



/******************************* Complex Vector *******************************/

static COMPLEX_T Sine_at_60Hz[MAX_FREQ_SAMPLES];



/******************************** Main Function *******************************/

int
main()
{
int i;
int direction = FORWARD;

int time_samples = MAX_TIME_SAMPLES;
int freq_samples = MAX_FREQ_SAMPLES;
int power = Find_Power( freq_samples );

int    samples_per_period = 64;
double frequency  = 100;
double period     = 1.0 / frequency;
double delta_time = period / samples_per_period;
double delta_freq = 1.0 / (delta_time * freq_samples);

//double omegadt    = 2 * M_PI * frequency * delta_time;

FILE *fp;
char str[5];
float flo;

fp = fopen("TEK00001.CSV", "r");

for( i = 0; i < time_samples; i++ ){

	fgets (str, 10, fp);
	flo = strtod(str,NULL);
    //printf("%f\n", flo);
	Sine_at_60Hz[i].real = flo; // > 0.0 ? 1.0 : -1.0;
	}
Compute_FFT( direction, power, Sine_at_60Hz );


// fprintf( stdout, "\tSine_at_60Hz(f)\n" );
for( i = 0; i < freq_samples/2; i++ )
	fprintf( stdout, "%.*e\t%.*e\n",
		16, i*delta_freq, 16, cplx_Magnitude( Sine_at_60Hz[i] ) );

return( 0 );

} /* main  */



/*****************************  Public Functions  *****************************/

/* ---------------------------- Complex Numbers ----------------------------- */

/*FN****************************************************************************
*
*   void
*   cplx_Swap( COMPLEX_T *a, COMPLEX_T *b );
*
*   Return:          Nothing (result is passed by reference)
*
*   Purpose:         Exchange complex numbers
*
*   Register of Revisions (Debugging Process):
*
*   DATE       RESPONSIBLE  COMMENT
*   -----------------------------------------------------------------------
*   Jun 04/13  J.C.Giraldo  Initial implementation
*
*******************************************************************************/

int
cplx_Swap( COMPLEX_T *a, COMPLEX_T *b )
{
COMPLEX_T t;

t  = *a;
*a = *b;
*b =  t;

return 1;

} /* cplx_Swap */



/*FN****************************************************************************
*
*   COMPLEX_T
*   cplx_Multiply( COMPLEX_T a, COMPLEX_T b );
*
*   Purpose:         Return the product of two complex numbers
*
*   Note:            Temporal vble t is required in order to avoid changing
*                    result when computing result
*
*   Register of Revisions (Debugging Process):
*
*   DATE       RESPONSIBLE  COMMENT
*   -----------------------------------------------------------------------
*   Jun 04/13  J.C.Giraldo  Initial implementation
*
*******************************************************************************/

COMPLEX_T
cplx_Multiply( COMPLEX_T a, COMPLEX_T b )
{
COMPLEX_T t;

t.real = a.real * b.real - a.imag * b.imag;
t.imag = a.real * b.imag + a.imag * b.real;

return t;

} /* cplx_Multiply */



/*FN****************************************************************************
*
*   COMPLEX_T
*   cplx_Subtract( COMPLEX_T a, COMPLEX_T b );
*
*   Purpose:         Return the subtraction of two complex numbers
*
*   Register of Revisions (Debugging Process):
*
*   DATE       RESPONSIBLE  COMMENT
*   -----------------------------------------------------------------------
*   Jun 04/13  J.C.Giraldo  Initial implementation
*
*******************************************************************************/

COMPLEX_T
cplx_Subtract( COMPLEX_T a, COMPLEX_T b )
{
a.real -= b.real;
a.imag -= b.imag;

return a;

} /* cplx_Subtract */



/*FN****************************************************************************
*
*   COMPLEX_T
*   cplx_Add( COMPLEX_T a, COMPLEX_T b );
*
*   Purpose:         Return the summation of two complex numbers
*
*   Register of Revisions (Debugging Process):
*
*   DATE       RESPONSIBLE  COMMENT
*   -----------------------------------------------------------------------
*   Jun 04/13  J.C.Giraldo  Initial implementation
*
*******************************************************************************/

COMPLEX_T
cplx_Add( COMPLEX_T a, COMPLEX_T b )
{
a.real += b.real;
a.imag += b.imag;

return a;

} /* cplx_Add */



/*FN****************************************************************************
*
*   COMPLEX_T
*   cplx_Scale( COMPLEX_T a, long n );
*
*   Purpose:         Return each component scalized to n
*
*   Register of Revisions (Debugging Process):
*
*   DATE       RESPONSIBLE  COMMENT
*   -----------------------------------------------------------------------
*   Jun 04/13  J.C.Giraldo  Initial implementation
*
*******************************************************************************/

COMPLEX_T
cplx_Scale( COMPLEX_T a, long n )
{
a.real /= (double)n, a.imag /= (double)n;

return a;

} /* cplx_Scale */



/*FN****************************************************************************
*
*   double
*   cplx_Magnitude( COMPLEX_T a );
*
*   Purpose:         Return the magnitude of a complex number
*
*   Register of Revisions (Debugging Process):
*
*   DATE       RESPONSIBLE  COMMENT
*   -----------------------------------------------------------------------
*   Jun 05/13  J.C.Giraldo  Initial implementation
*
*******************************************************************************/

double
cplx_Magnitude( COMPLEX_T a )
{
return sqrt( a.real * a.real + a.imag * a.imag );

} /* cplx_Magnitude */



/*FN****************************************************************************
*
*   double
*   cplx_Phase( COMPLEX_T a );
*
*   Purpose:         Return the phase of a complex number
*
*   Register of Revisions (Debugging Process):
*
*   DATE       RESPONSIBLE  COMMENT
*   -----------------------------------------------------------------------
*   Nov 07/17  J.C.Giraldo  Initial implementation
*
*******************************************************************************/

double
cplx_Phase( COMPLEX_T a )
{
return atan2( a.imag, a.real );

} /* cplx_Phase */



/*FN****************************************************************************
*
*   double
*   cplx_Ratio( COMPLEX_T a, COMPLEX_T b );
*
*   Purpose:         Return the ratio of magnitudes of two complex numbers
*
*   Register of Revisions (Debugging Process):
*
*   DATE       RESPONSIBLE  COMMENT
*   -----------------------------------------------------------------------
*   Nov 07/17  J.C.Giraldo  Initial implementation
*
*******************************************************************************/

double
cplx_Ratio( COMPLEX_T a, COMPLEX_T b )
{
return fabs( a.real / b.real );

} /* cplx_Ratio */



/*FN****************************************************************************
*
*   int cplx_Initiate_Vector( COMPLEX_T *a, int size );
*
*   Purpose: Fill a complex vector with zeros
*
*   Return:  Nothing
*
*   Register of Revisions (Debugging Process):
*
*   DATE       RESPONSIBLE  COMMENT
*   -----------------------------------------------------------------------
*   Nov 07/17  J.C.Giraldo  Initial implementation
*
*******************************************************************************/

int
cplx_Initiate_Vector( COMPLEX_T *a, int size )
{
for( int ii = 0; ii < size; ii++ )
    a[ii].real = 0.0, a[ii].imag = 0.0;

return 1;

} /* cplx_Initiate_Vector */



/* ---------------------- Analysis in Frequency Domain ---------------------- */

/*FN****************************************************************************
*
*   int
*   Compute_FFT( int dir, int pow, COMPLEX_T *x );
*
*   Return:          Transformation in values passed by reference
*
*   Purpose:         Compute an in-place complex-to-complex FFT
*
*   Note:            This function computes an in-place complex-to-complex FFT
*                    x.real and y.imag are the real and imaginary arrays of
*                    2^pow points.
*                    dir = 1 or FORWARD gives forward transform
*                    dir = 0 or REVERSE gives reverse transform
*
*                    There is a modification by Peter Cusak to utilize the
*                    Microsoft complex type.
*
*   Plan:
*           Part 1: Calculate the number of points
*           Part 2: Do the bit reversal
*           Part 3: Compute the FFT
*           Part 4: Scale for forward transformation
*
*   Register of Revisions (Debugging Process):
*
*   DATE         RESPONSIBLE  COMMENT
*   -------------------------------------------------------------------------
*   Jun 04/2013  J.C.Giraldo  Incorporation of functions with complex numbers
*   May 03/2013  J.C.Giraldo  Readable identifiers
*   Jun --/1993  P. Bourkes   Initial implementation
*
*******************************************************************************/
typedef enum {
    Estate_0,
    Estate_1,
    Estate_2,
    Estate_3,
    Estate_4,
    Estate_5,
    Estate_6,
    Estate_7,
    Estate_1_5,
    Estate_4_5
} STATE_T;

int
Compute_FFT( int dir, int pow, COMPLEX_T *x )
{
	STATE_T state = Estate_0; 
	long points, i, j, k, l, i1, i2, l1, l2;
	COMPLEX_T c, temp, u;
	int finish = 0;
	i = 0;
	points = 1;
	while(!finish){
		switch( state ) {
			case Estate_0: 						// Se verifica si i < pow permanece en el estado cero en caso contrario pasa al estado uno.
				if (i < pow){
					points <<= 1;
					i++;
					state = Estate_0;	
				}
				else{
					i2 = points >> 1;
					j = 0;	
					i = 0;
					state = Estate_1;
				}
			break;
			
			case Estate_1:
				if (i < points-1){				// Se verifica si i < points-1 pasa al estado intermedio uno en caso contrario pasa al estado tres.
					state = Estate_1_5;
				}else{
					c.real = -1.0;
					c.imag =  0.0;
					l2 =  1;
					state = Estate_3;
					l = 0;
				}
			break;	

			case Estate_1_5:
				if (i < j){						// Verifica si i < j, si se cumple esa condicion pasa al estado dos
					cplx_Swap( &x[i], &x[j] );	// en caso contrario pasa al estado dos pero sin ejecutar la instruccion 464.
					k = i2;
					state = Estate_2;
				} else{
					k = i2;
					state = Estate_2;	
				}

			break;			
				
			case Estate_2:						
				if ( k <= j ){					// Verifica si k <= j, si esto se cumple permanece en el estado dos
					j -= k; 					// en caso contrario regresa al esatdo uno.
					k >>= 1;
					state = Estate_2;
				}else{
					j += k;
					i++;
					state = Estate_1;
				}
			break;
				
			case Estate_3:
				if (l < pow){					// Si l < pow pasa al estado cuatro de otra forma pasa al estado seis.
					l1 = l2;
				    l2 <<= 1;
				    u.real = 1.0;
				    u.imag = 0.0;
				    state = Estate_4;
				    j = 0;
				} else{
					state = Estate_6;
				}
			break;
				
			case Estate_4:
				if (j < l1){					// Si j < l1 pasa al estdo cinco de otra forma pasa al estado intermedio cuatro.
					i = j;
					state = Estate_5;
				} else{
				    c.imag = sqrt( ( 1.0 - c.real ) / 2.0 );
					state = Estate_4_5;  
				}
			break;
			
			case Estate_4_5:
				if (dir == FORWARD){			// El estado intermedio cuatro es una secuencia de selección
					c.imag = -c.imag;			// si dir == FORWARD realiza la instrucción 512.
				    c.real = sqrt( ( 1.0 + c.real ) / 2.0 );
				    l++;
					state = Estate_3;		
				} else{
				    c.real = sqrt( ( 1.0 + c.real ) / 2.0 );
				    l++;
					state = Estate_3;
				}
			break;
				
			case Estate_5:
				if (i < points){				//  Si i < points pasa al estado cinco de otra forma pasa al estdo 4.
		            i1    = i + l1;
		            temp  = cplx_Multiply( u, x[i1] );
		            x[i1] = cplx_Subtract( x[i], temp );
		            x[i]  = cplx_Add( x[i], temp );
					i += l2;	
		            state = Estate_5;
				}	else{
					u = cplx_Multiply( u, c );
					j++;
					state = Estate_4;
				}
			break;
				
			case Estate_6:					// Secuencia de selección, si dir == FORWARD pasa al estdo 7 
				if( dir == FORWARD ){		// De otra forma se termina la maquina de estados
					state = Estate_7;
					i = 0;
				} else{
					finish = 1;
				}
			break;
				
			case Estate_7:					// Si i < points permanece en el estado 7 de otra forma se termina
				if (i < points){			// la maquina de estados.
					x[i] = cplx_Scale( x[i], points );
					state = Estate_7;
					i++;
				} else{
					finish = 1;
				}
			break;
    	} 	
	
	}
	return( SUCCEED );
} /* Compute_FFT */



/*FN****************************************************************************
*
*   int
*   Find_Power( int number );
*
*   Purpose:         Return the (integer) logarithm in base 2 of a number
*
*   Note:            Number must be an integer equal to zero or greater
*                    Another option is shifting and truncation
*
*   Register of Revisions (Debugging Process):
*
*   DATE       RESPONSIBLE  COMMENT
*   -----------------------------------------------------------------------
*   May 03/13  J.C.Giraldo  Readable identifiers
*   --- --/--  P. Bourkes   Initial implementation
*
*******************************************************************************/

int
Find_Power( long number )
{
int power = 0;

while( number != 1 ) {
    number = (long)( number/2 );
    power++;
}

return( power );

} /* Find_Power */
