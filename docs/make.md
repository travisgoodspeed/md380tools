
# Make reference

Clean everything

    make distclean

Clean everything except downloads

    make clean

Flash original FW for MD380 with old vocoder.

    make flash_original_D03

Flash original FW for MD380 with new vocoder.

    make flash_original_D13

Flash original oldest FW for MD380.

    make flash_original_D02

Flash experimental FW for MD380 (based on D13)

    make flash

Flash experimental FW for MD380 (based on D02)

    make flash_D02

Flash experimental FW for MD390 (based on S13)

    make flash_S13

Update userdb

    make updatedb

Flash userdb

    make flashdb

Check if all version compile, and processing functions OK (mainly for automatic tests)

    make ci

Create a windows installation package (and distclean to make sure everything is fresh)

    make distclean dist

