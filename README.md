# MicroFMU Test Library

Ce projet a pour but de fournir une bibliothèque MicroPython permettant de lancer des simulations FMU (Functional Mock-up Units).

## Utilisation

### Problèmes actuels

On doit créer un générateur :
```python
from FMUSimulator import *
simInstance = setup_simulation(StartTime, EndTime, StepSize)
for i in simInstance:
	next(simInstance)
```

On peut modifier les valeurs avant et pendant la simulation (selon les variables) :
```python
bool change_variable_value(simInstance, variableIndex/VariableName, value)
```

Si on clone et utilise sur Unix, il faut faire un chmod +x sur tout les fichiers .sh

## Compilation

Pour compiler micropython avec cette bibliothèque, suivez les étapes suivantes :

1. Placez votre fichier FMU dans le répertoire de la bibliothèque.
	- Actuellement, le compilation de décompresse pas l'archive fmu, il faut extraire son contenu vers un fichier fmu/ 
2. Changez de répertoire et placez-vous dans celui du matériel cible.
3. Compilez MicroPython avec l'option `USER_C_MODULES=[chemin vers le répertoire]`:

```sh
make USER_C_MODULES=path/to/your/library
```

## Structure du projet

- `fmi2.c` : Contient les fonctions de chargement des FMU.
- `testlibrary.c` : Contient des exemples de fonctions et de classes utilisables avec MicroPython.
- `micropython.mk` : Fichier Makefile pour la compilation et le nettoyage du projet.
- `headers/` : Dossier des fichiers C fournis par le standard FMU nécessaires pour la compilation du simulateur.

## Nettoyage

Pour nettoyer le projet, utilisez la commande suivante :

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