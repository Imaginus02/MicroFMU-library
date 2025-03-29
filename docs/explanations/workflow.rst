Workflow
===================

MicroFMU integrates FMU execution within MicroPython by embedding a lightweight FMU engine into a C module.

**Key Components**

* **FMU Preprocessing**: Extracts metadata and source code.
* **MicroPython C Module**: Wraps FMU functions for MicroPython.
* **Unix & ESP32 Targets**: Separate compilation workflows.


.. figure:: /_static/MicroFMU_pipeline.png
   :alt: Flow diagram of MicroPython firmware creation process
   :align: center
   :width: 80%

   **Figure 1**: Flow diagram illustrating the integration of the FMU execution engine, preprocessing, and compilation steps in the MicroPython firmware creation process.

**Steps:**


* A. FMU archive file is obtained from the simulation, with sources in C.
* B. FMU archive file is decompressed into folder fmu
* C. Constitute the library for usage with MicroPython:
    * Adapt the source C code to the constraints of an embedded environment
    * FMU metadata is parsed for variable definitions
* D. MicroPython is compiled for Unix/ESP32 with MicroFMU
* E. MicroPython interacts with FMU via the `ufmu` API