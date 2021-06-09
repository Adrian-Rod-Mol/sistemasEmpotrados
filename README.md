# Práctica final de sistemas empotrados

**Autor:** Adrián Rodríguez Molina

**Fecha:** 09/06/2021

**Descripción:**

Esta práctica ha consistido en la relización de una interfaz gráfica para una sensor infrarojo GRID-EYE equipado con el sensor AMG8833. 
Esta intefáz gráfica se ha realizado sobre la plataforma de desarrollo **STM32F429I-Discovery** equipada con un Cortex M4 de STM. La interfaz gráfica
se ha diseñando usando la herramienta **TouchGFX Designer** proporcionada por STM y el código de la placa se ha generado y editado usando las herramientas
de **CubeMX** y **CubeIDE** proporcionadas por la misma compañía y todas disponibles gratuitamente.

La interfaz gráfica diseñada cuenta con dos pantallas: una pantalla principal que cuenta con un botón para acceder a la pantalla de configuración, un botón
para activar o desactivar la cámara, uno para envíar por el puerto serie los datos obtenidos por el sensor de la cámara y un último botón que muestra la temperatura 
central de los píxeles de la cámara. La segunda pantalla cuenta con un botón para volver a la pantalla principal y unos botones que permiten configurar 
los parámetros de obtención y representación de datos de la cámara.

Ambas pantallas cuentan con una barra superior que muestra datos relacionados con la temperatura del núcleo de la plataforma, datos sobre un termistor incluido en
la cámara y los frames por segundo de refresco de la pantalla.

Además de estas funcionalidades se puede acceder a la interfaz por comandos enviados a través de la USART con la que cuenta la plataforma de desarrollo, permitiendo
así controlar la interfaz y variar los parámetros con los que cuenta. Estos comandos se detallan más adelante en este documento.

El control de la plataforma de desarrollo se realiza usando el sistema operativo en tiempo real **FreeRTOS**, que es tambien usado por el **TouchGFX** y al que se
accede a través de la API **CMSIS-RTOS-V2**.

### Lista de comandos CLI
```
Nombre         Descripción

help         - Solicitar comandos del programa
cam on       - Enciende la camara
cam off      - Apaga la camara
cam temp     - Solicitar datos Temperatura
cam frame    - Solicitar datos de la matriz de temperatura
cam raw      - Solicitar datos raw de la matriz de temperatura
cam set rate - Establece el framerate de la camara
cam get rate - Devuelve el framerate de la camara
set max temp - Establece la temperatura maxima de la escala del frame
get max temp - Devuelve la temperatura maxima de la escala del frame
set min temp - Establece la temperatura minima de la escala del frame
get min temp - Devuelve la temperatura minima de la escala del frame
cpu temp     - Devuelve la temperatura de la cpu
cpu echo on  - Envia la temperatura de la cpu cada 500 ms
cpu echo off - Deja de enviar la temperatura de la cpu cada 500 ms

```
### Lista de archivos usados

> A excepción del archivo de **board.c**, todos los archivos cuentan con un archivo .h/.hpp en el que se declaran las variables y funciones que se usan
> luego en los archivos .c/.cpp. Por tanto, solo se mencionará en la siguiente lista estos últimos, pero aun así se informa al lector de que estos archivos
> existen y de que cuentan con el mismo nombre que el archivo de la lista pero cambiando su terminación.

+ **Archivos generados por el TouchGFX**:
  1. **Model.cpp:** clase central del programa que sirve para gestionar los datos ofrecidos tanto por el sensor termográfico como por las tareas del sitema operativo, sirviendo además de puente entre estas tareas y la interfaz gráfica.
  2. **MainScreenPresenter.cpp:** presenter de la pantalla principal. Se encarga de comunicar al modelo los cambios que han ocurrido en la vista para que este actue en cosnecuencia y además muestra en la vista los cambios ocurridos en el modelo. En esta clase también se genera el bitmap a partir del frame obtenido de la cámara.
  3. **MainScreenView.cpp:** vista de la pantalla principal. Se encarga de avisar al presenter de los cambios de estado de los botones de la pantalla táctil y de mostrar, ocultar y renderizar los widget que se deben mostrar en la pantalla. También se crea el bitmap y se reserva y asigna la caché que le corresponde.
  4. **ConfScreenPresenter.cpp:** presenter de la pantalla de configuración. Se encarga de comunicar al modelo cuando se han pulsado los botones de la vista para que modifique los parámetros de configuración de la cámara y además se encarga de enviar a la vista dichos parámetros de configuración para que esta los muestre.
  5. **ConfScreenView.cpp:** vista de la pantalla de configuración. Se encarga de transmitir al presenter cuando se pulsa alguno de los botones de la pantalla táctil y de cambiar el estado de los parámetros relacionados con estos botones.
