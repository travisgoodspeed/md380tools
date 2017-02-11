
# md380-tool / md380-dfu reference

To actively watch incoming calls, printing a call log with name and
callsign:

    md380-tool calllog

To dump the recent dmesg log:

    md380-tool dmesg

To download a raw (headerless) codeplug into the MD380.

    m380-dfu write <filename.img>

To upload a raw codeplug from the MD380.

    md380-dfu read <filename.img>

To dump the bootloader from the MD380.  (Only in radio mode, only on Mac.)

    md380-dfu readboot <filename.bin>

To set time and date to system local time or specified time.

    md380-dfu settime
    md380-dfu settime "mm/dd/yyyy HH:MM:SS" (with quotes)

To set time and date to a specific timezone like UTC, change the environment variable.

    TZ=utc md380-dfu settime

To exit programming mode, returning to radio mode.

    md380-dfu detach

To extract the raw app binary from an ecrypted Tytera firmware image:

    md380-fw --unwrap MD-380-D2.32\(AD\).bin app.bin

To wrap a raw app binary into a flashable Tytera firmware image:

    md380-fw --wrap app-patched.bin MD-380-D2.32-patched.bin

To export all sprites and glyphs from a raw firmware image:

    md380-gfx --dir=imgout --firmware=patched.bin extract

To re-import a single modified PPM sprite (must restore text header
of the originally exported .ppm file; gimp et al. discard it):

    md380-gfx --firmware=patched.bin --gfx=0x80f9ca8-poc.ppm write

To flash the Ham-DMR UserDB to SPI Flash. **Works only on radios
with 16MByte SPI-Flash.**

    generate the upload file

       wc -c < db/users.csv > data ; cat db/users.csv >> data

    program to flash with: (very experimental)

       md380-tool spiflashwrite data 0x100000


    or (all steps included): (very experimental)

       make flashdb

After successfully flashing, the radio will be restarted.

##Flashing on Linux Notes##

To check the type / size of SPI-Flash

    md380-tool spiflashid

