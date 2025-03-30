Challenges in Developing the MicroFMU Library
==============================================

Developing a MicroPython library for FMU simulations on the ESP32 presented several challenges, primarily due to the constraints of embedded systems and differences in compilation between Unix and ESP32 environments.

Compilation Differences
------------------------
The Unix and ESP32 ports use different build systems—Makefiles for Unix and CMake/ESP-IDF for ESP32. This required adjustments in build configurations, particularly adding specific flags to resolve function name conflicts during linking.

Memory Constraints
-------------------
With only ~4MB of RAM, memory optimization was essential. The library reuses a single variable for storing simulation results, preventing excessive memory usage while handling large simulations.

Function Naming Conflicts
-------------------------
FMU source code includes function prefixes that caused linker issues on ESP32. To resolve this, the library dynamically modifies function names during preprocessing to ensure compatibility.

Performance Considerations
--------------------------
Due to the ESP32’s lower processing power, simulations can be slow. The library minimizes computational overhead and supports future optimizations like parallel processing or hardware acceleration.

Testing and Validation
----------------------
The Unix port facilitated debugging, but the ESP32 version required additional testing due to hardware constraints. The bouncing ball simulation was instrumental in identifying issues related to memory, function names, and execution speed.
