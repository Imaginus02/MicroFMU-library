Getting Started
===============

This tutorial walks you through setting up and running your first FMU simulation with MicroPython.

Prerequisites
-------------
* **MicroPython source code**: Clone the MicroPython repository.
* **MicroFMU Test Library**: Clone this repository inside MicroPython.
* **Functional Mock-up Unit (FMU)**: Obtain an `.fmu` file for testing.
* **Development environment**: Linux/macOS/WSL (Windows).

Installation Steps
------------------
1. **Clone the MicroPython repository**  

   .. code-block:: bash

      git clone -b v1.24.1 --depth 1 --recurse-submodules --shallow-submodules https://github.com/micropython/micropython.git
      micropython_dir="$(pwd)/micropython"

2. **Clone the MicroFMU library**  

   .. code-block:: bash

      cd $micropython_dir
      git clone https://github.com/Imaginus02/MicroFMU-library.git
      cd MicroFMU-library

3. **Place your FMU file in the library directory**  

   Example:  
   .. code-block:: bash

      cp path/to/fmu/BouncingBall.fmu .

4. **Ensure required scripts are executable**  

   .. code-block:: bash

      chmod +x parseFMU.sh changeCode.sh

5. **Build the MicroFMU library**  

   .. code-block:: bash

      make

6. **Compile MicroPython with MicroFMU (Unix target)**  

   .. code-block:: bash

      cd "$micropython_dir/ports/unix"
      make submodules
      make USER_C_MODULES=$micropython_dir/MicroFMU-library

7. **Run MicroPython with FMU support**  

   .. code-block:: bash

      ./build-standard/micropython