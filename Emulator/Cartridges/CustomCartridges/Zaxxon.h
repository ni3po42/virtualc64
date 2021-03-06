// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _ZAXXON_H
#define _ZAXXON_H

#include "Cartridge.h"

class Zaxxon : public Cartridge {
    
public:
    
    Zaxxon(C64 &ref) : Cartridge(ref, "Zaxxon") { };
    CartridgeType getCartridgeType() override { return CRT_ZAXXON; }

private:
    
    void _reset() override;

    
    //
    // Accessing cartridge memory
    //
    
public:
        
    u8 peekRomL(u16 addr) override;
    u8 spypeekRomL(u16 addr) override;
};

#endif
