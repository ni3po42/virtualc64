/*!
 * @header      Datasette.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2015 - 2016 Dirk W. Hoffmann
 * @brief       Declares Datasette class
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

Datasette::Datasette()
{
    setDescription("Datasette");
    debug(3, "Creating virtual datasette at address %p\n", this);
        
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Tape properties (will survive reset)
        { &size,               sizeof(size),              KEEP_ON_RESET },
        { &type,               sizeof(type),              KEEP_ON_RESET },
        { &durationInCycles,   sizeof(durationInCycles),  KEEP_ON_RESET },
        
        // Internal state (will be cleared on reset)
        { &head,               sizeof(head),              CLEAR_ON_RESET },
        { &headInCycles,       sizeof(headInCycles),      CLEAR_ON_RESET },
        { &headInSeconds,      sizeof(headInSeconds),     CLEAR_ON_RESET },
        { &nextRisingEdge,     sizeof(nextRisingEdge),    CLEAR_ON_RESET },
        { &nextFallingEdge,    sizeof(nextFallingEdge),   CLEAR_ON_RESET },
        { &playKey,            sizeof(playKey),           CLEAR_ON_RESET },
        { &motor,              sizeof(motor),             CLEAR_ON_RESET },
        
        { NULL,                0,                         0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

Datasette::~Datasette()
{
    debug(3, "Releasing Datasette...\n");

    if (data)
        delete[] data;
}

void
Datasette::reset()
{
    VirtualComponent::reset();
    rewind();
}

void
Datasette::ping()
{
    VirtualComponent::ping();
    c64->putMessage(hasTape() ? MSG_VC1530_TAPE : MSG_VC1530_NO_TAPE);
    c64->putMessage(MSG_VC1530_PROGRESS);
}

size_t
Datasette::stateSize()
{
    return VirtualComponent::stateSize() + size;
}

void
Datasette::didLoadFromBuffer(uint8_t **buffer)
{
    if (data) delete[] data;
    
    if (size) {
        data = new uint8_t[size];
        readBlock(buffer, data, size);
    }
}

void
Datasette::didSaveToBuffer(uint8_t **buffer)
{
    if (size) {
        assert(data != NULL);
        writeBlock(buffer, data, size);
    }
}

void
Datasette::setHeadInCycles(uint64_t value)
{
    printf("Fast forwarding to cycle %lld (duration %lld)\n", value, durationInCycles);
    rewind();
    while (headInCycles <= value && head < size) advanceHead(true);
    printf("Head is %llu (max %llu)\n", head, size);
}

bool
Datasette::insertTape(TAPFile *a)
{
    suspend();
    
    size = a->getSize();
    type = a->TAPversion();
    
    debug(2, "Inserting tape (size = %d, type = %d)...\n", size, type);
    
    // Copy data
    data = (uint8_t *)malloc(size);
    memcpy(data, a->getData(), size);

    // Determine tape length (by fast forwarding)
    rewind();
    while (head < size)
        advanceHead(true /* Don't send tape progress messages */);

    durationInCycles = headInCycles;
    rewind();
    
    c64->putMessage(MSG_VC1530_TAPE);
    resume();
    
    return true;
}

void
Datasette::ejectTape()
{
    suspend();
    
    debug(2, "Ejecting tape\n");

    if (!hasTape())
        return;
    
    pressStop();
    
    assert(data != NULL);
    free(data);
    data = NULL;
    size = 0;
    type = 0;
    durationInCycles = 0;
    head = -1;

    c64->putMessage(MSG_VC1530_NO_TAPE);
    resume();
}

void
Datasette::advanceHead(bool silent)
{
    assert(head < size);
    
    // Update head and headInCycles
    int length, skip;
    length = pulseLength(&skip);
    head += skip;
    headInCycles += length;
    
    // Send message if the tapeCounter (in seconds) changes
    uint32_t newHeadInSeconds = (uint32_t)(headInCycles / c64->frequency);
    if (newHeadInSeconds != headInSeconds && !silent) {
        // debug("Tape counter: %d (%llu out of %llu)\n", newHeadInSeconds, head, size);
        c64->putMessage(MSG_VC1530_PROGRESS);
    }

    // Update headInSeconds
    headInSeconds = newHeadInSeconds;
}

int
Datasette::pulseLength(int *skip)
{
    assert(head < size);

    if (data[head] != 0) {
        // Pulse lengths between 1 * 8 and 255 * 8
        if (skip) *skip = 1;
        return 8 * data[head];
    }
    
    if (type == 0) {
        // Pulse lengths greater than 8 * 255 (TAP V0 files)
        if (skip) *skip = 1;
        return 8 * 256;
    } else {
        // Pulse lengths greater than 8 * 255 (TAP V1 files)
        if (skip) *skip = 4;
        if (head + 3 < size) {
            return  LO_LO_HI_HI(data[head+1], data[head+2], data[head+3], 0);
        } else {
            debug("TAP file ended unexpectedly (%d, %d)\n", size, head + 3);
            assert(false);
            return 8 * 256;
        }
    }
}

void
Datasette::pressPlay()
{
    if (!hasTape() || head >= size)
        return;
 
    debug("Datasette::pressPlay\n");
    playKey = true;

    // Schedule first pulse
    uint64_t length = pulseLength();
    nextRisingEdge = length / 2;
    nextFallingEdge = length;
    advanceHead();
}

void
Datasette::pressStop()
{
    debug("Datasette::pressStop\n");
    setMotor(false);
    playKey = false;
}

void
Datasette::setMotor(bool value)
{
    if (motor == value)
        return;
    
    motor = value;
}

void
Datasette::_execute()
{
    // Only proceed if the datasette is active
    if (!hasTape() || !playKey || !motor) return;
        
    if (--nextRisingEdge == 0) {
        
        c64->cia1.triggerRisingEdgeOnFlagPin();
    }

    if (--nextFallingEdge == 0) {
        
        c64->cia1.triggerFallingEdgeOnFlagPin();

        if (head < size) {

            // Schedule the next pulse
            uint64_t length = pulseLength();
            nextRisingEdge = length / 2;
            nextFallingEdge = length;
            advanceHead();
            
        } else {
            
            // Press the stop key
            pressStop();
        }
    }
}