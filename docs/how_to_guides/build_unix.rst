.. _build_unix:

Building and Running MicroFMU on Unix
========================================

Overview
--------
This guide walks you through compiling and running the MicroFMU library on a Unix-based system using MicroPython.

Prerequisites
-------------
- A Unix-based system (Linux/macOS)
- Installed dependencies: ``gcc``, ``make``, ``python3``, and ``pip``
- MicroPython source code (``micropython_dir`` set correctly)

Steps
-----

1. **Navigate to the Unix port directory**

   .. code-block:: bash

      cd "$micropython_dir/ports/unix"

2. **Compile MicroPython with MicroFMU support**

   .. code-block:: bash

      make submodules
      make USER_C_MODULES=$micropython_dir/MicroFMU-library

3. **Run MicroPython and test FMU simulation**

   .. code-block:: bash

      ./build-standard/micropython

   .. code-block:: python

      >>> import ufmu as fmu
      >>> simulation = fmu.setup_simulation(0, 3, 0.1)
      >>> for step, time, h, _, v, _, g, e in simulation:
      ...     print(time, h)

4. **(Optional) Enable Debugging**

   .. code-block:: bash

      make DEBUG=1 STRIP= USER_C_MODULES=$micropython_dir/MicroFMU-library/micropython.cmake
