/*

Хэш-таблица с открытой адресацией и линейным рехешированием.

Функциональная спецификация:

createMap: int -> Map - создать таблицу
deleteMap: Map -> void - удалить таблицу
getMap: ValueType x Map x Key -> ValueType - поиск по ключу
setMap: Map x Key x ValueType -> bool - добавить пару/обновить значение
removeMap: Map x Key -> bool - убрать пару
hasMap: Map x Key -> bool - есть ли ключ в таблице

*/

#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define MAP_KEY_MAX_LEN 30
#define MAP_FILL_LIM 3 / 4

typedef void* MAP_T;

typedef struct {
    char *key;
    MAP_T value;
} __MapPair;

typedef struct {
    __MapPair *__pairs;
    int __size;
    int __fillLimit;
    int __pairsCount;
} _Map, *Map;

Map createMap (int size);
void deleteMap (Map this, void (*destructor)(void *object));
MAP_T __getMap (Map this, char *key);
bool __setMap (Map this, char *key, MAP_T value);
bool __removeMap (Map this, char *key);
bool __hasMap (Map this, char *key);

void __checkMemoryMap (Map this);
int __hashMap (int mapSize, char *key);
int __nearestPrime (int number);

#define getMap(ValueType, map, key) (ValueType)__getMap((map), (key))
#define setMap(map, key, value) __setMap((map), (key), (MAP_T)(value))
#define removeMap(map, key) __removeMap((map), (key))
#define hasMap(map, key) __hasMap((map), (key))

void mapDestructorPlug (void *object);

