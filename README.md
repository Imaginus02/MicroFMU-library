# MicroFMU Test Library

Ce projet a pour but de fournir une bibliothèque MicroPython permettant de lancer des simulations FMU (Functional Mock-up Units).

## Compilation

Pour compiler MicroPython avec cette bibliothèque, suivez les étapes suivantes :


1. Open your terminal.

2. Clone and enter the MicroPython repository:

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

1. Placez votre fichier FMU dans le répertoire de la bibliothèque (au même niveau que ce README). Ce tutoriel utilise `BouncingBall.fmu`


```shell
cp path/to/fmu/BouncingBall.fmu .
```

2. Assurez-vous que `parseFMU.sh` est exécutable:

```shell
sudo chmod +x parseFMU.sh
```

2. Lancez la commande make:

```shell
make
```

## Step 3: Build and Run MicroPython with "MicroPython-FMU"

### UNIX port

3. Placez-vous dans le répertoire du port cible dans le projet MicroPython.

```bash
cd "$micropython_dir/ports/unix"
```
4. Compilez MicroPython avec la commande suivante :

```shell
make submodules
make USER_C_MODULES=$micropython_dir/MicroFMU-library
```

After build, you can run it by:

```shell
$ ./build-standard/micropython
```

Initialize and run the simulation as follows:

```$ ./build-standard/micropython
>>> import FMUSimulator as fmu
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

### ESP32 port 


```bash
cd "$micropython_dir/ports/esp32"
```

Follow the steps in the [README.md](https://github.com/micropython/micropython/blob/master/ports/esp32/README.md)

Then, after `make submodules`, run `make` with the following arguments: 


```
make USER_C_MODULES=$micropython_dir/MicroFMU-library/micropython.cmake
```

## Structure du projet
- `micropython.cmake` : Fichier CMake pour la compilation vers ESP32.
- `parseFMU.sh` : Script pour analyser et extraire les fichiers FMU.
- `Makefile` : Fichier Makefile principal pour la gestion de la compilation et du nettoyage du projet.
- `library/` : Répertoire contenant le code du projet.
	- `headers/` : Dossier des fichiers C fournis par le standard FMU nécessaires pour la compilation du simulateur.
	- `micropython.mk` : Fichier Makefile pour la compilation vers Unix.
	- `main.c` : Fichier C principal contenant l'implémentation du simulateur et des fonctions MicroPython.


## Utilisation


## Nettoyage

Pour nettoyer le projet, utilisez la commande suivante depuis le répertoire principal de la bibliothèque :

```sh
make clean
```

Cela supprimera le répertoire `fmu` et son contenu, ainsi que le fichier modelDescription.c.  
*Nb : Fonctionnel uniquement si lancé depuis le répertoire de la bibliothèque*  

## Remarques

- Les fichiers `.fmu` seront automatiquement décompressés dans le répertoire `fmu` lors de la compilation.  
- Le simulateur en C, sans les fonctions Python, est disponible [ici](https://github.com/Imaginus02/FMUSimulator)

## License

### fmuSDK

Les fichiers main.c et fmi2.c sont basé sur ceux du projet github [fmuSDK](https://github.com/qtronic/fmusdk)  

Copyright (c) 2008-2018, QTronic GmbH. All rights reserved. The FMU SDK is licensed by the copyright holder under the 2-Clause BSD License:

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

**THIS SOFTWARE IS PROVIDED BY QTRONIC GMBH "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL QTRONIC GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.**
---
Piste de débug : [Même problème avec seulement esp-idf]https://stackoverflow.com/questions/67039814/linker-error-in-esp-idf-framework-undefined-reference
