# MacOS specifics #

## Preparing build environment ##

#### With Homebrew: ####

    brew update
    brew upgrade
    brew tap armmbed/formulae  # This is where you get the ARM cross-compiling stuff
    brew install binutils libusb armmbed/formulae/arm-none-eabi-gcc
    sudo easy_install pip  # See below for methods that don't require sudo access
    sudo pip2 install pyusb

#### Fix the environment statements: ####

The scripts look for python2, but you likely just have that installed as (system) python. Hence:

    sudo ln -s /usr/bin/python /usr/bin/python2

#### Alternative no-`sudo` method - install Python 2 from Homebrew: ####

    brew install python2
    pip2 install pyusb

#### Alternative no-`sudo` method - use `virtualenv`: ####

    python2 -m virtualenv path/to/md380-virtualenv  # Modify to your environment
    . path/to/md380-virtualenv/bin/activate
    which python2 || ln -s python path/to/md380-virtualenv/bin/python2  # Most likely unnecessary
    which pip2 || ln -s pip path/to/md380-virtualenv/bin/pip2  # Most likely unnecessary
    pip2 install pyusb

After this, `make flash` as expected.
