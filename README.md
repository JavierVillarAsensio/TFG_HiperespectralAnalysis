# Análisis de imágenes hiperespectrales en infraestructura con docker - ESP
El proyecto consiste en dada una imagen hiperespectral y unas firmas de referencia de diferentes materiales, determinar qué tipo de material correspeonde a la imagen 2D según las firmas, para paralelizar el trabajo se usan contenedores de **docker**, uno para el que se va a encargar de crear la imagen final con el mapa de colores y uno por cada firma de referencia que se vaya a usar. 

## Estructura
En la carpeta raíz estan los archivos para ejecutar los **contenedores** de docker y los *.yaml* de configuración para las diferentes versiones. También está el Makefile utilizado para la versión sin docker y para limpiar archivos temporales. \
En la carpeta *code* se encuentran todos los archivos C/C++ para la clasificación. En la propia carpeta están los archivos .cpp correspondientes al lector de firmas y al master de las diferentes versiones y en la carpeta *include* están las implementaciones comunes utilizadas por los archivos ya mencionados. \
También, en la carpeta *include* se encuentra un archivo de cabecera de C utilizado para la creación de la imagen .jpg, este archivo es original del repositorio https://github.com/nothings/stb/blob/master/stb_image_write.h en el que se pueden encontrar diferentes implementaciones para la creación de imágenes .jpg. \
En la carpeta *jasperRidge2_R198* se encuentran dos archivo homónimos con diferente extensión; el archivo *.img* es un archivo binario que contiene la imagen hiperespectral y el archivo *.hdr* contiene los metadatos necesarios para interpretarla y un archivo *.mat* que se usará para comparar el resultado con la referencia. \
En la carpeta *output* se va a utilizar para guardar todos los archivos resultado de la ejecución. Dentro está la carpeta *logs* donde cada contenedor tendrá un archivo de logs, y *distances* donde los contenedores que leen las firmas van a guardar su resultado para que puedan ser utilizados por el master. Después de la ejecución en esta carpeta habrá dos archivos, una leyenda para saber a qué material corresponde cada color y el propio mapa de colores. \
En la carpeta *spectrums* están las firmas de referencia que van a ser usadas. \
En la carpeta *Docker_images* están los Dockerfile con los que se crean las imágenes que se van a usar en las versiones de docker, hay una para el master, una para las réplicas en la versión compose de docker y otra para las réplicas en la versión swarm de docker. \
En la carpeta *testing* hay dos carpetas; *black_box* contiene a su vez tres carpetas; *code_test* contiene los archivos de código que realizan pruebas de las diferentes funciones del código principal, *jasperRidge2_R198_test* que contiene un archivo de cabecera de la imagen hiperespectral simplificada para las pruebas y *spectrums_test* que tiene firmas de referencia simplificadas para las pruebas, la otra carpeta dentro de *testing* es *white_box* donde hay un script que creará la imagen con la versión sin docker con la de la versión de docker. \
En la carpeta *performance* hay scripts para ejecutar las 3 versiones con diferentes cantidades de firmas y guardar el tiempo que tardan. \
En la carpeta *perf_specs* hay varias carpetas con más firmas de referencia para usar en las pruebas de performance.

## Requisitos
Se necesita un sistema operativo basado en **UNIX** que tenga disponibles:
- Docker https://docs.docker.com/engine/install/ubuntu/
- g++ https://linuxconfig.org/how-to-install-g-the-c-compiler-on-ubuntu-20-04-lts-focal-fossa-linux
- shell bash *sudo apt install bash* (Ubuntu version)
- make *sudo apt install make* (Ubuntu version)
- librería matio *sudo apt-get install libmatio-dev* (Ubuntu version)
- librerías de python3: numpy, matplot y sklearn

## Ejecución
Hay 3 maneras de ejecutar según el desarrollo iba avanzando se han ido mejorando:
1. Con make. Con el comando *make static_execution* se ejecutará el lector de espectros y después el master que creará la solución
2. Versión compose. Con el script *run.sh* creará los contenedores necesarios con compose, un contenedor con la imagen del lector de firmas por cada firma de referencia. Para limpiar los contenedores *docker compose -f docker-compose-replicas.yaml down*.
3. Versión swarm. Con el script *run_swarm.sh* crea los nodos de swarm de forma dinámica que realizan la misma función que en la versión de compose, para limpiar los contenedores *docker stack rm <tab>*.
\
Cuando acabe la ejecución de cualquiera de las opciones se pueden borrar los archivo de la carpeta *output* con el comando *make clean*.

## Autor
Javier Villar Asensio

# Hyperspectral Image Analysis in Infrastructure with Docker - ENG

The project consists of analyzing a hyperspectral image and reference signatures of different materials to determine which material corresponds to the 2D image based on the signatures. To parallelize the process, **docker** containers are used: one for creating the final image with the color map, and one for each reference signature used.

## Structure

In the root folder, there are files to execute the **docker** containers and the *.yaml* configuration files for different versions. There is also a Makefile used for the non-docker version and to clean temporary files.  
In the *code* folder, you will find all the C/C++ files for classification. The folder contains the .cpp files for the signature reader and the master files for the different versions, and in the *include* folder, you’ll find the common implementations used by the aforementioned files.  
Additionally, in the *include* folder, there is a C header file used for creating the .jpg image. This file is original from the repository https://github.com/nothings/stb/blob/master/stb_image_write.h, where you can find different implementations for generating .jpg images.  
In the *jasperRidge2_R198* folder, there are two files with the same name but different extensions: the *.img* file is a binary file containing the hyperspectral image, and the *.hdr* file contains the metadata necessary to interpret it. There is also a *.mat* file, which will be used to compare the result with the reference.  
The *output* folder is used to store all the output files from the execution. Inside, there is a *logs* folder, where each container will have its own log file, and a *distances* folder, where containers that read the signatures will store their results for use by the master. After execution, this folder will contain two files: a legend indicating which material corresponds to each color and the color map itself.  
In the *spectrums* folder, you will find the reference signatures that will be used.  
The *Docker_images* folder contains the Dockerfiles to build the images used in the Docker versions: one for the master, one for replicas in the Docker Compose version, and another for replicas in the Docker Swarm version.  
In the *testing* folder, there are two subfolders: *black_box*, which contains three additional folders: *code_test* (with code files for testing different functions of the main code), *jasperRidge2_R198_test* (with a header file for the simplified hyperspectral image for testing), and *spectrums_test* (with simplified reference signatures for testing). The other folder in *testing* is *white_box*, which includes a script that compares the image generated by the non-Docker version with the one generated by the Docker version.  
The *performance* folder contains scripts to run all three versions with different numbers of signatures and record the time taken.  
The *perf_specs* folder contains several subfolders with more reference signatures for use in performance tests.

## Requirements

A **UNIX-based** operating system with the following tools and libraries is required:
- Docker: https://docs.docker.com/engine/install/ubuntu/
- g++: https://linuxconfig.org/how-to-install-g-the-c-compiler-on-ubuntu-20-04-lts-focal-fossa-linux
- bash shell: *sudo apt install bash* (Ubuntu version)
- make: *sudo apt install make* (Ubuntu version)
- matio library: *sudo apt-get install libmatio-dev* (Ubuntu version)
- Python3 libraries: numpy, matplotlib, and sklearn

## Execution

There are 3 ways to execute, each improving as the development progressed:
1. **Using make**: The command *make static_execution* will execute the spectrum reader and then the master, which will generate the solution.
2. **Compose version**: The *run.sh* script will create the necessary containers using Compose, one container for each signature reference. To clean the containers, use *docker compose -f docker-compose-replicas.yaml down*.
3. **Swarm version**: The *run_swarm.sh* script dynamically creates swarm nodes that perform the same function as in the Compose version. To clean the containers, use *docker stack rm <tab>*.

After completing the execution of any option, you can remove files from the *output* folder using the command *make clean*.

## Author

Javier Villar Asensio
