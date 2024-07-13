# Análisis de imágenes hiperespectrales en infraestructura con docker - ESP
El proyecto consiste en dada una imagen hiperespectral y unas firmas de referencia de diferentes materiales, determinar qué tipo de material correspeonde a la imagen 2D según las firmas, para paralelizar el trabajo se usan contenedores de **docker**, uno para el que se va a encargar de crear la imagen final con el mapa de colores y uno por cada firma de referencia que se vaya a usar. 

## Estructura
En la carpeta raíz estan los archivos para crear las **imágenes** de docker y el *.yaml* utilizado para levantar los servicios con *docker-compose*. También están los archivos de código c++, los scripts para ejecutar y un archivo *Makefile* para automatizar diferentes gestiones. \
En la carpeta *include* se encuentra un archivo de cabecera de C utilizado para la creación de la imagen .jpg, este archivo es original del repositorio https://github.com/nothings/stb/blob/master/stb_image_write.h en el que se pueden encontrar diferentes implementaciones para la creación de imágenes .jpg. \
En la carpeta *jasperRidge2_R198* se encuentran dos archivo homónimos con diferente extensión; el archivo *.img* es un archivo binario que contiene la imagen hiperespectral y el archivo *.hdf* contiene los metadatos para conocer los metadatos de la imagen necesarios para interpretarla. \
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
2. Versión estática. Dentro del directorio *static_version* ejecutar el comando *docker-compose up*. Para borrar los contenedores al acabar *docker-compose down*.
3. Versión final (recomendado). En el directorio raiz ejecutar el script *run.sh*. Para borrar los contenedores al acabar *docker-compose -f docker-compose-replicas.yaml down*.
\
Cuando acabe la ejecución de cualquiera de las opciones se pueden borrar los archivo de la carpeta *output* con el comando *make clean*.

## Autor
Javier Villar Asensio