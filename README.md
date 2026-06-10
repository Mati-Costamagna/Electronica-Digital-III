# Electrónica Digital III - Ejercicios Prácticos LPC1769

![C](https://img.shields.io/badge/Language-C-blue)
![Microcontroller](https://img.shields.io/badge/MCU-LPC1769-orange)
![Status](https://img.shields.io/badge/Status-Completado-success)
![License](https://img.shields.io/badge/License-MIT-green)

## 📋 Descripción

Repositorio que contiene una colección de **ejercicios prácticos** desarrollados durante la asignatura **Electrónica Digital III** para aprender y dominar el desarrollo de aplicaciones embebidas en el **microcontrolador ARM LPC1769**.

Este trabajo es parte de la carrera de **Ingeniería en Computación** en la **FCEFyN - Universidad Nacional de Córdoba**.

---

## 🎯 Objetivos de Aprendizaje

- ✅ Programación en **C/C++** para sistemas embebidos ARM
- ✅ Configuración y control de periféricos del LPC1769
- ✅ Manejo de entrada/salida digital y analógica
- ✅ Desarrollo de drivers de hardware
- ✅ Programación de interrupciones y timers
- ✅ Optimización de código para plataformas embebidas
- ✅ Uso de herramientas de desarrollo (compiladores, debuggers, programadores)

---

## 💻 Acerca del LPC1769

El **LPC1769** es un microcontrolador de arquitectura **ARM Cortex-M3** fabricado por NXP Semiconductors, ampliamente utilizado en educación y sistemas embebidos.

### Características Principales

| Característica | Especificación |
|---|---|
| **Arquitectura** | ARM Cortex-M3 |
| **Frecuencia de Reloj** | Hasta 120 MHz |
| **Memoria Flash** | 256 KB |
| **Memoria RAM** | 32 KB |
| **Periféricos** | GPIO, UART, SPI, I²C, ADC, DAC, PWM, Timers |
| **Voltaje de Operación** | 3.3V |
| **Encapsulado** | LQFP100 |

---

## 📂 Estructura del Repositorio

```
Electronica-Digital-III/
├── ejercicio_01_LED/          # Ejemplo básico: Control de LED
│   ├── src/
│   │   └── main.c
│   ├── inc/
│   │   └── config.h
│   └── README.md
├── ejercicio_02_UART/         # Comunicación Serial
│   ├── src/
│   └── README.md
├── ejercicio_03_GPIO/         # Entrada/Salida Digital
│   ├── src/
│   └── README.md
├── ejercicio_04_ADC/          # Conversión Analógico-Digital
│   ├── src/
│   └── README.md
├── ejercicio_05_Interrupciones/  # Manejo de Interrupciones
│   ├── src/
│   └── README.md
├── ejercicio_06_Timers/       # Timers y PWM
│   ├── src/
│   └── README.md
├── ejercicio_07_I2C/          # Comunicación I²C
│   ├── src/
│   └── README.md
├── ejercicio_08_SPI/          # Comunicación SPI
│   ├── src/
│   └── README.md
├── docs/
│   ├── LPC1769_Datasheet.pdf
│   ├── Esquematicos.pdf
│   └── Guia_Desarrollo.md
├── tools/
│   ├── programador_firmware.sh
│   └── scripts_build.sh
└── README.md
```

---

## 🔧 Tecnologías y Herramientas

### Hardware
- **Microcontrolador:** NXP LPC1769 (ARM Cortex-M3)
- **Placa de Desarrollo:** LPCXpresso 1769 (o similar)
- **Programador/Debugger:** JTAG, SWD, UART bootloader

### Software
- **Lenguaje:** C (estándar ANSI C99)
- **Compilador:** GCC ARM (arm-none-eabi-gcc)
- **IDE (opcional):** LPCXpresso IDE, Keil µVision
- **Herramientas:** Make, CMake, OpenOCD

---

## 🚀 Requisitos y Setup

### Requisitos de Software

```bash
# Ubuntu/Debian
sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi gdb-arm-none-eabi

# macOS (con Homebrew)
brew tap ARMmbed/homebrew-formulae
brew install arm-none-eabi-gcc

# Windows
# Descargar desde: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain-arm-embedded
```

### Requisitos de Hardware

- Placa de desarrollo LPC1769 (LPCXpresso 1769, Embedded Artists LPC1769, etc.)
- Cable USB para programación/debug
- Componentes electrónicos (LED, resistencias, botones, sensores, etc.)

---

## 📖 Guía de Ejercicios

### Ejercicio 1: Control de LED
**Objetivo:** Aprender configuración básica de GPIO y control de salidas digitales.

```c
// Ejemplo simple
#include "LPC17xx.h"

int main(void) {
    LPC_GPIO0->FIODIR = (1 << 22);  // Pin 0.22 como salida
    LPC_GPIO0->FIOSET = (1 << 22);  // LED encendido
    while(1);
    return 0;
}
```

### Ejercicio 2: Comunicación UART
**Objetivo:** Implementar comunicación serial con PC.

- Configuración de baudrate
- Transmisión y recepción de datos
- Implementación de printf() redirigido

### Ejercicio 3: GPIO e Interrupciones
**Objetivo:** Manejo de entradas digitales y respuesta a eventos.

- Configuración de pines como entradas
- Debouncing de botones
- Interrupciones de cambio de estado

### Ejercicio 4: Conversión ADC
**Objetivo:** Lectura de señales analógicas.

- Configuración del ADC
- Muestreo de canales
- Procesamiento de datos

### Ejercicio 5-8: Periféricos Avanzados
**Objetivos:** Dominar timers, PWM, I²C, SPI para sistemas más complejos.

---

## 💡 Conceptos Clave Abordados

| Concepto | Descripción |
|----------|------------|
| **GPIO** | Control de pines digitales (entrada/salida) |
| **UART** | Comunicación serial asincrónica |
| **ADC** | Conversión de señales analógicas a digitales |
| **PWM** | Modulación por ancho de pulso para control analógico |
| **Interrupciones** | Respuesta a eventos de hardware |
| **Timers** | Contadores y generación de timing |
| **I²C/SPI** | Protocolos de comunicación serial sincronos |
| **Registros** | Acceso directo a periféricos del microcontrolador |

---

## 🔨 Cómo Compilar y Programar

### Compilación

```bash
# Navegar al directorio del ejercicio
cd ejercicio_01_LED

# Compilar
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Wall -O0 -g src/main.c -o build/output.elf

# Generar archivo binario
arm-none-eabi-objcopy -O binary build/output.elf build/output.bin
```

### Programación del Firmware

```bash
# Mediante OpenOCD y JTAG
openocd -f interface/jlink.cfg -f target/lpc1769.cfg

# En GDB:
(gdb) load
(gdb) continue

# O mediante bootloader UART (si disponible)
./tools/programador_firmware.sh build/output.bin /dev/ttyUSB0
```

---

## 📊 Resultados Esperados

Cada ejercicio produce:
- ✅ Código compilable sin errores
- ✅ Firmware funcional en el LPC1769
- ✅ Validación de comportamiento esperado
- ✅ Documentación del procedimiento

---

## 📚 Referencias y Recursos

### Documentación Oficial
- [LPC1769 Datasheet](http://www.nxp.com/) - Especificaciones completas del microcontrolador
- [ARM Cortex-M3 Architecture](https://developer.arm.com/) - Referencia de arquitectura
- [GCC ARM Embedded](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain-arm-embedded) - Compilador toolchain

### Libros Recomendados
- "Embedded Systems with ARM Cortex-M3 Microcontrollers" - Yifeng Zhu
- "The Definitive Guide to ARM Cortex-M3 and Cortex-M4 Processors" - Joseph Yiu

### Tutoriales Online
- LPC17xx Programming Tutorials
- ARM Cortex-M3 Development
- Embedded Systems Best Practices

---

## 🎓 Contexto Académico

- **Asignatura:** Electrónica Digital III
- **Carrera:** Ingeniería en Computación
- **Universidad:** FCEFyN - Universidad Nacional de Córdoba
- **Año:** 2025

---

## 📝 Notas Importantes

- ⚠️ **Voltaje:** El LPC1769 es de 3.3V. Evitar conectar directamente a 5V
- ⚠️ **Protecciones:** Usar resistencias de protección en entradas sensibles
- ⚠️ **Debugging:** Utilizar debugger JTAG/SWD para depuración eficiente
- ✅ **Documentación:** Cada ejercicio debe estar documentado y comentado

---

## 📄 Licencia

Este proyecto se distribuye bajo la licencia MIT.

---

## 👤 Autor

Desarrollado como parte de la asignatura Electrónica Digital III  
Ingeniería en Computación - FCEFyN - UNC

**Última actualización:** Octubre 2025  
**Estado:** ✅ Completado

---

## 🔗 Enlaces Útiles

- [Datasheet LPC1769](http://www.nxp.com/documents/data_sheet/LPC1769_68_67_66_65_64_63.pdf)
- [LPCXpresso IDE](https://www.nxp.com/design/software/development-software/lpcxpresso-ide-free-powerful-ide-microcontrollers:LPCXPRESSO)
- [ARM Mbed OS](https://os.mbed.com/) - Framework alternativo
