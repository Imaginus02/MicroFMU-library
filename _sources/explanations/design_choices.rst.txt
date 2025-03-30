Design Choices
==============

MicroFMU is designed for DIY enthusiasts and developers looking to run FMU simulations on embedded systems with minimal setup, balancing portability, performance, and ease of use while ensuring efficient execution on resource-constrained devices.

Key Considerations
------------------

* **Simple Workflow**  
  The integration process is designed to be as straightforward as compiling MicroPython, minimizing setup complexity.

* **Single FMU Embedding**  
  As a design choice for optimal resource usage, only one FMU is embedded in an embedded environment.

* **Streamlined API**  
  A lightweight and intuitive Python API simplifies FMU execution, making it accessible for DIY enthusiasts and developers.

* **Minimal Memory Footprint**  
  The library only retains the latest simulation step, ensuring efficient memory usage on devices with limited RAM.

* **Cross-Platform Compatibility**  
  Designed to work seamlessly across Unix and ESP32 environments with minimal modifications.

By focusing on simplicity and efficiency, MicroFMU enables reliable FMU simulations on embedded devices without unnecessary overhead.  
