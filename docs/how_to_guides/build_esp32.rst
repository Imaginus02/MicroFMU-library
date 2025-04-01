.. _build_esp32:

Building and Deploying MicroFMU on ESP32
===========================================

Overview
--------
This guide explains how to compile MicroPython with MicroFMU for ESP32 and deploy it to the microcontroller.

Prerequisites
-------------
- ESP32 board
- Installed ESP-IDF toolchain
- MicroPython source code (``micropython_dir`` set correctly)
- USB serial drivers for ESP32

Steps
-----

1. **Navigate to the ESP32 port directory**

   .. code-block:: bash

      cd "$micropython_dir/ports/esp32"

2. **Initialize ESP-IDF and set environment variables**

   .. code-block:: bash

      . $HOME/esp/esp-idf/export.sh

3. **Compile MicroPython with MicroFMU**

   .. code-block:: bash

      make submodules
      make USER_C_MODULES=$micropython_dir/MicroFMU-library/micropython.cmake

4. **Flash MicroPython onto ESP32**

   .. code-block:: bash

      make erase
      make deploy USER_C_MODULES=$micropython_dir/MicroFMU-library/micropython.cmake

5. **Run FMU Simulation on ESP32**

   Connect via serial and run:

   .. code-block:: python

      >>> import ufmu as fmu
      >>> simulation = fmu.setup_simulation(0, 3, 0.1)
      >>> for step, time, h, _, v, _, g, e in simulation:
      ...     print(time, h)

