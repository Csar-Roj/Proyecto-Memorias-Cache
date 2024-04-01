#include <iostream>
#include <conio.h>

using namespace std;

int replacement_integer;
int load_memory(int _memory[], char* filename);
int insert_block(int direction, int _cache[][2], int cache_size);
int retrieve_block(int direction, int _cache[][2], int cache_size);
int insert_block_asociative(int direction, int _cache[][2], int blocks_per_set, int cache_position);
int retrieve_block_asociative(int direction, int _cache[][2], int cache_size, int subsets);
void print_cache(int _cache[][2], int cache_size);

int main()
{
    int cache[4][2];
    int memory[40];
    replacement_integer = 0;

    //Inicializar cache
    for(int i = 0; i < 4; i++)
    {
        cache[i][0] = 0;
        cache[i][1] = 0;
    }
    char name[30] = "direcciones.txt";
    int n_directions = load_memory(memory, name);

    //Por Correspondencia Directa:
    int fails = 0;
    for(int i = 0; i < n_directions; i++)
    {
        fails += retrieve_block(memory[i], cache, 4);
    }
    printf("El numero de fallos obtenidos por correspondencia directa es %d\n", fails);
    print_cache(cache,4);
    //Reiniciar Cache
    for(int i = 0; i < 4; i++)
    {
        cache[i][0] = 0;
        cache[i][1] = 0;
    }

    //Por Asociativa por conjuntos (2 Conjuntos):
    fails = 0;
    for(int i = 0; i < n_directions; i++)
    {
        fails += retrieve_block_asociative(memory[i], cache, 4, 2);
    }
    printf("El numero de fallos obtenidos por asociativa por conjuntos es %d\n", fails);
    print_cache(cache,4);
    //Reiniciar Cache
    for(int i = 0; i < 4; i++)
    {
        cache[i][0] = 0;
        cache[i][1] = 0;
    }

    //Por Completamente asociativa
    fails = 0;
    for(int i = 0; i < n_directions; i++)
    {
        fails += retrieve_block_asociative(memory[i], cache, 4, 4);
    }
    printf("El numero de fallos obtenidos por asociativa completa es %d\n", fails);
    print_cache(cache,4);
    getch();
    return 0;
}

int load_memory(int _memory[], char* filename)
{
    FILE *fp; //Crear un puntero de archivo
    fp = fopen("direcciones.txt", "rb"); //Abrir el archivo
    //Si el archivo no existe, retornar 0 para saber que el arreglo no contiene nada
    if (fp == NULL)
    {
        fclose(fp);
        return 0;
    }
    //Leer la primera linea del archivo, que deberia ser el tamaño
    int _size;
    //fread(&_size, sizeof(int), 1, fp);
    fscanf(fp, "%d", &_size);
    //Leer las siguientes lineas y guardarlas en la posicion del arreglo
    for(int i = 0; i < _size; i++)
    {
        //Leer la direccion
        //fread(&_memory[i], sizeof(int), 1, fp);
        fscanf(fp, "%d", &_memory[i]);
    }
    //Cerrar el archivo
    fclose(fp);
    return _size;
}

int insert_block(int direction, int _cache[][2], int cache_size)
{
    //Calculamos la posicion de memoria donde se pondra el bloque
    int cache_position = direction % cache_size;
    
    //Insertamos el bloque en el cache:
    _cache[cache_position][0] = direction;
    _cache[cache_position][1] = replacement_integer;
    replacement_integer++;
    return 0;
}

int retrieve_block(int direction, int _cache[][2], int cache_size)
{  
    //Calcular la posicion donde la direccion pertenece
    int cache_position = direction % cache_size;
    //Si la direccion ya existe, retornar un exito
    if(_cache[cache_position][0] == direction)
    {
        return 0;
    }
    //Si no existe, reemplazarla y retornar un fallo
    insert_block(direction, _cache, cache_size);
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

int retrieve_block_asociative(int direction, int _cache[][2], int cache_size, int subsets)
{  
    //Encontrar a que conjunto pertenece la direccion
    int target_subset = direction % subsets;
    int blocks_per_set = cache_size / subsets;
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

void print_cache(int _cache[][2], int cache_size)
{
    for(int i = 0; i < cache_size; i++)
    {
        printf("%d : %d\n", i, _cache[i][0]);
    }
}