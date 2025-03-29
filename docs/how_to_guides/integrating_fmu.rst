.. _integrating_fmu:

===================================
Integrating FMUs into MicroFMU
===================================

Overview
--------
This guide details how to preprocess FMUs to work with MicroFMU.

Steps
-----

1. **Place the FMU file in the project directory**

   .. code-block:: bash

      cp path/to/fmu/BouncingBall.fmu .

2. **Ensure preprocessing scripts are executable**

   .. code-block:: bash

      sudo chmod +x parseFMU.sh changeCode.sh

3. **Run FMU preprocessing**

   .. code-block:: bash

      make

4. **Inspect extracted files in ``fmu/`` directory**

   .. code-block:: bash

      ls fmu/

5. **Check ``modelDescription.xml`` for variable names and metadata**

This ensures your FMU is properly integrated into the library.
