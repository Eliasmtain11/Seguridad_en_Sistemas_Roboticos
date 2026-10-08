# Práctica 1 · Cifrado César y rotura automática

## Compilar

```bash
mkdir build && cd build
gcc -g -c -Wall -Wshadow -Wvla ../caesar/caesar.c
gcc -g -o caesar caesar.o
gcc -g -c -Wall -Wshadow -Wvla ../caesar/breakcaesar.c
gcc -g -o breakcaesar breakcaesar.o -lm
cd ..
```

## Ejecutar

Cifrar (la clave es un número entero):

```bash
./build/caesar 23 < texto_en_claro.txt > texto_cifrado.txt
```

Romper:

```bash
./build/breakcaesar < texto_cifrado.txt
```

Se escribe una línea por candidato (`clave: distancia, digramas, trigramas`) y se crea un fichero `key-<clave>.txt` por candidato en el directorio actual.

## Ejemplo

```bash
./build/breakcaesar < adventures_sherlock_holmes_onlylettersandblanks_encrypted.txt
```
