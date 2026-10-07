En esta práctica vas a programar, para Linux, un ataque criptoanalítico automático a un texto cifrado con un código simple de sustitución monoalfabético, tipo César.

Cifrado


El código César consiste en un desplazamento (módulo el tamaño del alfabeto). Vamos a suponer que no hay distinción entre mayúsculas y minúsculas, por lo que todas las letras minúsculas se pasarán a mayúsculas antes de su transformación.

Por ejemplo, si la clave es 23,
A -> X
B -> Y
C -> Z
...
Texto plano:  THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG
Texto cifrado: QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD

Observa que  se están sustituyendo sólo las letras, los espacios no se cambian. En esta práctica vamos a trabajar con textos en inglés para no tener problemas con la codificación de texto.

Primero, debes escribir un programa caesar que reciba como parámetro la clave y cifre un texto leído de la entrada estándar, para poder probar bien tu programa. 

Por ejemplo, este mensaje cifrado con césar:

$> echo 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG'| ./caesar 23
QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD
$>
Rotura


Después, debes escribir un programa llamado breakcaesar que rompa el cifrado. En un ataque de este tipo, hay siempre dos partes. 

A) El intento de generar una solución. Vamos a realizar un ataque de fuerza bruta, es decir, el ataque consistirá en probar todas las posibles claves (25), empezando desde la clave 1.

B) Medir cómo de buena es la solución.  Para esto, vamos a usar tres indicadores:

    La frecuencia de cada letra en el texto descifrado. La solución que tenga las frecuencias de aparición de cada letra más cercana a la del inglés será la candidata. Para esto, se debe calcular la distancia euclídea de cada solución. La solución con menos distancia será la candidata.
    La cantidad de digramas  (pares de letras) comunes en el inglés. La solución que tenga más será la candidata.
    La cantidad de trigramas (tríos de letras) comunes en el inglés.  La solución que tenga más será la candidata.

Es importante que lo pruebes con texto de diferentes longitudes para ver cuándo y cómo falla. El programa tiene que funcionar con ficheros grandes. El programa debe leer el texto cifrado de su entrada estándar.

El programa breakcaesar debe ignorar todos los caracteres que no sean letras. Las letras minúsculas las pasará a mayúsculas, como hemos dicho antes.

Dado un texto cifrado en la entrada estándar, el programa breakcaesar tiene que escribir para cada candidato a solución una línea con:

clave: distancia, número de digramas, número de trigramas

También debe dejar en un fichero, en el directorio de trabajo, cada solución (el texto descifrado). El fichero se tiene que llamar key-número.txt., siendo el número la clave correspondiente.

El número de candidatos a la solución que debe imprimir el programa depende de los tres indicadores explicados anteriormente (distancia, digramas, trigramas).

    Si los tres indicadores señalan la misma solución, sólo habrá un candidato.
    Si dos indicadores señalan la misma solución, habrá dos candidatos (primero el que gana en dos indicadores)
     En otro caso, se imprimirán los tres candidatos (los que ganan en cada indicador).

Para cada indicador, en caso de empate, el candidato para ese indicador será la primera solución encontrada.

Por ejemplo, si para un texto tenemos dos candidatos, el candidato con clave 21 que gana en digramas y trigramas, y el candidato con clave 12 que gana en distancia, el programa breakcaesar debería imprimir por su salida:

21: 0.3229, 134, 33

12: 0.1234, 123, 12

En este caso, se habrán generado únicamente dos ficheros:  key-21.txt y key-12.txt (cada uno con el texto descifrado usando la clave correspondiente).

Un ejemplo de ejecución del programa breakcaesar:

$> echo 'QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD' | ./breakcaesar
23: 0.13080, 6, 2
$> cat  key-23.txt
THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG
$>

En este caso, sólo hay un candidato (clave 23), que gana en los tres indicadores. La distancia de frecuencias se puede hacer en tanto por cien o tanto por uno, como se prefiera.

Otro ejemplo con empate:

$ cat empate-cipher.txt
QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD
FT CGUOW NDAIZ RAJ VGYBE AHQD FTQ XMLK PAS
HHHHHHHHHHHHHHHHHHHHHHHHHHHHH
$ ../breakcaesar < empate-cipher.txt
3: [0.238327, 1, 0]
13: [0.333946, 8, 0]
23: [0.345902, 7, 2]
$

Para tener en cuenta cuánto tarda la rotura en el portátil del profesor:

$ time ./breakcaesar < adventures_sherlock_holmes_onlylettersandblanks_encrypted.txt
15: [0.011568, 116432, 21464]

real    0m1.134s
user    0m1.131s
sys    0m0.004s
$
Implementación


Puedes suponer que la máquina tiene suficiente memoria para albergar el texto cifrado a cifrar o romper, y todas las posibles soluciones en el caso de la rotura. 

Puedes realizar los programas en cualquiera de estos lenguajes:

    C
    C++
    Java
    Python
    Go

Deben ejecutar en los terminales del laboratorio sin tener que instalar nada extra.

Entrega


Hay que entregar un fichero llamado caesar.tgz con un directorio caesar que incluya:

     Todo el código fuente de los programas caesar y breakcaesar.
     Un fichero README.txt con instrucciones para compilar y ejecutar el programa. El programa debe compilar y ejecutar en el laboratorio sin tener que instalar nada extra.
     Un ejecutable para cada programa con el nombre indicado (caesar y breakcaesar). Si el binario no es un ejecutable (p. ej. Java), se debe entregar el binario y un script de shell que ejecute el programa. Dicho script se tiene que llamar como indica el enunciado en los ejemplos (breakcaesar).

En cualquier caso,  para probarlo, el profesor sólo deberá escribir esto en un terminal del laboratorio:

$> tar xzf caesar.tgz
$> cd caesar
$> chmod +x breakcaesar
$> ./breakcaesar < ciphertext

(lo mismo para el otro programa)

Datos


Las frecuencias y digramas/trigramas que debes usar son los siguientes (la primera columna está ordenada por letra, la segunda por frecuencia):

Relative frequencies of letters
By letter    By frequency
Letter    Frequency    Letter    Frequency
a    0.08167    e    0.12702
b    0.01492    t    0.09056
c    0.02782    a    0.08167
d    0.04253    o    0.07507
e    0.12702    i    0.06966
f    0.02228    n    0.06749
g    0.02015    s    0.06327
h    0.06094    h    0.06094
i    0.06966    r    0.05987
j    0.00153    d    0.04253
k    0.00772    l    0.04025
l    0.04025    c    0.02782
m    0.02406    u    0.02758
n    0.06749    m    0.02406
o    0.07507    w    0.02360
p    0.01929    f    0.02228
q    0.00095    g    0.02015
r    0.05987    y    0.01974
s    0.06327    p    0.01929
t    0.09056    b    0.01492
u    0.02758    v    0.00978
v    0.00978    k    0.00772
w    0.02360    j    0.00153
x    0.00150    x    0.00150
y    0.01974    q    0.00095
z    0.00074    z    0.00074

Most common bigrams (in order)

th, he, in, en, nt, re, er, an, ti, es, on, at, se, nd, or, ar, al, te, co, de, to, ra, et, ed, it, sa, em, ro.

Most common trigrams (in order)

the, and, tha, ent, ing, ion, tio, for, nde, has, nce, edt, tis, oft, sth, men


Referencias


[1] https://es.wikipedia.org/wiki/Distancia_euclidiana