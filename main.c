//////////////////////////////////////////////////////////////////////////////
// *
// * VEŽBA 2 – Banke filtara
// * Godina: 2016
// *
// * Zadatak: Banke filtara
// * Autor:
// *
// *
//////////////////////////////////////////////////////////////////////////////

#include "stdio.h"
#include "ezdsp5535.h"
#include "ezdsp5535_i2c.h"
#include "aic3204.h"
#include "ezdsp5535_aic3204_dma.h"
#include "ezdsp5535_i2s.h"
#include "ezdsp5535_sar.h"
#include "fir.h"
#include "filter_coeff.h"
#include "processing.h"
#include "string.h"

/* Frekvencija odabiranja */
#define SAMPLE_RATE 8000L
#define GAIN 0

/* Niz za smestanje ulaznih i izlaznih odbiraka */
#pragma DATA_ALIGN(InputBufferL,4)
Int16 InputBufferL[AUDIO_IO_SIZE];
#pragma DATA_ALIGN(InputBufferR,4)
Int16 InputBufferR[AUDIO_IO_SIZE];

#pragma DATA_ALIGN(InputBufferMono,4)
Int16 InputBufferMono[AUDIO_IO_SIZE];
#pragma DATA_ALIGN(OutputBufferMono,4)
Int16 OutputBufferMono[AUDIO_IO_SIZE];



Int16 zHI0[AUDIO_IO_SIZE];
Int16 zHI1[AUDIO_IO_SIZE];
Int16 zHO0[AUDIO_IO_SIZE];
Int16 zHO1[AUDIO_IO_SIZE];
Int16 zHI00[AUDIO_IO_SIZE];
Int16 zHI01[AUDIO_IO_SIZE];
Int16 zHI10[AUDIO_IO_SIZE];
Int16 zHI11[AUDIO_IO_SIZE];
Int16 zHO00[AUDIO_IO_SIZE];
Int16 zHO01[AUDIO_IO_SIZE];
Int16 zHO10[AUDIO_IO_SIZE];
Int16 zHO11[AUDIO_IO_SIZE];

Int16 TempF0[AUDIO_IO_SIZE];
Int16 TempF1[AUDIO_IO_SIZE];
Int16 TempS0[AUDIO_IO_SIZE];
Int16 TempS1[AUDIO_IO_SIZE];
Int16 TempProcess00[AUDIO_IO_SIZE];
Int16 TempProcess01[AUDIO_IO_SIZE];
Int16 TempProcess10[AUDIO_IO_SIZE];
Int16 TempProcess11[AUDIO_IO_SIZE];

Int16 TempFinalOut0[AUDIO_IO_SIZE];
Int16 TempFinalOut1[AUDIO_IO_SIZE];


void stereoToMono(Int16* inputL, Int16* inputR, Int16* output, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		output[i] = (inputL[i]>>1) + (inputR[i]>>1);
	}
}

void main( void )
{
	int i;
    int state=1;

    /* Inicijalizaija razvojne ploce */
    EZDSP5535_init( );

    /* Inicijalizacija kontrolera za ocitavanje vrednosti dugmeta*/
    EZDSP5535_SAR_init();

	printf("\n DTMF Predajnik\n");

    /* Inicijalizacija veze sa AIC3204 kodekom (AD/DA) */
    aic3204_hardware_init();

    /* Inicijalizacija AIC3204 kodeka */
	aic3204_init();

    aic3204_dma_init();

    /* Postavljanje vrednosti frekvencije odabiranja i pojacanja na kodeku */
    set_sampling_frequency_and_gain(SAMPLE_RATE, GAIN);

    /* Empty z buffers */
    memset(zHI0, 0, AUDIO_IO_SIZE);
    memset(zHI1, 0, AUDIO_IO_SIZE);
    memset(zHO0, 0, AUDIO_IO_SIZE);
    memset(zHO1, 0, AUDIO_IO_SIZE);
    memset(zHI00, 0, AUDIO_IO_SIZE);
    memset(zHI01, 0, AUDIO_IO_SIZE);
    memset(zHI10, 0, AUDIO_IO_SIZE);
    memset(zHI11, 0, AUDIO_IO_SIZE);
    memset(zHO00, 0, AUDIO_IO_SIZE);
    memset(zHO01, 0, AUDIO_IO_SIZE);
    memset(zHO10, 0, AUDIO_IO_SIZE);
    memset(zHO11, 0, AUDIO_IO_SIZE);


 	while(1)
	{

 		/* Promeniti stanje na osnovu pritisnutog tastera */
 		/* Za dobavljanje vrednosti tastera koristiti funkciju EZDSP5535_SAR_getKey */
 		/* Your code here */

 		aic3204_read_block(InputBufferL, InputBufferR);
 		stereoToMono(InputBufferL, InputBufferR, InputBufferMono, AUDIO_IO_SIZE);

 		/* Izvrsiti analizu signala InputBufferMono upotrebom filter banke 4. reda */
 		fir_basic(InputBufferMono, H0, zHI0, N_COEFF, AUDIO_IO_SIZE, TempF0);
 		fir_basic(InputBufferMono, H1, zHI1, N_COEFF, AUDIO_IO_SIZE, TempF1);
 		downsample(TempF0, TempS0, 2, AUDIO_IO_SIZE);
 		downsample(TempF1, TempS1, 2, AUDIO_IO_SIZE);

 		/* Split again */
 		fir_basic(TempS0, H0, zHI00, N_COEFF, AUDIO_IO_SIZE, TempF0);
 		fir_basic(TempS0, H1, zHI01, N_COEFF, AUDIO_IO_SIZE, TempF1);
 		downsample(TempF0, TempProcess00, 2, AUDIO_IO_SIZE);
 		downsample(TempF1, TempProcess01, 2, AUDIO_IO_SIZE);

 		fir_basic(TempS1, H0, zHI10, N_COEFF, AUDIO_IO_SIZE, TempF0);
 		fir_basic(TempS1, H1, zHI11, N_COEFF, AUDIO_IO_SIZE, TempF1);
 		downsample(TempF0, TempProcess10, 2, AUDIO_IO_SIZE);
 		downsample(TempF1, TempProcess11, 2, AUDIO_IO_SIZE);



 		/* U zavisnosti od trenutnog stanja nulirati odredjene komponente signala */
 		/* Stanje 0 - nema obrade (komponente signala na ulazu nalaze se i na izlazu) */
 		/* Stanje 1 - samo frekvencijske komponente iz prvog podopsega */
 		/* Stanje 2 - samo frekvencijske komponente iz drugog podopsega */
 		/* Stanje 3 - samo frekvencijske komponente iz treceg podopsega */
 		/* Stanje 4 - samo frekvencijske komponente iz cetvrtog podopsega */
 		/* Your code here */
 		switch (state) {
 			case 1:
				memset(TempProcess01, 0, AUDIO_IO_SIZE);
				memset(TempProcess10, 0, AUDIO_IO_SIZE);
				memset(TempProcess11, 0, AUDIO_IO_SIZE);
				break;

 			case 2:
				memset(TempProcess00, 0, AUDIO_IO_SIZE);
				memset(TempProcess10, 0, AUDIO_IO_SIZE);
				memset(TempProcess11, 0, AUDIO_IO_SIZE);
				break;

 			case 3:
				memset(TempProcess00, 0, AUDIO_IO_SIZE);
				memset(TempProcess01, 0, AUDIO_IO_SIZE);
				memset(TempProcess11, 0, AUDIO_IO_SIZE);
				break;

 			case 4:
				memset(TempProcess00, 0, AUDIO_IO_SIZE);
				memset(TempProcess01, 0, AUDIO_IO_SIZE);
				memset(TempProcess10, 0, AUDIO_IO_SIZE);
				break;
 		}




 		/* Izvrsiti sintezu signala i rezultat smestiti u OutputBufferMono */
 		upsample(TempProcess00, TempF0, 2, AUDIO_IO_SIZE / 2);
 		upsample(TempProcess01, TempF1, 2, AUDIO_IO_SIZE / 2);
 		fir_basic(TempF0, H0, zHO00, N_COEFF, AUDIO_IO_SIZE, TempS0);
 		fir_basic(TempF1, H1, zHO01, N_COEFF, AUDIO_IO_SIZE, TempS1);
 		for (i = 0; i < AUDIO_IO_SIZE; i++) {
 			TempFinalOut0[i] = TempS0[i] + TempS1[i];
 		}

 		upsample(TempProcess10, TempF0, 2, AUDIO_IO_SIZE / 2);
 		upsample(TempProcess11, TempF1, 2, AUDIO_IO_SIZE / 2);
 		fir_basic(TempF0, H0, zHO10, N_COEFF, AUDIO_IO_SIZE, TempS0);
 		fir_basic(TempF1, H1, zHO11, N_COEFF, AUDIO_IO_SIZE, TempS1);
 		for (i = 0; i < AUDIO_IO_SIZE; i++) {
 			TempFinalOut1[i] = TempS0[i] + TempS1[i];
 		}

 		upsample(TempFinalOut0, TempF0, 2, AUDIO_IO_SIZE / 2);
 		upsample(TempFinalOut1, TempF1, 2, AUDIO_IO_SIZE / 2);
 		fir_basic(TempF0, H0, zHO0, N_COEFF, AUDIO_IO_SIZE, TempS0);
 		fir_basic(TempF1, H1, zHO1, N_COEFF, AUDIO_IO_SIZE, TempS1);
 		for (i = 0; i < AUDIO_IO_SIZE; i++) {
 			OutputBufferMono[i] = TempS0[i] + TempS1[i];
 		}


		aic3204_write_block(OutputBufferMono, OutputBufferMono);
 		// aic3204_write_block(InputBufferMono, InputBufferMono);
	}


	/* Prekid veze sa AIC3204 kodekom */
    aic3204_disable();

    printf( "\n***Kraj programa***\n" );
	SW_BREAKPOINT;
}


