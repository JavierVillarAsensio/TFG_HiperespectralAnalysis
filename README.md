# Análisis de imágenes hiperespectrales en infraestructura con docker - ESP
El proyecto consiste en dada una imagen hiperespectral y unas firmas de referencia de diferentes materiales, determinar qué tipo de material correspeonde a la imagen 2D según las firmas, para paralelizar el trabajo se usan contenedores de **docker**, uno para el que se va a encargar de crear la imagen final con el mapa de colores y uno por cada firma de referencia que se vaya a usar. 

## Estructura
En la carpeta raíz estan los archivos para crear las **imágenes** de docker y el *.yaml* utilizado para levantar los servicios con *docker-compose*. También están los archivos de código c++, los scripts para ejecutar y un archivo *Makefile* para automatizar diferentes gestiones. \
En la carpeta *include* se encuentra un archivo de cabecera de C utilizado para la creación de la imagen .jpg, este archivo es original del repositorio https://github.com/nothings/stb/blob/master/stb_image_write.h en el que se pueden encontrar diferentes implementaciones para la creación de imágenes .jpg. \
En la carpeta *jasperRidge2_R198* se encuentran dos archivo homónimos con diferente extensión; el archivo *.img* es un archivo binario que contiene la imagen hiperespectral y el archivo *.hdr* contiene los metadatos necesarios para interpretarla. \
En la carpeta *output* se va a utilizar para guardar todos los archivos resultado de la ejecución. Dentro está la carpeta *logs* donde cada contenedor tendrá un archivo de logs, y *distances* donde los contenedores que aplican las firmas van a guardar su resultado para que puedan ser utilizados por el master. Después de la ejecución en esta carpeta habrá dos archivos, una leyenda para saber a qué material corresponde cada color y el propio mapa de colores. \
En la carpeta *spectrums* están las firmas de referencia que van a ser usadas. \
En la carpeta *static_version* hay una versión anterior a la finale en la que se creaba un servicio por cada firma de referencia.

## Requisitos
Se necesita un sistema operativo basado en **UNIX** que tenga disponibles:
- Docker https://docs.docker.com/engine/install/ubuntu/
- g++ https://linuxconfig.org/how-to-install-g-the-c-compiler-on-ubuntu-20-04-lts-focal-fossa-linux
- shell bash *sudo apt install bash* (Ubuntu version)
- make *sudo apt install make* (Ubuntu version)

## Ejecución
Hay 3 maneras de ejecutar según el desarrollo iba avanzando se han ido mejorando:
1. Con make (no recomendado). Se necesita ir indicando una por una la firma de referencia en el archivo spectrum.cpp y después ejecutar el master con *make run_spectrum* lo compila y ejecuta y con *make run_master* ejecuta el master, con *make* ejecuta el spectrum que haya junto con el master.
2. Versión estática. Crear las imágenes con el script *images.sh* y dentro del directorio *static_version* ejecutar el comando *docker-compose up*. Para borrar los contenedores al acabar *docker-compose down*.
3. Versión final (recomendado). En el directorio raiz ejecutar el script *run.sh*. Para borrar los contenedores al acabar *docker-compose -f docker-compose-replicas.yaml down*.
\
Cuando acabe la ejecución de cualquiera de las opciones se pueden borrar los archivo de la carpeta *output* con el comando *make clean*.

## Autor
Javier Villar Asensio

# Analysis of hyperspectral images in infrastructure with docker - ENG
The project consists of, given a hyperspectral image and reference signatures of different materials, determining what type of material corresponds to the 2D image according to the signatures, to parallelize the work **docker** containers are used, one container will be in charge of creating the final image with the color map and one for each reference signature that will be used.

## Structure
In the root folder are the files to create the docker **images** and the *.yaml* used to build the services with *docker-compose*. There are also the c++ code files, the scripts to run and a *Makefile* file to automate different procedures. \
In the *include* folder there is a C header file used for creating the .jpg image, this file is original from the repository https://github.com/nothings/stb/blob/master/stb_image_write.h in which you can find different implementations for creating .jpg images. \
In the *jasperRidge2_R198* folder there are two homonymous files with different extensions; The *.img* file is a binary file that contains the hyperspectral image and the *.hdr* file contains the metadata necessary to interpret it. \
The *output* folder will be used to save all the files resulting from the execution. Inside is the *logs* folder where each container will have a log file, and *distances* where the containers that apply the signatures will save their results so that they can be used by the master. After execution, there will be two files in this folder, a legend to know which material each color corresponds to and the color map itself. \
In the *spectrums* folder are the reference signatures that will be used. \
In the *static_version* folder there is a version before the finale in which a service was created for each reference signature.

## Requirements
You need a **UNIX**-based operating system that has available:
- Docker https://docs.docker.com/engine/install/ubuntu/
- g++ https://linuxconfig.org/how-to-install-g-the-c-compiler-on-ubuntu-20-04-lts-focal-fossa-linux
- bash shell *sudo apt install bash* (Ubuntu version)
- make *sudo apt install make* (Ubuntu version)

## Execution
There are 3 ways to execute as development has progressed and they have been improved:
1. With make (not recommended). You need to indicate one by one the reference signature in the spectrum.cpp file and then run the master with *make run_spectrum* compile it and execute it and with *make run_master* execute the master, with *make* execute the spectrum that you have along with the master.
2. Static version. Create the images with the *images.sh* script and within the *static_version* directory run the *docker-compose up* command. To delete the containers when *docker-compose down* ends.
3. Final version (recommended). In the root directory execute the script *run.sh*. To delete the containers when finished *docker-compose -f docker-compose-replicas.yaml down*.
\
When the execution of any of the options finishes, the files in the *output* folder can be deleted with the *make clean* command.

## Author
Javier Villar Asensio