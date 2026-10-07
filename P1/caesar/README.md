# Caesar

Programa en C que cifra un texto usando el cifrado César con una clave numérica.

## Compilar

```bash
mkdir build && cd build
gcc -g -c -Wall -Wshadow -Wvla -g ../caesar.c
gcc -g -o caesar caesar.o
```

## Ejecutar

```bash
./build/caesar key
```

Donde `key` es un número entero (la clave de desplazamiento). Por ejemplo:

```bash
./build/caesar 22
```

El programa pedirá un texto (`plaintext`) y mostrará el resultado cifrado (`ciphertext`).

### Errores de uso

Si no se pasa exactamente un argumento, o el argumento no es numérico, el programa muestra:

```
Usage: ./caesar key
```

y termina con código de salida distinto de 0.
