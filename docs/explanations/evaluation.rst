.. _evaluation:

Evaluation
==========

To ensure reliable and accurate performance measurements, each simulation was executed 10 times under identical conditions, with the mean value computed to minimize the effect of outliers. The simulations were conducted using the `BouncingBall.fmu` model with two step sizes: 0.01 and 0.0001, as detailed in the tables below.

Test Environment
----------------

The experiments were performed on three different implementations:

- **Plain C Simulator**: Executed within a WSL2 environment on a Windows 10 system with an Intel Core i7-10750H CPU.
- **MicroPython Unix Port**: Also run within WSL2.
- **MicroPython ESP32**: Executed directly on the embedded ESP32 hardware.

Performance Analysis
--------------------

- The **Plain C simulator** was the fastest, benefiting from low-level optimizations and minimal overhead. However, it has significantly higher memory usage since it stores all step results.
- The **MicroPython Unix port** performed well on smaller simulations but failed to complete the larger simulation (step size = 0.0001) due to a well-known bug.
- The **MicroPython ESP32 implementation** successfully completed both simulations with very low RAM usage, but is obviously slower due to CPU limitations.

Simulation Code
---------------

The following MicroPython script was used to measure execution time:

.. code-block:: python

    import ufmu as fmu
    import time
    import micropython
    
    start_time = 0
    end_time = 10
    step_size = XX  # Replaced with 0.01 or 0.0001
    
    start = time.ticks_ms()
    simInstance = fmu.setup_simulation(start_time, end_time, step_size)
    fmu.change_variable_value(simInstance, 'h', 20)
    
    for i in simInstance:
        pass
    
    print("Time taken: ", time.ticks_diff(time.ticks_ms(), start), "ms")
    micropython.mem_info()

Results
-------

Performance results are summarized in the tables below.

.. table:: Performance with step_size = 0.01
   :align: center
   :widths: auto

   +-------------------------+----------------------+----------------------+----------------------+
   | **Category**            | **Plain C Simulator**| **MicroPython Unix** | **MicroPython ESP32**|
   +=========================+======================+======================+======================+
   | Time (ms)               | 1                    | 2                    | 139                  |
   +-------------------------+----------------------+----------------------+----------------------+
   | Memory usage (KB)       | 1.632                | 355                  | 21                   |
   +-------------------------+----------------------+----------------------+----------------------+
   | Executable size (KB)    | 112                  | 843                  | 1.725                |
   +-------------------------+----------------------+----------------------+----------------------+

.. table:: Performance with step_size = 0.0001. X means that the test couldn't be conducted
   :align: center
   :widths: auto

   +-------------------------+----------------------+----------------------+----------------------+
   | **Category**            | **Plain C Simulator**| **MicroPython Unix** | **MicroPython ESP32**|
   +=========================+======================+======================+======================+
   | Time (ms)               | 1.005                | X                    | 141.309              |
   +-------------------------+----------------------+----------------------+----------------------+
   | Memory usage (KB)       | 64.032               | X                    | 46                   |
   +-------------------------+----------------------+----------------------+----------------------+
   | Executable size (KB)    | 112                  | 843                  | 1.725                |
   +-------------------------+----------------------+----------------------+----------------------+

Conclusion
----------

The results demonstrate that MicroPython provides a flexible, efficient, and easy-to use option to execute FMU simulation in embedded systems like ESP32. 