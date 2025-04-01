MicroFMU `ufmu` API Reference
==============================

This section provides details on available functions in the MicroFMU module.

**Module Name**: ``ufmu``

Functions
---------
- **setup_simulation(start, stop, step)**  
  Initializes an FMU simulation instance.

- **get_variables_names()**  
  Returns a tuple of available FMU variable names.

- **get_variables_description()**  
  Returns descriptions of FMU variables.

- **get_variables_base_values()**  
  Returns initial values of FMU variables.

- **change_variable_value(sim, var_name, new_value)**  
  Modifies the value of a variable within a running simulation.

Example Usage
-------------
.. code-block:: python

   import ufmu as fmu
   sim = fmu.setup_simulation(0, 3, 0.1)  # start, stop, step
   print(fmu.get_variables_names())
   fmu.change_variable_value(sim, "h", 20)
   for step in sim:
       print(step)