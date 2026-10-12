Practica 1 - Cifrado Cesar y rotura automatica

Compilar
--------
Desde este directorio:

  make

Se generan los ejecutables caesar y breakcaesar. "make clean" los borra.

Ejecutar
--------
Cifrar (la clave es un numero entero, puede ser negativo):

  echo 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG' | ./caesar 23

Romper:

  ./breakcaesar < texto_cifrado.txt


