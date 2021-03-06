// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

C64Component::C64Component(C64& ref) :

c64(ref),
mem(ref.mem),
cpu(ref.cpu),
vic(ref.vic),
cia1(ref.cia1),
cia2(ref.cia2),
sid(ref.sid),
keyboard(ref.keyboard),
port1(ref.port1),
port2(ref.port2),
expansionport(ref.expansionport),
iec(ref.iec),
drive8(ref.drive8),
drive9(ref.drive9),
datasette(ref.datasette),
mouse(ref.mouse),
messageQueue(ref.messageQueue)
{
};

void
C64Component::suspend()
{
    c64.suspend();
}

void
C64Component::resume()
{
    c64.resume();
}

void
C64Component::prefix()
{
    c64.prefix();
}
