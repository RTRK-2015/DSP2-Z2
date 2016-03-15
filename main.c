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

void stereoToMono(Int16* inputL, Int16* inputR, Int16* output, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		output[i] = inputL[i]>>1 + inputR[i]>>1;
	}
}

void main( void )
{
	int i;
    int state=0;

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


 	while(1)
	{

 		/* Promeniti stanje na osnovu pritisnutog tastera */
 		/* Za dobavljanje vrednosti tastera koristiti funkciju EZDSP5535_SAR_getKey */
 		/* Your code here */

 		aic3204_read_block(InputBufferL, InputBufferR);
 		stereoToMono(InputBufferL, InputBufferR, InputBufferMono, AUDIO_IO_SIZE);

 		/* Izvrsiti analizu signala InputBufferMono upotrebom filter banke 4. reda */
 		/* Your code here */

 		/* U zavisnosti od trenutnog stanja nulirati odredjene komponente signala */
 		/* Stanje 0 - nema obrade (komponente signala na ulazu nalaze se i na izlazu) */
 		/* Stanje 1 - samo frekvencijske komponente iz prvog podopsega */
 		/* Stanje 2 - samo frekvencijske komponente iz drugog podopsega */
 		/* Stanje 3 - samo frekvencijske komponente iz treceg podopsega */
 		/* Stanje 4 - samo frekvencijske komponente iz cetvrtog podopsega */
 		/* Your code here */

 		/* Izvrsiti sintezu signala i rezultat smestiti u OutputBufferMono */
 		/* Your code here */

		aic3204_write_block(OutputBufferMono, OutputBufferMono);
	}


	/* Prekid veze sa AIC3204 kodekom */
    aic3204_disable();

    printf( "\n***Kraj programa***\n" );
	SW_BREAKPOINT;
}


