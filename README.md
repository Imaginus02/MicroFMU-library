# MicroFMU Test Library

This project aims to provide a MicroPython library to run FMU (Functional Mock-up Units) simulations.  
Documentation can be seen [here](https://imaginus02.github.io/MicroFMU-library/index.html)

## How to cite

> Reynaud, T., Enferad, E. and Lefrançois, M. (2025). Facilitating the use of Physics-Based Simulations on Embedded Devices by running FMUs from MicroPython. In: Proceedings of the 16th International Modelica and FMI Conference. Lucerne, Switzerland.


## Step 1: Preparation of the library

To compile MicroPython with this library, follow these steps:

1. Open your terminal.

2. Clone and enter the MicroPython repository as well as set the `micropython_dir` variable:

```shell
git clone -b v1.24.1 --depth 1 --recurse-submodules --shallow-submodules https://github.com/micropython/micropython.git
micropython_dir="$(pwd)/micropython"
```

3. Clone this repository inside the MicroPython directory

```shell
cd $micropython_dir
git clone https://github.com/Imaginus02/MicroFMU-library.git
cd $micropython_dir/MicroFMU-library
```

4. Place your FMU file in the library directory (at the same level as this README). This tutorial uses `BouncingBall.fmu`

```shell
cp path/to/fmu/BouncingBall.fmu .
```

5. Ensure that `parseFMU.sh` and `changeCode.sh` are executable:

```shell
sudo chmod +x parseFMU.sh
sudo chmod +x changeCode.sh
```

6. Run the make command:

```shell
make
```

## Step 2: Build MicroPython with "ufmu"

### UNIX port

1. Navigate to the target port directory in the MicroPython project.

```bash
cd "$micropython_dir/ports/unix"
```

2. Compile MicroPython with the following command:

```shell
make submodules
make USER_C_MODULES=$micropython_dir/MicroFMU-library
```

After build, you can run it by:

```shell
$ ./build-standard/micropython
```

___
### ESP32 port

To build and deploy MicroPython with "MicroPython-FMU" on an ESP32, follow these steps:

1. Navigate to the ESP32 port directory:

```bash
cd "$micropython_dir/ports/esp32"
```

2. Follow the steps in the [ESP32 README.md](https://github.com/micropython/micropython/blob/master/ports/esp32/README.md) until you reach the `make submodules` step.

3. After running `make submodules`, connect your ESP32 to your computer. If you are on Windows, follow the appropriate steps for your WSL installation. If you are on Linux, skip to step 5.

#### On WSL1
*TODO: Add specific instructions for WSL1.*

#### On WSL2
Open PowerShell and run the following command to list all USB connected devices (you may need to install a powershell library using this [tutorial](https://learn.microsoft.com/en-us/windows/wsl/connect-usb)):

```powershell
usbipd list
```

Identify your ESP32 device from the list and then run the following commands:

```powershell
usbipd bind --busid [bus-device]
usbipd attach --wsl --busid [bus-device]
```

Note: The `usbipd bind` command only needs to be run once, even after disconnecting the ESP32.

4. In WSL2, run the following commands to set up the USB serial connection:

```bash
sudo modprobe usbserial
sudo modprobe cp210x
sudo chmod a+rw /dev/ttyUSB0
```

5. Finally, erase the flash and deploy MicroPython with the FMU library:

```bash
make erase
make deploy USER_C_MODULES=$micropython_dir/MicroFMU-library/micropython.cmake
```

## Run MicroPython with "ufmu"
Initialize and run the simulation as follows:

```$ ./build-standard/micropython
>>> import ufmu as fmu
>>> fmu.get_variables_names()
('step', 'time', 'h', 'der(h)', 'v', 'der(v)', 'g', 'e')
>>> fmu.get_variables_description()
('Simulation step count', 'Simulation time', 'Position of the ball', 'Derivative of h', 'Velocity of the ball', 'Derivative of v', 'Gravity acting on the ball', 'Coefficient of restitution')
>>> fmu.get_variables_base_values()
(0, 0.0, 1.0, 0.0, 0.0, 0.0, -9.81, 0.7)
>>> simulation = fmu.setup_simulation(0, 3, 0.1) # startTime, stopTime, stepSize
>>> fmu.change_variable_value(simulation, 'h', 20)
True
>>> for step, time, h, der_h, v, der_v, g, e in simulation:
...   print(time, h)
```

With `BouncingBall.fmu`, you should get something like:

```
0.1 20.0
0.2 19.9019
... [ more lines ] ...
2.800000000000001 8.034390000000011
2.900000000000001 8.789760000000012
``` 
## Project Structure
- `micropython.cmake`: CMake file for compilation to ESP32.
- `parseFMU.sh`: Script to parse and extract FMU files.
- `Makefile`: Main Makefile for project compilation and cleanup management.
- `library/`: Directory containing the project code.
	- `headers/`: Folder of C files provided by the FMU standard necessary for simulator compilation.
	- `micropython.mk`: Makefile for Unix compilation.
	- `main.c`: Main C file containing the implementation of the simulator and MicroPython functions.

## Cleanup

To clean the project, use the following command from the main library directory:

```sh
make clean
```

This will remove the `fmu` directory and its contents, as well as the modelDescription.c file.  
*Note: Functional only if run from the library directory*  

## Remarks

- The `.fmu` files will be automatically decompressed into the `fmu` directory during compilation.  
- The C simulator, without the Python functions, is available [here](https://github.com/Imaginus02/FMUSimulator)
- To enable debugging, when preparing the library, add the DEBUG=1 arguement:
```bash
make DEBUG=1
```
Then when you compile Micropython, add those two arguements:
#### Unix
```bash
make DEBUG=1 STRIP= USER_C_MODULES=$micropython_dir/MicroFMU-library/micropython.cmake
```
#### ESP32
```bash
make deploy DEBUG=1 STRIP= USER_C_MODULES=$micropython_dir/MicroFMU-library/micropython.cmake
```
