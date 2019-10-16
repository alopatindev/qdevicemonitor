Troubleshooting
---------------

### iOS devices are not recognized?
Make sure that you've confirmed (from the device) that you trust this computer

#### on GNU/Linux
If you've just installed **usbmuxd** (as dependence) it probably didn't run
automatically as you plugged in the device.
If so—try `udevadm control --reload` or run usbmuxd manually

#### on Windows
Make sure you've installed [iTunes](https://www.apple.com/itunes/download/)
and checked that it runs and detects your device

### Android devices are not recognized?
1. Make sure you've enabled Developer mode on your device (basically with Settings — Developer Options — USB Debugging)
2. Make sure you've confirmed (from the device) you trust your computer
    1. If confirmation dialog doesn't appear—try to downgrade Android SDK Tools (for instance downgrading `dev-util/android-tools` from `9.0.0_p3` to `6.0.1_p79` helped me with Xiaomi Mi 8) and then run `adb kill-server` and `adb devices`
3. Make sure your device is detectable with ddms/monitor
    1. On some devices debugging works only if you select the particular "Connect as" option (Built-in CD-ROM, Camera, MTP, etc.)
    2. Try `adb kill-server` and `adb devices` terminal commands

#### on GNU/Linux
1. Check the **Vendor ID** of your device with `lsusb`
2. Create or modify the file `~/.android/adb_usb.ini`. Add a line (with the content `0x1234`, where `1234` should be replaced with your **Vendor ID**) to the beginning or end of the file
3. Create or modify the file `/etc/udev/rules.d/51-android.rules`
4. Modify the file permission: `sudo chmod a+rx /etc/udev/rules.d/51-android.rules`
5. Add a line (with the following content) to the file: `SUBSYSTEM=="usb", ATTR{idVendor}=="1234", MODE="0666", GROUP="plugdev"` where `1234` should be replaced with your **Vendor ID**
6. Ensure that you are in the user group **plugdev** or else add yourself into the group: `sudo gpasswd -a ${USER} plugdev` (this change will work after next login)
7. Restart the udev service: `sudo service udev restart`
8. `adb kill-server ; adb devices`

#### on Windows
1. If your device is not detectable with ddms/monitor
    1. Check Control Panel — System — Hardware — **Device Manager**: is your device detected correctly? If it's not—try reinstalling the driver specifying the path to `<Android SDK>\extras\google\usb-driver`
    2. If it didn't work—try reinstalling the driver by choosing `Don't search. I will choose the driver to install` and select `Android Composite ADB Interface`
2. Make sure you've added a path to `platform-tools` directory (it's located in Android SDK) to **Path** environment variable
3. Try to **restart** the OS

If you're still experiencing issues—try all troubleshooting steps from the beginning (probably you've configured everything right and reapplying one of the previous steps will work)

If it didn't help—try to Google it, specifying the device you use. I'm pretty sure you're running into some known issue ;)

### Mac OS X: App can’t be opened because it is from an unidentified developer
Try [this guide](http://osxdaily.com/2012/07/27/app-cant-be-opened-because-it-is-from-an-unidentified-developer/)

### Windows protected your PC
"Windows SmartScreen prevented an unrecognized app from starting. Running this app might put your PC at risk"

Press More info — Run anyway

### Android Emulator was not detected
Automatic detection of Android Emulator is not supported. Please use File — **Detect Devices** after Emulator finishes booting
