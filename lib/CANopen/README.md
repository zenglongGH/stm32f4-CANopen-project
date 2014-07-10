CANopenNode Tweaks
------------------

CANopenNode as used on our Cortex-M4 controllers.

This codebase was obtained from the CANopenNode project at:

http://sourceforge.net/projects/canopennode/

All that exists here are minor tweaks for our specific projects.

Notes on Usage
--------------

This repo is for use as a git submodule, and assumes object dictionary
configuration (CO_OD.h, etc.) lives in a parent directory structured as follows.

project lib root
  ./stm32f4-canopennode
    ./src
      ./CO_OD.c -> ../../od_config/src/CO_OD.c
      ./_project.xml -> ../../od_config/src/_project.xml
    ./inc
      ./CO_OD.h -> ../../od_config/inc/CO_OD.h
    ./doc
    ./Object_Dictionary_Editor
  ./od_config
    ./src
      ./CO_OD.c
      ./_project.xml
    ./inc
      ./CO_OD.h
