stm32f4-empty-project
=====================

An empty project with all the boilerplate to start developing with stm32f4.

Change the output file name is app/module.mk

Install the arm-gcc toolchain
---

The toolchain is here:

	https://launchpad.net/gcc-arm-embedded

Find the link for the PPA and add to your system:

	https://launchpad.net/~terry.guo/+archive/gcc-arm-embedded

	sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded
	sudo apt-get update
	sudo apt-get install gcc-arm-none-eabi


Download stlink to interface with the stlink programmer
---

Install stlink

    sudo apt-get install libusb-1.0
    git clone https://github.com/texane/stlink
    cd stlink
    ./autogen.sh 
    ./configure
    make
    sudo mkdir /opt/stlink
    sudo cp st-util st-flash /opt/stlink
    sudo cp 49-stlinkv2.rules /etc/udev/rules.d
    sudo /etc/init.d/udev restart 

Add /opt/stlink to your PATH and test out the st-util utility

    st-util -h

Build the code
---

Run

    make

to build the application code.

Use "make clean" to remove program build files. Use "make realclean" to remove library build files as well.

Load and execute an elf file with gdb
---

While your laptop is connected to the board via the debugger run

    st-util 

Then reset the processor, open a new terminal and run

    arm-none-eabi-gdb app.elf
    target remote localhost:4242 
    load
    continue

Note that optimization settings can cause issues with breakpoints

Flashing a bin file to the program memory
---

While your laptop is connected to the board via the debugger run

    st-flash write app.bin 0x08000000

