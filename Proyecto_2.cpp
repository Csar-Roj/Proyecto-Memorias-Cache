#include <iostream>
#include "rapidcsv.h"

using namespace std;

const int csize = 12; //Tamaño del cache

int replacement_integer;
vector<int> load_memory(char* filename);
int retrieve_block(int direction, int _cache[][2]);
int insert_block(int direction, int _cache[][2]);
int retrieve_block_asociative(int direction, int _cache[][2], int subsets);
int insert_block_asociative(int direction, int _cache[][2], int blocks_per_set, int cache_position);
void print_cache(int _cache[][2]);

int main()
{
    //Abrir el dataset como un documento usando nuestra libreria
    rapidcsv::Document doc("Eartquakes-1990-2023-pruned.csv");
    //Crear Vectores para guardar los datos en memoria
    vector<double> tsunami = doc.GetColumn<double>("tsunami");
    vector<double> significance = doc.GetColumn<double>("significance");
    vector<float> magnitude = doc.GetColumn<float>("magnitudo");
    vector<float> longitude = doc.GetColumn<float>("longitude");
    vector<float> latitude = doc.GetColumn<float>("latitude");
    vector<float> depth = doc.GetColumn<float>("depth");
    

    //Crear Cache, primer valor para la direccion, segundo para el orden de guardado
    int cache[csize][2];
    //Crear vector de memoria, donde guardamos las direcciones
    vector<int> memory;
    //asignar valor al entero que usamos para encontrar la posicion en memoria mas antigua
    replacement_integer = 0;

    //Inicializar cache
    for(int i = 0; i < csize; i++)
    {
        cache[i][0] = 0;
        cache[i][1] = 0;
    }

    //Inicializar memoria
    char name[30] = "direcciones_final.txt";
    memory = load_memory(name);

    //Por Correspondencia Directa:
    int fails = 0;
    for(int i = 0; i < memory.size(); i++)
    {
        fails += retrieve_block(memory[i], cache);
    }
    printf("El numero de fallos obtenidos por correspondencia directa es %d\n", fails);
    //print_cache(cache);
    //Reiniciar Cache
    for(int i = 0; i < 4; i++)
    {
        cache[i][0] = 0;
        cache[i][1] = 0;
    }

    //Por Asociativa por conjuntos (2 Conjuntos):
    fails = 0;
    for(int i = 0; i < memory.size(); i++)
    {
        fails += retrieve_block_asociative(memory[i], cache, 2);
    }
    printf("El numero de fallos obtenidos por asociativa por conjuntos es %d\n", fails);
    //print_cache(cache);
    //Reiniciar Cache
    for(int i = 0; i < 4; i++)
    {
        cache[i][0] = 0;
        cache[i][1] = 0;
    }

    //Por Completamente asociativa
    fails = 0;
    for(int i = 0; i < memory.size(); i++)
    {
        fails += retrieve_block_asociative(memory[i], cache, 4);
    }
    printf("El numero de fallos obtenidos por asociativa completa es %d\n", fails);
    print_cache(cache);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Guardamos los contenidos de nuestro cache en un archivo de salida
    FILE *fp; //Crear un puntero de archivo
    fp = fopen("salida.txt", "w+"); //Abrir el archivo

    if(fp == NULL)
    {
        printf("FILE DOES NOT EXIST\n");
    }
    else
    {
        //Escribir contenidos de las direcciones guardadas en el cache en un archivo de salida
        for(int i = 0; i < csize; i++)
        {
            double numero = tsunami.at(cache[i][0]);
            float flotante = magnitude.at(cache[i][0]);
            fprintf(fp,"\nTsunami:\n%f\n", numero);
            numero = significance.at(cache[i][0]);
            fprintf(fp,"Significance:\n%f\n", numero);
            fprintf(fp,"Magnitude:\n%f\n", flotante);
            flotante = longitude.at(cache[i][0]);
            fprintf(fp,"Longitude:\n%f\n", flotante);
            flotante = latitude.at(cache[i][0]);
            fprintf(fp,"Latitude:\n%f\n", flotante);
            flotante = depth.at(cache[i][0]);
            fprintf(fp,"Depth:\n%f\n\n", flotante);

            printf("\nDirection %d: %d", i, cache[i][0]);
            printf("\nEarthquake Depth %d: %f\n", i, flotante);
        }
    }
    fclose(fp);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    return 0;
}

vector<int> load_memory(char* filename)
{
    FILE *fp; //Crear un puntero de archivo
    fp = fopen("direcciones.txt", "rb"); //Abrir el archivo
    vector<int> _memory; //Crear vector de memoria
    //Si el archivo no existe, retornar sin cambios
    if (fp == NULL)
    {
        fclose(fp);
        return _memory;
    }
    //Leer la primera linea del archivo, que deberia ser el tamaño
    int _size;
    fscanf(fp, "%d", &_size);
    _memory.resize(_size);
    //Leer las siguientes lineas y guardarlas en la posicion del vector
    for(int i = 0; i < _size; i++)
    {
        fscanf(fp, "%d", &_memory[i]);
    }
    //Cerrar el archivo
    fclose(fp);
    return _memory;
}

int retrieve_block(int direction, int _cache[][2])
{  
    //Calcular la posicion donde la direccion pertenece
    int cache_position = direction % csize;
    //Si la direccion ya existe, retornar un exito
    if(_cache[cache_position][0] == direction)
    {
        return 0;
    }
    //Si no existe, reemplazarla y retornar un fallo
    insert_block(direction, _cache);
    return 1;
}

int insert_block(int direction, int _cache[][2])
{
    //Calculamos la posicion de memoria donde se pondra el bloque
    int cache_position = direction % csize;
    
    //Insertamos el bloque en el cache:
    _cache[cache_position][0] = direction;
    _cache[cache_position][1] = replacement_integer;
    replacement_integer++;
    return 0;
}

int retrieve_block_asociative(int direction, int _cache[][2], int subsets)
{  
    //Encontrar a que conjunto pertenece la direccion
    int target_subset = direction % subsets;
    int blocks_per_set = csize / subsets;
    int cache_position = target_subset * blocks_per_set;
    //Si la direccion ya existe, retornar un exito
    int found = 0;
    for(int i = 0; i < blocks_per_set; i++)
    {
        if(_cache[cache_position+i][0] == direction)
        {
            found = 1;
        }
    }
    if(found == 1)
    {
        return 0;
    }
    //Si no existe, reemplazarla y retornar un fallo
    insert_block_asociative(direction, _cache, blocks_per_set, cache_position);
    return 1;
}

int insert_block_asociative(int direction, int _cache[][2], int blocks_per_set, int cache_position)
{
    //Intentar colocar en una posicion vacia
    int i = 0;
    int found = 0;
    while(found == 0)
    {
        if(_cache[cache_position+i] == 0)
        {
            _cache[cache_position+i][0] = direction;
            _cache[cache_position+i][1] = replacement_integer;
            replacement_integer++;
            found = 1;
        }
        if(i == blocks_per_set-1)
        {
            found = -1;
        }
        i++;
    }
    if(found == 1)
    {
        return 0;
    }
    //Si no es posible, reemplazar el mas antiguo
    //Hallar el mas antiguo
    int oldest = 2147483647; //2147483647 es el mayor numero que se puede guardar en un entero
    int target;
    for(int j = 0; j < blocks_per_set; j++)
    {
        if(_cache[cache_position+j][1] < oldest)
        {
            target = cache_position+j;
            oldest = _cache[cache_position+j][1];
        }
    }
    //Reemplazar
    _cache[target][0] = direction;
    _cache[target][1] = replacement_integer;
    replacement_integer++;
    return 1;
    
}

void print_cache(int _cache[][2])
{
    for(int i = 0; i < csize; i++)
    {
        printf("%d : %d\n", i, _cache[i][0]);
    }
}