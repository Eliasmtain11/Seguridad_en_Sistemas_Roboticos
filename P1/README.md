# Práctica 1 · Cifrado César y rotura automática

Dos programas en C para Linux:

| Programa      | Qué hace                                                        |
|---------------|-----------------------------------------------------------------|
| `caesar`      | Cifra con César el texto de la entrada estándar.                |
| `breakcaesar` | Rompe por fuerza bruta un texto cifrado con César.              |

Solo usan la biblioteca estándar y llamadas POSIX, así que compilan en el laboratorio sin instalar nada.

```
caesar/
├── caesar.c
├── breakcaesar.c
├── caesar          ← ejecutable
├── breakcaesar     ← ejecutable
└── README.txt
```

---

## Compilar

```bash
mkdir build && cd build
gcc -g -c -Wall -Wshadow -Wvla ../caesar.c
gcc -g -o caesar caesar.o
gcc -g -c -Wall -Wshadow -Wvla ../breakcaesar.c
gcc -g -o breakcaesar breakcaesar.o -lm
cd ..
```

> `breakcaesar` necesita `-lm` porque usa `pow()` y `sqrt()`.

---

## 1. `caesar`

### Uso

```bash
./build/caesar key
```

`key` es un número entero (la clave de desplazamiento).

### Ejemplo

```bash
$ echo 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG' | ./build/caesar 23
QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD
```

El programa lee el texto en claro de la entrada estándar y escribe el cifrado en la salida estándar. Se puede escribir a mano y terminar con `Ctrl+D`, o usar una tubería o un fichero.

- Las minúsculas se pasan a mayúsculas antes de cifrar.
- Cada letra `A-Z` se desplaza `key` posiciones, módulo 26.
- Los caracteres que no son letras se copian sin cambios.

### Errores de uso

Si no se pasa exactamente un argumento, o no es numérico:

```
Usage: ./caesar key
```

y termina con código de salida distinto de 0.

---

## 2. `breakcaesar`

### Uso

```bash
./build/breakcaesar < fichero_cifrado
```

### Ejemplo rápido

```bash
$ echo 'QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD' | ./build/breakcaesar
23: 0.130800, 6, 2

$ cat key-23.txt
THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG
```

### Cómo funciona

```
 entrada estándar
        │
        ▼
┌──────────────────────┐     ┌─────────────┐
│ 1. Lectura por       │────▶│  temp_file  │  (texto en mayúsculas)
│    bloques de 8 KB   │     └─────────────┘
│    + conteo de       │
│    letras, digramas  │
│    y trigramas       │
└──────────┬───────────┘
           ▼
┌──────────────────────┐
│ 2. Prueba claves     │   distancia ↓   digramas ↑   trigramas ↑
│    1 a 25            │
└──────────┬───────────┘
           ▼
┌──────────────────────┐
│ 3. Elige candidatos  │──▶ salida estándar + key-N.txt
└──────────────────────┘
```

**1. Lectura.** Lee la entrada en bloques de 8192 bytes, así que funciona con ficheros grandes. En una sola pasada cuenta cada letra, cada digrama y cada trigrama. Los caracteres que no son letras se ignoran, y un espacio en blanco corta la secuencia para no contar digramas ni trigramas entre palabras distintas. A la vez guarda el texto en `temp_file` para generar después los descifrados.

**2. Prueba de claves.** Para cada clave del 1 al 25 calcula tres indicadores a partir de las cuentas ya hechas, sin volver a recorrer el texto:

| Indicador | Qué mide | Gana |
|-----------|----------|------|
| Distancia | Distancia euclídea entre las frecuencias del texto descifrado y las del inglés (tanto por uno) | La **menor** |
| Digramas  | Apariciones de los 28 digramas más comunes del inglés | La **mayor** |
| Trigramas | Apariciones de los 16 trigramas más comunes del inglés | La **mayor** |

En caso de empate en un indicador, gana la primera clave encontrada.

**3. Candidatos.**

| Situación | Candidatos | Orden |
|-----------|------------|-------|
| Los tres indicadores coinciden | 1 | — |
| Coinciden dos | 2 | Primero el que gana en dos |
| Ninguno coincide | 3 | Distancia, digramas, trigramas |

### Salida

Por cada candidato, una línea con el formato:

```
clave: distancia, digramas, trigramas
```

y un fichero `key-<clave>.txt` en el directorio de trabajo con el texto descifrado (los caracteres que no son letras se conservan).

> El fichero auxiliar `temp_file` queda en el directorio de trabajo y puede borrarse tras la ejecución.

---

## 3. Pruebas con *The Adventures of Sherlock Holmes*

Los ficheros de prueba están en el directorio padre (`P1/`):

| Fichero | Contenido |
|---------|-----------|
| `adventures_sherlock_holmes_onlylettersandblanks.txt` | Texto en claro |
| `adventures_sherlock_holmes_onlylettersandblanks_encrypted.txt` | Texto cifrado |

### Romper el texto cifrado

```bash
$ time ./build/breakcaesar < ../adventures_sherlock_holmes_onlylettersandblanks_encrypted.txt
15: 0.011568, 116432, 21464
```

Los tres indicadores coinciden en la clave **15**, así que solo hay un candidato y se genera `key-15.txt`.

### Comprobar que el descifrado es correcto

```bash
$ tr 'a-z' 'A-Z' < ../adventures_sherlock_holmes_onlylettersandblanks.txt | diff - key-15.txt && echo "OK: descifrado idéntico"
OK: descifrado idéntico
```

### Ciclo completo: cifrar y romper

Cifrar el libro con cualquier clave y comprobar que `breakcaesar` la encuentra:

```bash
$ ./build/caesar 7 < ../adventures_sherlock_holmes_onlylettersandblanks.txt | ./build/breakcaesar
7: ...
```

### ¿Cuándo falla? Textos cortos

Con pocos caracteres las estadísticas no son fiables y los indicadores dejan de coincidir:

```bash
$ head -c 100 ../adventures_sherlock_holmes_onlylettersandblanks_encrypted.txt | ./build/breakcaesar
$ head -c 500 ../adventures_sherlock_holmes_onlylettersandblanks_encrypted.txt | ./build/breakcaesar
$ head -c 2000 ../adventures_sherlock_holmes_onlylettersandblanks_encrypted.txt | ./build/breakcaesar
```

Cuanto más largo es el texto, antes coinciden los tres indicadores en la clave correcta.