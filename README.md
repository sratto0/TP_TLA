[![CI](https://github.com/sratto0/TP_TLA/actions/workflows/pipeline.yaml/badge.svg?branch=development)](https://github.com/sratto0/TP_TLA/actions/workflows/pipeline.yaml)
[![Stage III](https://img.shields.io/badge/Proyecto-Stage%20III-2563eb.svg)](https://github.com/sratto0/TP_TLA)

# SchedLang

SchedLang es un lenguaje especifico de dominio para describir y validar
cronogramas academicos. El compilador permite declarar docentes, aulas,
materias, comisiones y restricciones horarias, comprobar la consistencia del
cronograma y generar una visualizacion en formato HTML.

El proyecto fue desarrollado en C a partir de
[Flex-Bison-Compiler](https://github.com/agustin-golmar/Flex-Bison-Compiler),
utilizando Flex para el analisis lexico y Bison para el analisis sintactico.

## Funcionalidades

- Declaracion de docentes, aulas, materias y comisiones.
- Disponibilidad y franjas bloqueadas de docentes.
- Asignacion de comisiones a docentes, aulas, dias y horarios.
- Validacion de referencias y nombres duplicados.
- Validacion de capacidades y cantidades de estudiantes.
- Deteccion de superposiciones de docentes, aulas y comisiones.
- Generacion de `schedule.html` mediante `print schedule;`.

## Ejemplo

```text
teacher "Perez";

room "A101" capacity 40;

course "Automatas" students 35;
section "Automatas-1" of "Automatas";

available "Perez" monday 08:00-12:00;

assign "Automatas-1" to "Perez" in "A101" at monday 08:00-10:00;

print schedule;
```

La sentencia `print schedule;` genera el archivo `schedule.html` en el
directorio actual. Si no se incluye esa sentencia, el programa puede ser
validado correctamente sin producir un archivo de salida.

## Requisitos

- Docker 28 o posterior.
- Docker Compose.

El entorno de Docker instala las versiones necesarias de GCC, CMake, Flex,
Bison y Make.

## Uso

Iniciar el entorno de desarrollo:

```bash
docker compose run --rm compiler
```

Construir el compilador:

```bash
src/main/bash/build.sh
```

Compilar un programa:

```bash
src/main/bash/run.sh <ruta-del-programa>
```

Ejecutar todos los casos de prueba:

```bash
src/main/bash/test.sh
```

La suite incluye casos de aceptacion, rechazo semantico y sintactico,
verificacion de fragmentos del HTML generado y comprobacion de que no se
produzca una salida cuando falta `print schedule;`.

## Configuracion

El comportamiento del compilador puede ajustarse mediante las siguientes
variables de entorno:

| Variable | Valor predeterminado | Descripcion |
| :--- | :---: | :--- |
| `ENVIRONMENT` | `Local` | Nombre del entorno activo. |
| `LOG_IGNORED_LEXEMES` | `true` | Indica si se registran los lexemas ignorados. |
| `LOGGING_LEVEL` | `ALL` | Nivel minimo de logging: `ALL`, `DEBUGGING`, `INFORMATION`, `WARNING`, `ERROR` o `CRITICAL`. |

## Alcance

La version actual se concentra en la validacion de un cronograma definido por
archivo. No realiza optimizacion automatica de horarios. Los schedules
nombrados, los tipos de aula, la carga horaria de las materias y el limite
diario de asignaciones por docente quedan planteados como futuras extensiones.

## Integrantes

- Sofia Ratto
- Martina Nudel
- Mateo Lopez Badias
