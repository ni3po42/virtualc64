// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

C64Memory::C64Memory(C64 &ref) : Memory(ref)
{	
	setDescription("C64 memory");
    		
    memset(rom, 0, sizeof(rom));
    stack = &ram[0x0100];
    
    // Register snapshot items
    SnapshotItem items[] = {

        { ram,             sizeof(ram),            KEEP_ON_RESET },
        { colorRam,        sizeof(colorRam),       KEEP_ON_RESET },
        { &rom[0xA000],    0x2000,                 KEEP_ON_RESET }, /* Basic ROM */
        { &rom[0xD000],    0x1000,                 KEEP_ON_RESET }, /* Character ROM */
        { &rom[0xE000],    0x2000,                 KEEP_ON_RESET }, /* Kernal ROM */
        { &ramInitPattern, sizeof(ramInitPattern), KEEP_ON_RESET },
        { &peekSrc,        sizeof(peekSrc),        KEEP_ON_RESET },
        { &pokeTarget,     sizeof(pokeTarget),     KEEP_ON_RESET },
        { NULL,            0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
    ramInitPattern = INIT_PATTERN_C64;
    
    // Setup the C64's memory bank map
    
    // If x = (EXROM, GAME, CHAREN, HIRAM, LORAM), then
    //   map[x][0] = mapping for range $1000 - $7FFF
    //   map[x][1] = mapping for range $8000 - $9FFF
    //   map[x][2] = mapping for range $A000 - $BFFF
    //   map[x][3] = mapping for range $C000 - $CFFF
    //   map[x][4] = mapping for range $D000 - $DFFF
    //   map[x][5] = mapping for range $E000 - $FFFF
    MemoryType map[32][6] = {
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_CRTHI, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_CRTLO, M_CRTHI, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_CRTHI, M_RAM,  M_IO,   M_KERNAL},
        {M_RAM,  M_CRTLO, M_CRTHI, M_RAM,  M_IO,   M_KERNAL},
        
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_CRTLO, M_BASIC, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_KERNAL},
        {M_RAM,  M_CRTLO, M_BASIC, M_RAM,  M_IO,   M_KERNAL},
        
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_BASIC, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_KERNAL},
        {M_RAM,  M_RAM,   M_BASIC, M_RAM,  M_IO,   M_KERNAL}
    };
    
    for (unsigned i = 0; i < 32; i++) {
        bankMap[i][0x0] = M_PP;
        bankMap[i][0x1] = map[i][0];
        bankMap[i][0x2] = map[i][0];
        bankMap[i][0x3] = map[i][0];
        bankMap[i][0x4] = map[i][0];
        bankMap[i][0x5] = map[i][0];
        bankMap[i][0x6] = map[i][0];
        bankMap[i][0x7] = map[i][0];
        bankMap[i][0x8] = map[i][1];
        bankMap[i][0x9] = map[i][1];
        bankMap[i][0xA] = map[i][2];
        bankMap[i][0xB] = map[i][2];
        bankMap[i][0xC] = map[i][3];
        bankMap[i][0xD] = map[i][4];
        bankMap[i][0xE] = map[i][5];
        bankMap[i][0xF] = map[i][5];
    }
    
    // Initialize peekSource and pokeTarket tables
    peekSrc[0x0] = pokeTarget[0x0] = M_PP;
    for (unsigned i = 0x1; i <= 0xF; i++) {
        peekSrc[i] = pokeTarget[i] = M_RAM;
    }
}

C64Memory::~C64Memory()
{
}

void
C64Memory::reset()
{
    HardwareComponent::reset();
    
    // Erase RAM
    eraseWithPattern(ramInitPattern);
        
    // Initialize color RAM with random numbers
    srand(1000);
    for (unsigned i = 0; i < sizeof(colorRam); i++) {
        colorRam[i] = (rand() & 0xFF);
    }
}

void 
C64Memory::dump()
{
	msg("C64 Memory:\n");
	msg("-----------\n");
	msg("    Basic ROM: %s loaded\n", basicRomIsLoaded() ? "" : " not");
	msg("Character ROM: %s loaded\n", characterRomIsLoaded() ? "" : " not");
    msg("   Kernal ROM: %s loaded\n", kernalRomIsLoaded() ? "" : " not");
	
    for (u16 addr = 0; addr < 0xFFFF; addr++) {
        if (cpu.hardBreakpoint(addr))
			msg("Hard breakpoint at %04X\n", addr);
        if (cpu.softBreakpoint(addr))
            msg("Soft breakpoint at %04X\n", addr);
	}
	msg("\n");
    
    /*
    for (u16 addr = 0x1000; addr < 0xB000; addr += 0x400) {
        msg("%04X: ", addr);
        for (unsigned i = 0; i < 30; i++) {
            msg("%02X ", ram[addr + i]);
        }
        msg("\n");
    }
    */
}

void
C64Memory::eraseWithPattern(RamInitPattern pattern)
{
    if (!isRamInitPattern(pattern)) {
        warn("Unknown RAM init pattern. Assuming INIT_PATTERN_C64\n");
        pattern = INIT_PATTERN_C64;
    }
    
    if (pattern == INIT_PATTERN_C64) {
        for (unsigned i = 0; i < sizeof(ram); i++)
            ram[i] = (i & 0x40) ? 0xFF : 0x00;
    } else {
        for (unsigned i = 0; i < sizeof(ram); i++)
            ram[i] = (i & 0x80) ? 0x00 : 0xFF;
    }
    
    // Make the screen look nice on startup
    memset(&ram[0x400], 0x01, 40*25);
}

void 
C64Memory::updatePeekPokeLookupTables()
{
    // Read game line, exrom line, and processor port bits
    u8 game  = expansionport.getGameLine() ? 0x08 : 0x00;
    u8 exrom = expansionport.getExromLine() ? 0x10 : 0x00;
    u8 index = (pport.read() & 0x07) | exrom | game;

    // Set ultimax flag
    vc64.setUltimax(exrom && !game);

    // Update table entries
    for (unsigned bank = 1; bank < 16; bank++) {
        peekSrc[bank] = pokeTarget[bank] = bankMap[index][bank];
    }
    
    // Call the Cartridge's delegation method
    expansionport.updatePeekPokeLookupTables();
}

u8
C64Memory::peek(u16 addr, MemoryType source)
{
    switch(source) {
        
        case M_RAM:
        return ram[addr];
        
        case M_ROM:
        return rom[addr];
        
        case M_IO:
        return peekIO(addr);
        
        case M_CRTLO:
        case M_CRTHI:
        return expansionport.peek(addr);
        
        case M_PP:
        if (likely(addr >= 0x02)) {
            return ram[addr];
        } else if (addr == 0x00) {
            return pport.readDirection();
        } else {
            return pport.read();
        }
        
        case M_NONE:
        return vic.getDataBusPhi1();
        
        default:
        assert(0);
        return 0;
    }
}

u8
C64Memory::peek(u16 addr, bool gameLine, bool exromLine)
{
    u8 game  = gameLine ? 0x08 : 0x00;
    u8 exrom = exromLine ? 0x10 : 0x00;
    u8 index = (pport.read() & 0x07) | exrom | game;
    
    return peek(addr, bankMap[index][addr >> 12]);
}

u8
C64Memory::peekZP(u8 addr)
{
    if (likely(addr >= 0x02)) {
        return ram[addr];
    } else if (addr == 0x00) {
        return pport.readDirection();
    } else {
        return pport.read();
    }
}

u8
C64Memory::peekIO(u16 addr)
{
    assert(addr >= 0xD000 && addr <= 0xDFFF);
    
    switch ((addr >> 8) & 0xF) {
            
        case 0x0: // VIC
        case 0x1: // VIC
        case 0x2: // VIC
        case 0x3: // VIC
            
            // Only the lower 6 bits are used for adressing the VIC I/O space.
            // As a result, VIC's I/O memory repeats every 64 bytes.
            return vic.peek(addr & 0x003F);

        case 0x4: // SID
        case 0x5: // SID
        case 0x6: // SID
        case 0x7: // SID
            
            // Only the lower 5 bits are used for adressing the SID I/O space.
            // As a result, SID's I/O memory repeats every 32 bytes.
            return sid.peek(addr & 0x001F);

        case 0x8: // Color RAM
        case 0x9: // Color RAM
        case 0xA: // Color RAM
        case 0xB: // Color RAM
 
            return (colorRam[addr - 0xD800] & 0x0F) | (vic.getDataBusPhi1() & 0xF0);

        case 0xC: // CIA 1
 
            // Only the lower 4 bits are used for adressing the CIA I/O space.
            // As a result, CIA's I/O memory repeats every 16 bytes.
            return cia1.peek(addr & 0x000F);
	
        case 0xD: // CIA 2
            
            return cia2.peek(addr & 0x000F);
            
        case 0xE: // I/O space 1
            
            return expansionport.peekIO1(addr);
            
        case 0xF: // I/O space 2

            return expansionport.peekIO2(addr);
	}
    
	assert(false);
	return 0;
}

u8
C64Memory::spypeek(u16 addr, MemoryType source)
{
    switch(source) {
            
        case M_RAM:
            return ram[addr];
            
        case M_ROM:
            return rom[addr];
            
        case M_IO:
            return spypeekIO(addr);
            
        case M_CRTLO:
        case M_CRTHI:
            return expansionport.spypeek(addr);
            
        case M_PP:
            return peek(addr, M_PP);
      
        case M_NONE:
            return ram[addr];
            
        default:
            assert(0);
            return 0;
    }
}

u8
C64Memory::spypeekIO(u16 addr)
{
    assert(addr >= 0xD000 && addr <= 0xDFFF);
    
    switch ((addr >> 8) & 0xF) {
            
        case 0x0: // VIC
        case 0x1: // VIC
        case 0x2: // VIC
        case 0x3: // VIC
            
            return vic.spypeek(addr & 0x003F);
            
        case 0x4: // SID
        case 0x5: // SID
        case 0x6: // SID
        case 0x7: // SID
            
            return sid.spypeek(addr & 0x001F);
            
        case 0xC: // CIA 1
            
            // Only the lower 4 bits are used for adressing the CIA I/O space.
            // As a result, CIA's I/O memory repeats every 16 bytes.
            return cia1.spypeek(addr & 0x000F);
            
        case 0xD: // CIA 2
            
            return cia2.spypeek(addr & 0x000F);
            
        case 0xE: // I/O space 1
            
            return expansionport.spypeekIO1(addr);
            
        case 0xF: // I/O space 2
            
            return expansionport.spypeekIO2(addr);

        default:
            
            return peek(addr);
    }
}

void
C64Memory::poke(u16 addr, u8 value, MemoryType target)
{
    switch(target) {
            
        case M_RAM:
        case M_ROM:
            ram[addr] = value;
            return;
            
        case M_IO:
            pokeIO(addr, value);
            return;
            
        case M_CRTLO:
        case M_CRTHI:
            expansionport.poke(addr, value);
            return;
            
        case M_PP:
            if (likely(addr >= 0x02)) {
                ram[addr] = value;
            } else if (addr == 0x00) {
                pport.writeDirection(value);
            } else {
                pport.write(value);
            }
            return;
            
        case M_NONE:
            return;
            
        default:
            assert(0);
            return;
    }
}

void
C64Memory::poke(u16 addr, u8 value, bool gameLine, bool exromLine)
{
    u8 game  = gameLine ? 0x08 : 0x00;
    u8 exrom = exromLine ? 0x10 : 0x00;
    u8 index = (pport.read() & 0x07) | exrom | game;
    
    poke(addr, value, bankMap[index][addr >> 12]);
}

void
C64Memory::pokeZP(u8 addr, u8 value)
{
    if (likely(addr >= 0x02)) {
        ram[addr] = value;
    } else if (addr == 0x00) {
        pport.writeDirection(value);
    } else {
        pport.write(value);
    }
}

void
C64Memory::pokeIO(u16 addr, u8 value)
{
    assert(addr >= 0xD000 && addr <= 0xDFFF);
    
    switch ((addr >> 8) & 0xF) {
            
        case 0x0: // VIC
        case 0x1: // VIC
        case 0x2: // VIC
        case 0x3: // VIC
            
            // Only the lower 6 bits are used for adressing the VIC I/O space.
            // As a result, VIC's I/O memory repeats every 64 bytes.
            vic.poke(addr & 0x003F, value);
            return;
            
        case 0x4: // SID
        case 0x5: // SID
        case 0x6: // SID
        case 0x7: // SID
            
            // Only the lower 5 bits are used for adressing the SID I/O space.
            // As a result, SID's I/O memory repeats every 32 bytes.
            sid.poke(addr & 0x001F, value);
            return;
            
        case 0x8: // Color RAM
        case 0x9: // Color RAM
        case 0xA: // Color RAM
        case 0xB: // Color RAM
            
            colorRam[addr - 0xD800] = (value & 0x0F) | (rand() & 0xF0);
            return;
            
        case 0xC: // CIA 1
            
            // Only the lower 4 bits are used for adressing the CIA I/O space.
            // As a result, CIA's I/O memory repeats every 16 bytes.
            cia1.poke(addr & 0x000F, value);
            return;
            
        case 0xD: // CIA 2
            
            cia2.poke(addr & 0x000F, value);
            return;
            
        case 0xE: // I/O space 1
            
            expansionport.pokeIO1(addr, value);
            return;
            
        case 0xF: // I/O space 2
            
            expansionport.pokeIO2(addr, value);
            return;
    }
    
    assert(false);
}

u16
C64Memory::nmiVector() {
    
    if (peekSrc[0xF] != M_ROM || kernalRomIsLoaded()) {
        return LO_HI(peek(0xFFFA), peek(0xFFFB));
    } else {
        return 0xFE43;
    }
}

u16
C64Memory::irqVector() {
    
    if (peekSrc[0xF] != M_ROM || kernalRomIsLoaded()) {
        return LO_HI(peek(0xFFFE), peek(0xFFFF));
    } else {
        return 0xFF48;
    }
}

u16
C64Memory::resetVector() {
    
    if (peekSrc[0xF] != M_ROM || kernalRomIsLoaded()) {
        return LO_HI(peek(0xFFFC), peek(0xFFFD));
    } else {
        return 0xFCE2;
    }
}

