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
así controlar la interfaz y variar los parámetros con los que cuenta. Estos comandos se detallan al final de este documento.


