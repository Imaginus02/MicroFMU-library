# MicroFMU Test Library

Ce projet a pour but de fournir une bibliothèque MicroPython permettant de lancer des simulations FMU (Functional Mock-up Units).

## Compilation

Pour compiler MicroPython avec cette bibliothèque, suivez les étapes suivantes :


1. Open your terminal.

2. Clone and enter the MicroPython repository:

```shell
$ git clone -b v1.24.1 --depth 1 --recurse-submodules --shallow-submodules https://github.com/micropython/micropython.git
$ micropython_dir="$(pwd)/micropython"
```

3. Clone this repository inside the MicroPython directory

```shell
$ cd $micropython_dir
$ git clone https://github.com/Imaginus02/MicroFMU-library.git
$ cd $micropython_dir/MicroFMU-library
```

1. Placez votre fichier FMU dans le répertoire de la bibliothèque (au même niveau que ce README).


```shell
$ mv path/to/fmu/BouncingBall.fmu .
```

2. Assurez-vous que `parseFMU.sh` est exécutable:

```shell
$	sudo chmod +x parseFMU.sh
```

2. Lancez la commande make:

```shell
make
```

## Step 3: Build and Run MicroPython with "urdflib"

### UNIX port

3. Placez-vous dans le répertoire du port cible dans le projet MicroPython.

```bash
$ cd "$micropython_dir/ports/unix"
```
4. Compilez MicroPython avec la commande suivante :

```shell
$ make clean
$ make submodules
$ make USER_C_MODULES=$micropython_dir/MicroFMU-library
```

After build, you can run it by:

```shell
$ ./build-standard/micropython
```

Initialize and run the simulation as follows:

```$ ./build-standard/micropython
from FMUSimulator import *
simInstance = setup_simulation(0, 3, 0.1)
for i in simInstance:
	print(simInstance)
```



	- Pour ESP32 :
		```shell
		make USER_C_MODULES=full/path/to/micropython.cmake
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
