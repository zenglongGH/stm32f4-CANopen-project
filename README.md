stm32f4-CANopen-project
=====================

An empty project with all the boilerplate to start developing with stm32f4 and canopen-node

Change the output file name in app/module.mk

Grab the project with:
	git clone file:///path/to/repo/

Cloning will bring over the remotes specified in that directory. So you'll need to remove the remotes you don't want.

	git remote rm <remote>

And add the ones you do, after you have created your remote repository.

	git remote add origin <url>

You will also want to --set-upstream-to, or -u to tell git this is the remote repository this branch will update to, presuming you are on the master branch.

	git push -u origin master

Then you'll need to decide which branches to keep and add to the remote. If you want to push all of them, just do git push --mirror. This will push all your tags and your remotes. But since we edited your remotes in an earlier step, it shouldn't be a problem.

If you only want to keep a few, you can git push -u origin <branch> each one you want.

Install a few packages to make everything work
---

    sudo apt-get install flex bison libgmp3-dev libmpfr-dev libncurses5-dev libmpc-dev autoconf texinfo build-essential libftdi-dev libexpat1 libexpat1-dev


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

