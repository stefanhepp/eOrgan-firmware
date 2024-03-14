/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: adc_const.h 109 2007-08-04 21:22:53Z stefan $
 *
 * Constant definitions for ADC driver.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __AVRLIB_ADC_ADC_CONST_H__
#define __AVRLIB_ADC_ADC_CONST_H__

#include <avr/io.h>

/* ==================================================== */
/* Constants                                            */
/* ==================================================== */

// ADC References
#define ADC_REF_AREF		0
#define ADC_REF_AVCC		(1<<REFS0)
#define ADC_REF_INTERNAL	((1<<REFS1)|(1<<REFS0))

// ADC Input Sources
#define ADC_MUX_SINGLE_ADC0	0
#define ADC_MUX_SINGLE_ADC1	1
#define ADC_MUX_SINGLE_ADC2	2
#define ADC_MUX_SINGLE_ADC3	3
#define ADC_MUX_SINGLE_ADC4	4
#define ADC_MUX_SINGLE_ADC5	5
#define ADC_MUX_SINGLE_ADC6	6
#define ADC_MUX_SINGLE_ADC7	7

// ADC Differential Input (ADC<positive><negative>[_<gain>]
// TODO define rest 
#define ADC_MUX_ADC00_10x	8
#define ADC_MUX_ADC10_10x	9


// ADC Prescaler settings
#define ADC_PRESCALE_2		(1<<ADPS0)
#define ADC_PRESCALE_4		(1<<ADPS1)
#define ADC_PRESCALE_8		((1<<ADPS1)|(1<<ADPS0))
#define ADC_PRESCALE_16		(1<<ADPS2)
#define ADC_PRESCALE_32		((1<<ADPS2)|(1<<ADPS0))
#define ADC_PRESCALE_62		((1<<ADPS2)|(1<<ADPS1))
#define ADC_PRESCALE_128	((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0))

// ADC Trigger source mode
// TODO define rest
#define ADC_MODE_SINGLE		0xFF
#define ADC_MODE_FREERUN	0
#define ADC_MODE_COMPARATOR	(1<<ADTS0)
#define ADC_MODE_EXTINT0	(1<<ADTS1)

#endif
