Felipe Luengo 202473509-8
Catalina Fernandez 202473645-0

formato del .map, lo dejamos con esas separaciones y sin lineas vacias para parsearlo de manera mas comoda:
INICIO DE ARCHIVO
ENEMIGOS:
enemigo|vida|ataque|precision|probabilidad
EVENTOS:
evento|probabilidad|contar evento|mostrar A|descripcion consecuencia de A|efecto A|mostrar B|descripcion consecuencia B|efecto B
HABITACIONES: 
id|nombre estacion|tipo habitacion|descripcion
ARCOS:
inicio|llegada
MEJORAS DE COMBATE:
vida|precision|ataque|recuperacion

ejemplo de compilacion (incluye el archivo principal con el main y funciones, y el tda arbol con sus operadores asociados): 
g++ -o juego TAREA2.cpp arbol.cpp

justificacion requisito especial: existe una estacion (p.egaña) que te lleva a la raiz. Como dice la descripcion eres arrastrado por unos seres desconocidos. 

