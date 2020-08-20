// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController: NSMenuItemValidation {
    
    open func validateMenuItem(_ item: NSMenuItem) -> Bool {

        let powered = c64.isPoweredOn
        let running = c64.isRunning
 
        var drive: DriveProxy { return c64.drive(DriveID(item.tag))! }
        
        func validateURLlist(_ list: [URL], image: NSImage) -> Bool {
            
            let slot = item.tag % 10
            
            if let url = myAppDelegate.getRecentlyUsedURL(slot, from: list) {
                item.title = url.lastPathComponent
                item.isHidden = false
                item.image = image
            } else {
                item.title = ""
                item.isHidden = true
                item.image = nil
            }
            
            return true
        }
        
        switch item.action {
            
        // File menu
        case #selector(MyController.importConfigAction(_:)),
             #selector(MyController.exportConfigAction(_:)),
             #selector(MyController.resetConfigAction(_:)):
            return !powered
            
        // Edit menu
        case #selector(MyController.stopAndGoAction(_:)):
            item.title = running ? "Pause" : "Continue"
            return true
            
        case #selector(MyController.powerAction(_:)):
            item.title = powered ? "Power Off" : "Power On"
            return true
            
        // View menu
        case #selector(MyController.toggleStatusBarAction(_:)):
            item.title = statusBar ? "Hide Status Bar" : "Show Status Bar"
            return true
            
        case #selector(MyController.hideMouseAction(_:)):
            item.title = hideMouse ? "Show Mouse Cursor" : "Hide Mouse Cursor"
            return true
            
        // Keyboard menu
        case #selector(MyController.shiftLockAction(_:)):
            item.state = c64.keyboard.shiftLockIsHoldDown() ? .on : .off
            return true
            
        // Drive menu
        case #selector(MyController.insertRecentDiskAction(_:)):
            return validateURLlist(myAppDelegate.recentlyInsertedDiskURLs, image: smallDisk)
            
        case  #selector(MyController.ejectDiskAction(_:)),
              #selector(MyController.exportDiskAction(_:)):
            return drive.hasDisk()
            
        case #selector(MyController.exportRecentDiskDummyAction8(_:)):
            return c64.drive8.hasDisk()
            
        case #selector(MyController.exportRecentDiskDummyAction9(_:)):
            return c64.drive9.hasDisk()
                        
        case #selector(MyController.exportRecentDiskAction(_:)):
            switch item.tag {
            case 8: return validateURLlist(myAppDelegate.recentlyExportedDisk8URLs, image: smallDisk)
            case 9: return validateURLlist(myAppDelegate.recentlyExportedDisk9URLs, image: smallDisk)
            default: fatalError()
            }
            
        case #selector(MyController.writeProtectAction(_:)):
            item.state = drive.hasWriteProtectedDisk() ? .on : .off
            return drive.hasDisk()
            
        // Tape menu
        case #selector(MyController.insertRecentTapeAction(_:)):
            return validateURLlist(myAppDelegate.recentlyInsertedTapeURLs, image: smallTape)
            
        case #selector(MyController.ejectTapeAction(_:)):
            return c64.datasette.hasTape()
            
        case #selector(MyController.playOrStopAction(_:)):
            item.title = c64.datasette.playKey() ? "Press Stop Key" : "Press Play On Tape"
            return c64.datasette.hasTape()
            
        case #selector(MyController.rewindAction(_:)):
            return c64.datasette.hasTape()
            
        // Cartridge menu
        case #selector(MyController.attachRecentCartridgeAction(_:)):
            return validateURLlist(myAppDelegate.recentlyAttachedCartridgeURLs, image: smallCart)
            
        case #selector(MyController.attachGeoRamDummyAction(_:)):
            item.state = (c64.expansionport.cartridgeType() == CRT_GEO_RAM) ? .on : .off
            
        case #selector(MyController.attachIsepicAction(_:)):
            item.state = (c64.expansionport.cartridgeType() == CRT_ISEPIC) ? .on : .off
            
        case #selector(MyController.detachCartridgeAction(_:)):
            return c64.expansionport.cartridgeAttached()
            
        case #selector(MyController.pressButtonDummyAction(_:)):
            return c64.expansionport.numButtons() > 0
            
        case #selector(MyController.pressCartridgeButton1Action(_:)):
            let title = c64.expansionport.getButtonTitle(1)
            item.title = title ?? ""
            item.isHidden = title == nil
            return title != nil
            
        case #selector(MyController.pressCartridgeButton2Action(_:)):
            let title = c64.expansionport.getButtonTitle(2)
            item.title = title ?? ""
            item.isHidden = title == nil
            return title != nil
            
        case #selector(MyController.setSwitchDummyAction(_:)):
            return c64.expansionport.hasSwitch()
            
        case #selector(MyController.setSwitchNeutralAction(_:)):
            let title = c64.expansionport.switchDescription(0)
            item.title = title ?? ""
            item.isHidden = title == nil
            item.state = c64.expansionport.switchIsNeutral() ? .on : .off
            return title != nil
            
        case #selector(MyController.setSwitchLeftAction(_:)):
            let title = c64.expansionport.switchDescription(-1)
            item.title = title ?? ""
            item.isHidden = title == nil
            item.state = c64.expansionport.switchIsLeft() ? .on : .off
            return title != nil
            
        case #selector(MyController.setSwitchRightAction(_:)):
            let title = c64.expansionport.switchDescription(1)
            item.title = title ?? ""
            item.isHidden = title == nil
            item.state = c64.expansionport.switchIsRight() ? .on : .off
            return title != nil
            
        case #selector(MyController.geoRamBatteryAction(_:)):
            item.state = c64.expansionport.hasBattery() ? .on : .off
            return c64.expansionport.cartridgeType() == CRT_GEO_RAM
            
        // Debug menu
        case #selector(MyController.markIRQLinesAction(_:)):
            item.state = c64.vic.showIrqLines() ? .on : .off
            
        case #selector(MyController.markDMALinesAction(_:)):
            item.state = c64.vic.showDmaLines() ? .on : .off
            
        case #selector(MyController.hideSpritesAction(_:)):
            item.state = c64.vic.hideSprites() ? .on : .off
            
        case #selector(MyController.traceAction(_:)):
            return !c64.isReleaseBuild
            
        case #selector(MyController.traceIecAction(_:)):
            item.state = c64.iec.tracing() ? .on : .off

        /*
        case #selector(MyController.traceVC1541CpuAction(_:)):
            item.state = c64.drive8.cpu.tracing() ? .on : .off
        */
            
        case #selector(MyController.traceViaAction(_:)):
            item.state = c64.drive8.via1.tracing() ? .on : .off
            
        case #selector(MyController.dumpStateAction(_:)):
            return !c64.isReleaseBuild
            
        default:
            return true
        }
        
        return true
    }

    //
    // Action methods (App menu)
    //
    
    @IBAction func preferencesAction(_ sender: Any!) {
        
        if myAppDelegate.prefController == nil {
            myAppDelegate.prefController =
                PreferencesController.make(parent: self,
                                           nibName: NSNib.Name("Preferences"))
        }
        myAppDelegate.prefController?.showWindow(self)
    }
    
    func importPrefs(_ prefixes: [String]) {
        
        track("Importing user defaults with prefixes \(prefixes)")
        
        let panel = NSOpenPanel()
        panel.prompt = "Import"
        panel.allowedFileTypes = ["vc64conf"]
        
        panel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = panel.url {
                    self.loadUserDefaults(url: url, prefixes: prefixes)
                }
            }
        })
    }
    
    func exportPrefs(_ prefixes: [String]) {
        
        track("Exporting user defaults with prefixes \(prefixes)")
        
        let panel = NSSavePanel()
        panel.prompt = "Export"
        panel.allowedFileTypes = ["vc64conf"]
        
        panel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = panel.url {
                    track()
                    self.saveUserDefaults(url: url, prefixes: prefixes)
                }
            }
        })
    }
    
    @IBAction func importConfigAction(_ sender: Any!) {
        
        importPrefs(["VC64_ROM", "VC64_HW", "VC64_VID"])
    }
    
    @IBAction func exportConfigAction(_ sender: Any!) {
        
        exportPrefs(["VC64_ROM", "VC64_HW", "VC64_VID"])
    }
    
    @IBAction func resetConfigAction(_ sender: Any!) {
        
        track()
        
        UserDefaults.resetRomUserDefaults()
        UserDefaults.resetHardwareUserDefaults()
        UserDefaults.resetVideoUserDefaults()
        
        c64.suspend()
        config.loadRomUserDefaults()
        config.loadHardwareUserDefaults()
        config.loadVideoUserDefaults()
        c64.resume()
    }
    
    //
    // Action methods (File menu)
    //
    
    func openConfigurator(tab: String = "") {
        
        if configurator == nil {
            let name = NSNib.Name("Configuration")
            configurator = ConfigurationController.make(parent: self, nibName: name)
        }
        configurator?.showSheet(tab: tab)
    }
    
    @IBAction func configureAction(_ sender: Any!) {
        
        openConfigurator()
    }

    @IBAction func inspectorAction(_ sender: Any!) {
        
        if inspector == nil {
            inspector = Inspector.make(parent: self, nibName: "Inspector")
        }
        inspector?.showWindow(self)
    }
    
    @IBAction func monitorAction(_ sender: Any!) {
        
        if monitor == nil {
            monitor = Monitor.make(parent: self, nibName: "Monitor")
        }
        monitor?.showWindow(self)
    }
    
    @IBAction func saveScreenshotDialog(_ sender: Any!) {
                
        // Create save panel
        let savePanel = NSSavePanel()
        savePanel.prompt = "Export"
        
        // Set allowed file types
        switch pref.screenshotTarget {
        case .tiff: savePanel.allowedFileTypes = ["jpg"]
        case .bmp: savePanel.allowedFileTypes = ["bmp"]
        case .gif: savePanel.allowedFileTypes = ["gif"]
        case .jpeg: savePanel.allowedFileTypes = ["jpg"]
        case .png: savePanel.allowedFileTypes = ["png"]
        default:
            track("Unsupported image format: \(pref.screenshotTarget)")
            return
        }
        
        // Run panel as sheet
        savePanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = savePanel.url {
                    do {
                        try self.saveScreenshot(url: url)
                    } catch {
                        NSApp.presentError(error)
                    }
                }
            }
        })
    }
    
    @IBAction func quicksaveScreenshot(_ sender: Any!) {
        
        // Determine file suffix
        var suffix: String
        switch pref.screenshotTarget {
        case .tiff: suffix = "tiff"
        case .bmp: suffix = "bmp"
        case .gif: suffix = "gif"
        case .jpeg: suffix = "jpg"
        case .png: suffix = "png"
        default:
            track("Unsupported image format: \(pref.screenshotTarget)")
            return
        }
        
        // Assemble URL and save
        let paths = NSSearchPathForDirectoriesInDomains(.desktopDirectory, .userDomainMask, true)
        let desktopUrl = NSURL.init(fileURLWithPath: paths[0])
        if let url = desktopUrl.appendingPathComponent("Screenshot." + suffix) {
            do {
                try saveScreenshot(url: url.addTimeStamp().makeUnique())
            } catch {
                track("Cannot quicksave screenshot")
            }
        }
    }
    
    func saveScreenshot(url: URL) throws {
        
        // Take screenshot
        let image = renderer.screenshot(afterUpscaling: pref.screenshotSource > 0)
        
        // Convert to target format
        let data = image?.representation(using: pref.screenshotTarget)
        
        // Save to file
        try data?.write(to: url, options: .atomic)
    }
    
    @IBAction func takeSnapshot(_ sender: Any!) {
        
        c64.takeUserSnapshot()
    }
    
    //
    // Action methods (Edit menu)
    //
    
    @IBAction func paste(_ sender: Any!) {
        
        track()
        
        let pasteBoard = NSPasteboard.general
        guard let text = pasteBoard.string(forType: .string) else {
            track("Cannot paste. No text in pasteboard")
            return
        }
        
        keyboard.type(string: text, completion: nil)
    }
    
    @IBAction func stopAndGoAction(_ sender: Any!) {
        
        track("MyControllerMenu")
        c64.stopAndGo()
    }
    
    @IBAction func stepIntoAction(_ sender: Any!) {

        track("MyControllerMenu")
        c64.stepInto()
        // inspector.refresh()
    }
    
    @IBAction func stepOverAction(_ sender: Any!) {
        
        track("MyControllerMenu")
        c64.stepOver()
        // inspector.refresh()
    }
    
    @IBAction func resetAction(_ sender: Any!) {

        track()

        renderer.rotateLeft()
        c64.reset()
        c64.run()
    }

    @IBAction func powerAction(_ sender: Any!) {
        
        var error: ErrorCode = ERR_OK

        if c64.isPoweredOn {
            c64.powerOff()
            return
        }
        
        if c64.isReady(&error) {
            c64.run()
        } else {
            mydocument.showConfigurationAltert(error)
        }
    }
     
    //
    // Action methods (View menu)
    //

    @IBAction func toggleStatusBarAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.toggleStatusBarAction(sender)
        }
        
        showStatusBar(!statusBar)
    }
    
    /*
    public func showStatusBar(_ value: Bool) {
        
        let items: [NSView: Bool] = [
            greenLED1: false,
            redLED1: false,
            progress1: false,
            diskIcon1: !c64.drive8.hasDisk(),
            greenLED2: false,
            redLED2: false,
            progress2: false,
            diskIcon2: !c64.drive9.hasDisk(),
            crtIcon: !c64.expansionport.cartridgeAttached(),
            crtSwitch: !c64.expansionport.hasSwitch(),
            crtButton1: c64.expansionport.numButtons() < 1,
            crtButton2: c64.expansionport.numButtons() < 2,
            tapeIcon: !c64.datasette.hasTape(),
            tapeProgress: false,
            clockSpeed: false,
            clockSpeedBar: false,
            warpIcon: false
        ]
        
        if !statusBar && value {
        
            for (item, hide) in items {
                item.isHidden = hide
            }
            metalScreen.shrink()
            window?.setContentBorderThickness(24, for: .minY)
            adjustWindowSize()
            statusBar = true
        }
 
        if statusBar && !value {
            
            for (item, _) in items {
                item.isHidden = true
            }
            metalScreen.expand()
            window?.setContentBorderThickness(0, for: .minY)
            adjustWindowSize()
            statusBar = false
        }
    }
    */
    
    @IBAction func hideMouseAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.hideMouseAction(sender)
        }
        
        if hideMouse {
            NSCursor.unhide()
            CGAssociateMouseAndMouseCursorPosition(boolean_t(truncating: true))
        } else {
            NSCursor.hide()
            CGAssociateMouseAndMouseCursorPosition(boolean_t(truncating: false))
        }
        
        hideMouse = !hideMouse
    }
    
    //
    // Action methods (Keyboard menu)
    //

    /*
    @IBAction func stickyKeyboardAction(_ sender: Any!) {
        
        // Open the virtual keyboard as a window
        let nibName = NSNib.Name("VirtualKeyboard")
        // virtualKeyboard = VirtualKeyboardController.init(windowNibName: nibName)
        // virtualKeyboard?.showWindow(withParent: self)
        myAppDelegate.virtualKeyboard = VirtualKeyboardController.init(windowNibName: nibName)
        myAppDelegate.virtualKeyboard?.showWindow(withParent: self)
    }
    */
    
    @IBAction func clearKeyboardMatrixAction(_ sender: Any!) {
        
        track()
        c64.keyboard.releaseAll()
    }

    // -----------------------------------------------------------------
    @IBAction func runstopAction(_ sender: Any!) {
        keyboard.type(key: C64Key.runStop)
    }
    @IBAction func restoreAction(_ sender: Any!) {
        keyboard.type(key: C64Key.restore)
    }
    @IBAction func runstopRestoreAction(_ sender: Any!) {
        keyboard.type(keyList: [C64Key.runStop, C64Key.restore])
    }
    @IBAction func commodoreKeyAction(_ sender: Any!) {
        keyboard.type(key: C64Key.commodore)
    }
    @IBAction func clearKeyAction(_ sender: Any!) {
        keyboard.type(keyList: [C64Key.home, C64Key.shift])
    }
    @IBAction func homeKeyAction(_ sender: Any!) {
        keyboard.type(key: C64Key.home)
    }
    @IBAction func insertKeyAction(_ sender: Any!) {
        keyboard.type(keyList: [C64Key.delete, C64Key.shift])
    }
    @IBAction func deleteKeyAction(_ sender: Any!) {
        keyboard.type(key: C64Key.delete)
    }
    @IBAction func leftarrowKeyAction(_ sender: Any!) {
        keyboard.type(key: C64Key.leftArrow)
    }
    @IBAction func shiftLockAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.shiftLockAction(sender)
        }
        if c64.keyboard.shiftLockIsHoldDown() {
            c64.keyboard.unlockShift()
        } else {
            c64.keyboard.lockShift()
        }
    }

    // -----------------------------------------------------------------
    @IBAction func loadDirectoryAction(_ sender: Any!) {
        keyboard.type(string: "LOAD \"$\",8", completion: nil)
    }
    @IBAction func listAction(_ sender: Any!) {
        keyboard.type(string: "LIST", completion: nil)
    }
    @IBAction func loadFirstFileAction(_ sender: Any!) {
        keyboard.type(string: "LOAD \"*\",8,1", completion: nil)
    }
    @IBAction func runProgramAction(_ sender: Any!) {
        keyboard.type(string: "RUN", completion: nil)
    }
    @IBAction func formatDiskAction(_ sender: Any!) {
        keyboard.type(string: "OPEN 1,8,15,\"N:TEST, ID\": CLOSE 1", completion: nil)
    }

    //
    // Action methods (Disk menu)
    //

    @IBAction func newDiskAction(_ sender: NSMenuItem!) {
        
        let drive = DriveID(sender.tag)
        let emptyArchive = AnyArchiveProxy.make()
        
        mydocument?.attachment = D64FileProxy.make(withAnyArchive: emptyArchive)
        mydocument?.mountAttachmentAsDisk(drive: drive)
        myAppDelegate.clearRecentlyExportedDiskURLs(drive: drive)
    }
    
    @IBAction func insertDiskAction(_ sender: NSMenuItem!) {
        
        let drive = DriveID(sender.tag)
        
        // Ask user to continue if the current disk contains modified data
        if !proceedWithUnexportedDisk(drive: drive) {
            return
        }
        
        // Show the OpenPanel
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Insert"
        openPanel.allowedFileTypes = ["t64", "prg", "p00", "d64", "g64", "nib"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = openPanel.url {
                    do {
                        try self.mydocument?.createAttachment(from: url)
                        self.mydocument?.mountAttachmentAsDisk(drive: drive)
                    } catch {
                        NSApp.presentError(error)
                    }
                }
            }
        })
    }
    
    @IBAction func insertRecentDiskAction(_ sender: NSMenuItem!) {

        // Extrace drive number and slot from tag
        let drive = sender.tag < 10 ? DRIVE8 : DRIVE9
        let item = sender.tag < 10 ? sender.tag : sender.tag - 10
        
        // Get URL and insert
        if let url = myAppDelegate.getRecentlyInsertedDiskURL(item) {
            do {
                try mydocument!.createAttachment(from: url)
                if mydocument!.proceedWithUnexportedDisk(drive: drive) {
                    mydocument!.mountAttachmentAsDisk(drive: drive)
                }
            } catch {
                NSApp.presentError(error)
            }
        }
    }
    
    @IBAction func exportRecentDiskDummyAction8(_ sender: NSMenuItem!) {}
    @IBAction func exportRecentDiskDummyAction9(_ sender: NSMenuItem!) {}

    @IBAction func exportRecentDiskAction(_ sender: NSMenuItem!) {
                
        // Extrace drive number and slot from tag
        let drive = sender.tag < 10 ? DRIVE8 : DRIVE9
        let item = sender.tag < 10 ? sender.tag : sender.tag - 10
        
        // Get URL and export
        if let url = myAppDelegate.getRecentlyExportedDiskURL(item, drive: drive) {
            mydocument!.export(drive: drive, to: url)
        }
    }
    
    @IBAction func clearRecentlyInsertedDisksAction(_ sender: Any!) {
        myAppDelegate.recentlyInsertedDiskURLs = []
    }

    @IBAction func clearRecentlyExportedDisksAction(_ sender: NSMenuItem!) {

        let drive = DriveID(sender.tag)
        myAppDelegate.clearRecentlyExportedDiskURLs(drive: drive)
    }

    @IBAction func clearRecentlyInsertedTapesAction(_ sender: Any!) {
        myAppDelegate.recentlyInsertedTapeURLs = []
    }
    
    @IBAction func clearRecentlyAttachedCartridgesAction(_ sender: Any!) {
        myAppDelegate.recentlyAttachedCartridgeURLs = []
    }
    
    @IBAction func ejectDiskAction(_ sender: NSMenuItem!) {
        
        let drive = DriveID(sender.tag)
        
        if proceedWithUnexportedDisk(drive: drive) {
            changeDisk(nil, drive: drive)
            myAppDelegate.clearRecentlyExportedDiskURLs(drive: drive)
        }
    }
    
    @IBAction func exportDiskAction(_ sender: NSMenuItem!) {

        let drive = DriveID(sender.tag)
        
        let nibName = NSNib.Name("ExportDiskDialog")
        let exportPanel = ExportDiskController.init(windowNibName: nibName)
        exportPanel.showSheet(forDrive: drive)
    }
     
    @IBAction func writeProtectAction(_ sender: NSMenuItem!) {
        
        let drive = DriveID(sender.tag)

        if drive == DRIVE8 {
            c64.drive8.disk.toggleWriteProtection()
        } else {
            c64.drive9.disk.toggleWriteProtection()
        }
    }
    
    /*
    @IBAction func drivePowerAction(_ sender: NSMenuItem!) {
        
        let drive = DriveID(sender.tag)
        drivePowerAction(drive: drive)
    }
    
    func drivePowerAction(drive: DriveID) {

        if drive == DRIVE8 {
            c64.drive8.toggleConnection()
        } else {
            c64.drive9.toggleConnection()
        }
    }
    */
    
    //
    // Action methods (Datasette menu)
    //
    
    @IBAction func insertTapeAction(_ sender: Any!) {
        
        // Show the OpenPanel
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Insert"
        openPanel.allowedFileTypes = ["tap"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = openPanel.url {
                    do {
                        try self.mydocument?.createAttachment(from: url)
                        self.mydocument?.mountAttachmentAsTape()
                    } catch {
                        NSApp.presentError(error)
                    }
                }
            }
        })
    }
    
    @IBAction func insertRecentTapeAction(_ sender: NSMenuItem!) {
        
        let tag = sender.tag
        
        if let url = myAppDelegate.getRecentlyInsertedTapeURL(tag) {
            do {
                try mydocument!.createAttachment(from: url)
                mydocument!.mountAttachmentAsTape()
            } catch {
                NSApp.presentError(error)
            }
        }
    }
    
    @IBAction func ejectTapeAction(_ sender: Any!) {
        track()
        c64.datasette.ejectTape()
    }
    
    @IBAction func playOrStopAction(_ sender: Any!) {
        track()
        if c64.datasette.playKey() {
            c64.datasette.pressStop()
        } else {
            c64.datasette.pressPlay()
        }
    }
    
    @IBAction func rewindAction(_ sender: Any!) {
        track()
        c64.datasette.rewind()
    }

    //
    // Action methods (Cartridge menu)
    //

    @IBAction func attachCartridgeAction(_ sender: Any!) {
        
        // Show the OpenPanel
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Attach"
        openPanel.allowedFileTypes = ["crt"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = openPanel.url {
                    do {
                        try self.mydocument?.createAttachment(from: url)
                        self.mydocument?.mountAttachmentAsCartridge()
                    } catch {
                        NSApp.presentError(error)
                    }
                }
            }
        })
    }
    
    @IBAction func attachRecentCartridgeAction(_ sender: NSMenuItem!) {
        
        track()
        let tag = sender.tag
        
        if let url = myAppDelegate.getRecentlyAtachedCartridgeURL(tag) {
            do {
                try mydocument!.createAttachment(from: url)
                mydocument!.mountAttachmentAsCartridge()
            } catch {
                NSApp.presentError(error)
            }
        }
    }
    
    @IBAction func detachCartridgeAction(_ sender: Any!) {
        track()
        c64.expansionport.detachCartridgeAndReset()
    }

    @IBAction func attachGeoRamDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction func attachGeoRamAction(_ sender: NSMenuItem!) {

        let capacity = sender.tag
        track("RAM capacity = \(capacity)")
        c64.expansionport.attachGeoRamCartridge(capacity)
    }
    
    @IBAction func attachIsepicAction(_ sender: Any!) {
        track("")
        c64.expansionport.attachIsepicCartridge()
    }
    
    @IBAction func geoRamBatteryAction(_ sender: Any!) {
        c64.expansionport.setBattery(!c64.expansionport.hasBattery())
    }
    
    @IBAction func pressCartridgeButton1Action(_ sender: NSButton!) {
        
        c64.expansionport.pressButton(1)
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            self.c64.expansionport.releaseButton(1)
        }
    }

    @IBAction func pressCartridgeButton2Action(_ sender: NSButton!) {
        
        c64.expansionport.pressButton(2)
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            self.c64.expansionport.releaseButton(2)
        }
    }
    
    @IBAction func pressButtonDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction func setSwitchNeutralAction(_ sender: Any!) {
        
        c64.expansionport.setSwitchPosition(0)
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            // TODO: Delete or call a method here if it is really needed.
        }
    }

    @IBAction func setSwitchLeftAction(_ sender: Any!) {
        
        c64.expansionport.setSwitchPosition(-1)
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            // TODO: Delete or call a method here if it is really needed.
        }
    }

    @IBAction func setSwitchRightAction(_ sender: Any!) {
        
        c64.expansionport.setSwitchPosition(1)
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            // TODO: Delete or call a method here if it is really needed.
        }
    }

    @IBAction func setSwitchDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }
    
    @IBAction func toggleSwitchAction(_ sender: NSButton!) {
        
        // tag remembers if we previously pulled left or right
        let dir = sender.tag
        let pos = c64.expansionport.switchPosition()
        
        // Move to the next valid switch position
        for newPos in [pos + dir, pos + 2 * dir, pos - dir, pos - 2 * dir] {
            
            if c64.expansionport.validSwitchPosition(newPos) {
                track("old pos = \(pos) new pos = \(newPos)")
                
                c64.expansionport.setSwitchPosition(newPos)
                sender.tag = (newPos > pos) ? 1 : -1
                break
            }
        }
    }
        
    //
    // Action methods (Debug menu)
    //

    @IBAction func hideSpritesAction(_ sender: Any!) {

        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.hideSpritesAction(sender)
        }
        
        c64.vic.setHideSprites(!c64.vic.hideSprites())
    }
  
    @IBAction func markIRQLinesAction(_ sender: Any!) {
    
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.markIRQLinesAction(sender)
        }
        
        c64.vic.setShowIrqLines(!c64.vic.showIrqLines())
    }
    
    @IBAction func markDMALinesAction(_ sender: Any!) {
    
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.markDMALinesAction(sender)
        }
        
        c64.vic.setShowDmaLines(!c64.vic.showDmaLines())
    }
    
    @IBAction func traceAction(_ sender: Any!) {
        // Dummy target to make menu item validatable
    }
    
    @IBAction func dumpStateAction(_ sender: Any!) {
        // Dummy target to make menu item validatable
    }

    @IBAction func zoomTextureInAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.zoomTextureOutAction(sender)
        }
        
        renderer.zoomTextureIn()
    }
    
    @IBAction func zoomTextureOutAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.zoomTextureInAction(sender)
        }
        
        renderer.zoomTextureOut()
    }
    
    @IBAction func traceIecAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.traceIecAction(sender)
        }
        
        c64.iec.setTracing(!c64.iec.tracing())
    }
 
    @IBAction func traceVC1541CpuAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.traceVC1541CpuAction(sender)
        }
        
        // c64.drive8.cpu.setTracing(!c64.drive8.cpu.tracing())
    }
  
    @IBAction func traceViaAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.traceViaAction(sender)
        }
        
        c64.drive8.via1.setTracing(!c64.drive8.via1.tracing())
        c64.drive8.via2.setTracing(!c64.drive8.via2.tracing())
    }
    
    @IBAction func dumpC64(_ sender: Any!) { c64.dump() }
    @IBAction func dumpC64CPU(_ sender: Any!) { c64.cpu.dump() }
    @IBAction func dumpC64CIA1(_ sender: Any!) {c64.cia1.dump() }
    @IBAction func dumpC64CIA2(_ sender: Any!) { c64.cia2.dump() }
    @IBAction func dumpC64VIC(_ sender: Any!) { c64.vic.dump() }
    @IBAction func dumpC64SID(_ sender: Any!) { c64.sid.dump() }
    @IBAction func dumpC64Memory(_ sender: Any!) { c64.mem.dump() }
    @IBAction func dumpVC1541(_ sender: Any!) { c64.drive8.dump() }
    @IBAction func dumpVC1541CPU(_ sender: Any!) { c64.drive8.dump() }
    @IBAction func dumpVC1541VIA1(_ sender: Any!) { c64.drive8.via1.dump() }
    @IBAction func dumpVC1541VIA2(_ sender: Any!) { c64.drive8.via2.dump() }
    @IBAction func dumpDisk(_ sender: Any!) { c64.drive8.disk.dump() }
    @IBAction func dumpKeyboard(_ sender: Any!) { c64.keyboard.dump() }
    @IBAction func dumpC64JoystickA(_ sender: Any!) { c64.port1.dump() }
    @IBAction func dumpC64JoystickB(_ sender: Any!) { c64.port2.dump(); gamePadManager.listDevices()}
    @IBAction func dumpIEC(_ sender: Any!) { c64.iec.dump() }
    @IBAction func dumpC64ExpansionPort(_ sender: Any!) { c64.expansionport.dump() }
}