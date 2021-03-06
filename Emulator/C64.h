// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _C64_H
#define _C64_H

// General
#include "C64Component.h"
#include "Serialization.h"
#include "MessageQueue.h"

// Configuration items
#include "C64Config.h"

// Data types and constants
#include "C64Types.h"

// Loading and saving
#include "Snapshot.h"
#include "T64File.h"
#include "D64File.h"
#include "G64File.h"
#include "PRGFile.h"
#include "PRGFolder.h"
#include "P00File.h"
#include "RomFile.h"
#include "TAPFile.h"
#include "CRTFile.h"

// Sub components
#include "ExpansionPort.h"
#include "IEC.h"
#include "Keyboard.h"
#include "ControlPort.h"
#include "C64Memory.h"
#include "DriveMemory.h"
#include "FlashRom.h"
#include "VICII.h"
#include "SIDBridge.h"
#include "TOD.h"
#include "CIA.h"
#include "CPU.h"

// Cartridges
#include "Cartridge.h"
#include "CustomCartridges.h"

// Peripherals
#include "Drive.h"
#include "Datasette.h"
#include "Mouse.h"


/* A complete virtual C64. This class is the most prominent one of all. To run
 * the emulator, it is sufficient to create a single object of this type. All
 * subcomponents are created automatically. The public API gives you control
 * over the emulator's behaviour such as running and pausing the emulation.
 * Please note that most subcomponents have their own public API. E.g., to
 * query information from VICII, you need to invoke a method on c64.vicii.
 */
class C64 : public HardwareComponent {
        
    // The currently set inspection target (only evaluated in debug mode)
    InspectionTarget inspectionTarget;

    
    //
    // Sub components
    //
    
public:
    
    // Memory (ROM, RAM and color RAM)
    C64Memory mem = C64Memory(*this);
    
    // CPU
    C64CPU cpu = C64CPU(*this, mem);
        
    // Video Interface Controller
    VICII vic = VICII(*this);
    
    // Complex Interface Adapters
    CIA1 cia1 = CIA1(*this);
    CIA2 cia2 = CIA2(*this);
    
    // Sound Interface Device
    SIDBridge sid = SIDBridge(*this);
    
    // Keyboard
    Keyboard keyboard = Keyboard(*this);
    
    // Ports
    ControlPort port1 = ControlPort(1, *this);
    ControlPort port2 = ControlPort(2, *this);
    ExpansionPort expansionport = ExpansionPort(*this);
    
    // Bus connecting the VC1541 floppy drives
    IEC iec = IEC(*this);
    
    // Floppy drives
    Drive drive8 = Drive(DRIVE8, *this);
    Drive drive9 = Drive(DRIVE9, *this);
    
    // Datasette
    Datasette datasette = Datasette(*this);
    
    // Mouse
    Mouse mouse = Mouse(*this);
    
    /* Communication channel to the GUI. The GUI registers a listener and a
     * callback function to retrieve messages.
     */
    MessageQueue messageQueue;

    
    //
    // Frame, rasterline, and rasterline cycle information
    //
    
    // The total number of frames drawn since power up
    u64 frame;
    
    /* The currently drawn rasterline. The first rasterline is numbered 0. The
     * number of the last rasterline varies between PAL and NTSC models.
     */
    u16 rasterLine;
    
    /* The currently executed rasterline cycle. The first rasterline cycle is
     * numbered 1. The number of the last cycle varies between PAL and NTSC
     * models.
     */
    u8 rasterCycle;
    
    // Clock frequency
    u32 frequency;
    
    // Duration of a CPU cycle in 1/10 nano seconds
    u64 durationOfOneCycle;
    
    /* The VICII function table. Each entry in this table is a pointer to a
     * VICII method executed in a certain rasterline cycle. vicfunc[0] is a
     * stub. It is never called, because the first cycle is numbered 1.
     */
    void (VICII::*vicfunc[66])(void);
    
    
    //
    // Emulator thread
    //
    
private:
    
    /* Run loop control. This variable is checked at the end of each runloop
     * iteration. Most of the time, the variable is 0 which causes the runloop
     * to repeat. A value greater than 0 means that one or more runloop control
     * flags are set. These flags are flags processed and the loop either
     * repeats or terminates depending on the provided flags.
     */
    u32 runLoopCtrl = 0;
    
    /* Stop request. This variable is used to signal a stop request coming from
     * the GUI. The variable is checked after each frame.
     */
    bool stopFlag = false; 
    
    // The invocation counter for implementing suspend() / resume()
    unsigned suspendCounter = 0;
    
    // The emulator thread
    pthread_t p = NULL;
    
    // Mutex to coordinate the order of execution
    pthread_mutex_t threadLock;
    
    /* Mutex to synchronize the access to all state changing methods such as
     * run(), pause(), etc.
     */
    pthread_mutex_t stateChangeLock;
    
    
    //
    // Emulation speed
    //
    
private:
    
    /* System timer information. Used to match the emulation speed with the
     * speed of a real C64.
     */
    mach_timebase_info_data_t timebase;
    
    /* Wake-up time of the synchronization timer in nanoseconds. This value is
     * recomputed each time the emulator thread is put to sleep.
     */
    u64 nanoTargetTime;
    

    //
    // Operation modes
    //
    
    /* Indicates whether C64 is running in ultimax mode. Ultimax mode can be
     * enabled by external cartridges by pulling game line low and keeping
     * exrom line high. In ultimax mode, most of the C64's RAM and ROM is
     * invisible.
     */
    bool ultimax;
    
    
    //
    // Snapshot storage
    //
    
private:
    
    Snapshot *autoSnapshot = NULL;
    Snapshot *userSnapshot = NULL;
    
    //
    // Initializing
    //
    
public:
    
    C64();
    ~C64();
    
    void prefix() override;

    void reset();

private:
    
    void _reset() override;

    
    //
    // Configuring
    //

public:
    
    // Returns the currently set configuration
    C64Configuration getConfig();
    
    // Gets a single configuration item
    long getConfigItem(ConfigOption option);
    long getConfigItem(DriveID id, ConfigOption option);
    
    // Sets a single configuration item
    bool configure(ConfigOption option, long value);
    bool configure(DriveID id, ConfigOption option, long value);

    // Configures the C64 to match a specific C64 model
    void configure(C64Model model);

    // Returns the C64 model matching the current configuration
    C64Model getModel();
        
    // Updates the VICII function table according to the selected model
    void updateVicFunctionTable();

private:

    bool setConfigItem(ConfigOption option, long value) override;

    
    //
    // Analyzing
    //
    
public:
       
    void inspect();
    void setInspectionTarget(InspectionTarget target);
    void clearInspectionTarget();
    
private:
    
    void _dump() override;

    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        & frequency
        & durationOfOneCycle;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        & frame
        & rasterLine
        & rasterCycle
        & warpMode
        & ultimax;
    }
    
    size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Controlling
    //
    
public:

    void powerOn();
    void powerOff();
    void run();
    void pause();
    
    void setWarp(bool enable);
    bool inWarpMode() { return warpMode; }
    void enableWarpMode() { setWarp(true); }
    void disableWarpMode() { setWarp(false); }

    void enableDebugMode() { setDebug(true); }
    void disableDebugMode() { setDebug(false); }
    bool inDebugMode() { return debugMode; }
    
private:

    void _powerOn() override;
    void _powerOff() override;
    void _run() override;
    void _pause() override;
    void _setWarp(bool enable) override;
    void _setDebug(bool enable) override;


    //
    // Working with the emulator thread
    //

public:
    
    /* Requests the emulator thread to stop and locks the threadLock.
     * The function is called in all state changing methods to obtain ownership
     * of the emulator thread. After returning, the emulator is either powered
     * off (if it was powered off before) or paused (if it was running before).
     */
    void acquireThreadLock();
    
    /* Returns true if a call to powerOn() will be successful.
     * It returns false, e.g., if no Rom is installed.
     */
    bool isReady(ErrorCode *error = NULL);
    
    
    //
    // Accessing the message queue
    //
    
public:
    
    // Registers a listener callback function
    void addListener(const void *sender, void(*func)(const void *, int, long) ) {
        messageQueue.addListener(sender, func);
    }
    
    // Removes a listener callback function
    void removeListener(const void *sender) {
        messageQueue.removeListener(sender);
    }
    
    // Gets a notification message from message queue
    Message getMessage() { return messageQueue.get(); }
    
    // Feeds a notification message into message queue
    void putMessage(MessageType msg, u64 data = 0) { messageQueue.put(msg, data); }
    
    
 
    
    /* The thread enter function. This (private) method is invoked when the
     * emulator thread launches. It has to be declared public to make it
     * accessible by the emulator thread.
     */
    void threadWillStart();
    
    /* The thread exit function. This (private) method is invoked when the
     * emulator thread terminates. It has to be declared public to make it
     * accessible by the emulator thread.
     */
    void threadDidTerminate();
        
    /* The C64 run loop.
     * This function is one of the most prominent ones. It implements the
     * outermost loop of the emulator and therefore the place where emulation
     * starts. If you want to understand how the emulator works, this function
     * should be your starting point.
     */
    void runLoop();
    
    /* Runs or pauses the emulator.
     */
    void stopAndGo();

    /* Executes a single instruction.
     * This function is used for single-stepping through the code inside the
     * debugger. It starts the execution thread and terminates it after the
     * next instruction has been executed.
     */
    void stepInto();
    
    /* Emulates the C64 until the instruction following the current one is
     * reached. This function is used for single-stepping through the code
     * inside the debugger. It sets a soft breakpoint to PC+n where n is the
     * length bytes of the current instruction and starts the emulator thread.
     */
    void stepOver();
    
    /* Emulates the C64 until the end of the current frame. Under certain
     * circumstances the function may terminate earlier, in the middle of a
     * frame. This happens, e.g., if the CPU jams or a breakpoint is reached.
     * It is save to call the function in the middle of a frame. In this case,
     * the C64 is emulated until the curent frame has been completed.
     */
    void executeOneFrame();
    
    /* Emulates the C64 until the end of the current rasterline. This function
     * is called inside executeOneFrame().
     */
    void executeOneLine();
    
    // Executes a single clock cycle
    void executeOneCycle();
    void _executeOneCycle();

    /* Finishes the current instruction. This function is called when the
     * emulator threads terminates in order to reach a clean state. It emulates
     * the CPU until the next fetch cycle is reached.
     */
    void finishInstruction();
    
private:
    
    // Invoked before executing the first cycle of a rasterline
    void beginRasterLine();
    
    // Invoked after executing the last cycle of a rasterline
    void endRasterLine();
    
    // Invoked after executing the last rasterline of a frame
    void endFrame();
    
    
    //
    // Managing the emulator thread
    //
    
public:
    
    /* Requests the emulator to stop at the end of the current frame. This
     * function sets a flag which is evaluated at the end of each frame. It it
     * is set, the run loop is signalled to stop via signalStop().
     */
    void requestStop() { stopFlag = true; }
    
    /* Pauses the emulation thread temporarily. Because the emulator is running
     * in a separate thread, the GUI has to pause the emulator before changing
     * it's internal state. This is done by embedding the code inside a
     * suspend / resume block:
     *
     *           suspend();
     *           do something with the internal state;
     *           resume();
     *
     * It it safe to nest multiple suspend() / resume() blocks.
     */
    void suspend();
    void resume();
    
    /* Sets or clears a run loop control flag. The functions are thread-safe
     * and can be called from inside or outside the emulator thread.
     */
    void setControlFlags(u32 flags);
    void clearControlFlags(u32 flags);
    
    // Convenience wrappers for controlling the run loop
    void signalAutoSnapshot() { setControlFlags(RL_AUTO_SNAPSHOT); }
    void signalUserSnapshot() { setControlFlags(RL_USER_SNAPSHOT); }
    void signalBreakpoint() { setControlFlags(RL_BREAKPOINT_REACHED); }
    void signalWatchpoint() { setControlFlags(RL_WATCHPOINT_REACHED); }
    void signalInspect() { setControlFlags(RL_INSPECT); }
    void signalJammed() { setControlFlags(RL_CPU_JAMMED); }
    void signalStop() { setControlFlags(RL_STOP); }

private:

    /* Restarts the synchronization timer. The function is invoked at launch
     * time to initialize the timer and reinvoked when the synchronization
     * timer gets out of sync.
     */
    void restartTimer();
    
    // Converts kernel time to nanoseconds
    u64 abs_to_nanos(u64 abs) { return abs * timebase.numer / timebase.denom; }
    
    // Converts nanoseconds to kernel time
    u64 nanos_to_abs(u64 nanos) { return nanos * timebase.denom / timebase.numer; }
 
    /* Puts the emulation the thread to sleep. This function is called inside
     * endFrame(). It makes the emulation thread wait until nanoTargetTime has
     * been reached. Before returning, nanoTargetTime is assigned with a new
     * target value.
     */
    void synchronizeTiming();
    
    
    //
    // Handling snapshots
    //
    
public:
    
    /* Requests a snapshot to be taken. Once the snapshot is ready, a message
     * is written into the message queue. The snapshot can then be picked up by
     * calling latestAutoSnapshot() or latestUserSnapshot(), depending on the
     * requested snapshot type.
     */
    void requestAutoSnapshot();
    void requestUserSnapshot();

    // Returns the most recent snapshot or NULL if none was taken
    Snapshot *latestAutoSnapshot();
    Snapshot *latestUserSnapshot();
    
    /* Loads the current state from a snapshot file. This function is not
     * thread-safe and must not be called on a running emulator.
     */
    void loadFromSnapshot(Snapshot *snapshot);
    
    
    //
    // Handling Roms
    //
    
public:
    
    // Computes a Rom checksum
    u32 romCRC32(RomType type);
    u64 romFNV64(RomType type);
     
    // Returns a unique identifier for the installed ROMs
    RomIdentifier romIdentifier(RomType type);
    
    // Returns printable titles for the installed ROMs
    const char *romTitle(RomType type);
    
    // Returns printable sub titles for the installed ROMs
    const char *romSubTitle(u64 fnv);
    const char *romSubTitle(RomType type);
    
    // Returns printable revision strings or hash values for the installed ROMs
    const char *romRevision(RomType type);
    
    // Checks if a certain Rom is present
    bool hasRom(RomType type);
    bool hasMega65Rom(RomType type);

private:
    
    // Returns a revision string if a Mega65 Rom is installed
    char *mega65BasicRev();
    char *mega65KernalRev();

public:
    
    // Installs a Rom
    bool loadRom(RomType type, RomFile *file);
    bool loadRomFromBuffer(RomType type, const u8 *buffer, size_t length);
    bool loadRomFromFile(RomType type, const char *path);
    
    // Erases an installed Rom
    void deleteRom(RomType type);
    
    // Saves a Rom to disk
    bool saveRom(RomType rom, const char *path);
    
    
    //
    // Flashing files
    //
    
    // Flashes a single file into memory
    bool flash(AnyFile *file);
    
    // Flashes a single item of an archive into memory
    bool flash(AnyArchive *file, unsigned item);
    
    
    //
    // Set and query ultimax mode
    //
    
public:
    
    // Returns the ultimax flag
    bool getUltimax() { return ultimax; }
    
    /* Setter for ultimax mode. When the peek / poke lookup table is updated,
     * this function is called if a certain combination is present on the Game
     * and Exrom lines.
     */
    void setUltimax(bool b) { ultimax = b; }
};

#endif
