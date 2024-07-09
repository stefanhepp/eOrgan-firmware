/**
 * Author: Stefan hepp (stefan@stefant.org)
 *
 * Minimalistic MIDI library implementation.
 **/

#ifndef __MiniMIDI_H__
#define __MiniMIDI_H__

#include <inttypes.h>

/**
 * Implementation of the Wire interface for AtMega 8bit controllers.
 * 
 * Only supports a single hardware TWI interface.
 * Only supports a single instantiation of this class.
 */
class MiniMIDI
{
    private:

    public:
        MiniMIDI();

        void begin(uint8_t channel);

};

#endif
