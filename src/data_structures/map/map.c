#include "map.h"
// #define __DEBUG_MAP__

Map createMap (int minSize) {
    int size = __nearestPrime(minSize > 5 ? minSize : 5);
    
    Map res = (Map)malloc(sizeof(_Map));
    res->__size = size;
    res->__fillLimit = size * MAP_FILL_LIM;
    res->__pairsCount = 0;
    res->__pairs = (__MapPair*)malloc(sizeof(__MapPair) * size);

    for (int i = 0; i < size; i++) {
        res->__pairs[i].key = (char*)malloc(sizeof(char) * MAP_KEY_MAX_LEN);
        strcpy(res->__pairs[i].key, "");
        
        res->__pairs[i].value = NULL;
    }

    return res;
}

void deleteMap (Map this, void (*destructor)(void *object)) {
    for (int i = 0; i < this->__size; i++) {
        free(this->__pairs[i].key);
        if (this->__pairs[i].value != NULL) destructor(this->__pairs[i].value); 
    }

    free(this);
}

MAP_T __getMap (Map this, char *key) {
    int keyHash = __hashMap(this->__size, key);
    int i = 0;

    #ifdef __DEBUG_MAP__
        printf("Key: %s, ", key);
    #endif

    while (strcmp(this->__pairs[keyHash].key, key) != 0 && i < this->__size) {
        // ключа в таблице нет
        // проверка идет по значению, ключ может быть пустым, но значение могло быть занято раньше
        if (this->__pairs[keyHash].value == NULL) {
            #ifdef __DEBUG_MAP__
                printf("Rehash: %d\n", i);
            #endif

            return NULL;
        } 

        // коллизия 
        // рехеширование
        keyHash = (keyHash + 1) % this->__size;
        i++;
    }

    #ifdef __DEBUG_MAP__
        printf("Rehash: %d\n", i);
    #endif

    return i < this->__size ? this->__pairs[keyHash].value : NULL;    
}

bool __setMap (Map this, char *key, MAP_T value) {
    int keyHash = __hashMap(this->__size, key);
    int i = 0;

    #ifdef __DEBUG_MAP__
        printf("Key: %s, ", key);
    #endif

    while (strcmp(this->__pairs[keyHash].key, key) != 0 && i < this->__size) {
        // ключа в таблице еще нет
        // заменять значение с пустым ключем можно
        
        if (strcmp(this->__pairs[keyHash].key, "") == 0) {
            #ifdef __DEBUG_MAP__
                printf("Rehash: %d\n", i);
            #endif

            strcpy(this->__pairs[keyHash].key, key);
            this->__pairs[keyHash].value = value;

            // расширение вектора пар по необходимости
            this->__pairsCount++;
            __checkMemoryMap(this);
            return true;
        }

        // коллизия 
        // рехеширование
        keyHash = (keyHash + 1) % this->__size;
        i++;
    }

    #ifdef __DEBUG_MAP__
        printf("Rehash: %d\n", i);
    #endif

    // место для записи не найдено
    if (i >= this->__size) return false;

    // ключ уже есть, перезапись значения
    this->__pairs[keyHash].value = value;
    return true;
}

bool __removeMap (Map this, char *key) {
    int keyHash = __hashMap(this->__size, key);
    int i = 0;

    #ifdef __DEBUG_MAP__
        printf("Key: %s, ", key);
    #endif

    while (strcmp(this->__pairs[keyHash].key, key) != 0 && i < this->__size) {
        // ключа в таблице нет
        if (strcmp(this->__pairs[keyHash].key, "") == 0) {
            #ifdef __DEBUG_MAP__
                printf("Rehash: %d\n", i);
            #endif

            return false;
        } 

        // коллизия 
        // рехеширование
        keyHash = (keyHash + 1) % this->__size;
        i++;
    }

    #ifdef __DEBUG_MAP__
        printf("Rehash: %d\n", i);
    #endif

    // ключ для удаления не найден
    if (i >= this->__size) return false;

    // обнуляем только ключ, значение обнулять нельзя, т.к. по нему идет проверка при поиске
    strcpy(this->__pairs[keyHash].key, "");
    return true;
}

bool __hasMap (Map this, char *key) {
    int keyHash = __hashMap(this->__size, key);
    int i = 0;

    #ifdef __DEBUG_MAP__
        printf("Key: %s, ", key);
    #endif

    while (strcmp(this->__pairs[keyHash].key, key) != 0 && i < this->__size) {
        // ключа в таблице нет
        if (this->__pairs[keyHash].value == NULL) {
            #ifdef __DEBUG_MAP__
                printf("Rehash: %d\n", i);
            #endif

            return false;
        } 

        // коллизия 
        // рехеширование
        keyHash = (keyHash + 1) % this->__size;
        i++;
    }

    #ifdef __DEBUG_MAP__
        printf("Rehash: %d\n", i);
    #endif

    return i < this->__size;
}

void __checkMemoryMap (Map this) {
    if (this->__pairsCount < this->__fillLimit) return;

    #ifdef __DEBUG_MAP__
        printf("Extand\n");
    #endif

    // начиная с 3 простые числа не могут идти подряд
    int newSize = __nearestPrime(this->__size + 2); 

    // выделяем новый массив для пар
    __MapPair *newPairs = (__MapPair*)malloc(sizeof(__MapPair) * newSize);

    for (int i = 0; i < newSize; i++) {
        newPairs[i].key = (char*)malloc(sizeof(char) * MAP_KEY_MAX_LEN);
        strcpy(newPairs[i].key, "");

        newPairs[i].value = NULL;
    }

    // переносим старые пары в новый массив
    for (int i = 0; i < this->__size; i++) {
        int keyHash = __hashMap(newSize, this->__pairs[i].key);

        while (strcmp(newPairs[keyHash].key, "") != 0) {
            keyHash = (keyHash + 1) % newSize;
        }
        
        strcpy(newPairs[keyHash].key, this->__pairs[i].key);
        newPairs[keyHash].value = this->__pairs[i].value;
    }

    free(this->__pairs);
    this->__pairs = newPairs;
    this->__size = newSize;
    this->__fillLimit = newSize * MAP_FILL_LIM;
}

int __nearestPrime (int number) {
    while (true) {
        bool hasDels = false;
        for (int i = 2; i * i <= number; i++) {
            if (number % i == 0) {
                hasDels = true;
                break;
            }
        }
        
        if (!hasDels) return number;
        number++;
    }
}

int __hashMap (int mapSize, char *key) {
    uint64_t codeSum = 0;

    int i = 0;
    while (key[i] != '\0') {
        codeSum = (codeSum << 5) + (uint8_t)key[i];
        i++;
    }
    
    return codeSum % mapSize;
}

void mapDestructorPlug (void *object) {}