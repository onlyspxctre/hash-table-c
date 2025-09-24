#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define FNV_PRIME_32 16777619
#define FNV_OFFSET_BASIS_32 2166136261

uint32_t hash_fnv(const char *data, const size_t bytes) {
    uint32_t hash = FNV_OFFSET_BASIS_32;

    for (size_t i = 0; i < bytes; ++i) {
        hash = (hash ^ data[i]) * FNV_PRIME_32;
    }

    return hash;
}

typedef struct HashTable HashTable;
typedef struct HashTableCell HashTableCell;

struct HashTable {
    HashTableCell* cells;
    size_t capacity;
};

struct HashTableCell {
    char* key;
    int value;
    HashTableCell* next;
};

bool CreateHashTableCell(HashTableCell** cell_ptr) {
    *cell_ptr = (HashTableCell*) malloc(sizeof(HashTableCell));

    if (!*cell_ptr) {
        fprintf(stderr, "Failed to allocate HashTableCell \n");
        return false;
    }

    return true;
}

bool CreateHashTableCellKey(HashTableCell* cell, size_t key_len) {
    cell->key = malloc(key_len + 1);

    if (!cell->key) {
        fprintf(stderr, "Failed to allocate HashTableCell.key \n");
        return false;
    }

    return true;
}

bool HashTableInsert(HashTable* table, const char* key, const size_t key_len) {
    uint32_t hash = hash_fnv(key, key_len);

    if (!table) {
        fprintf(stderr, "Failed to access uninitialized HashTable \n");
        return false;
    }

    if (!table->cells) {
        fprintf(stderr, "Failed to access uninitialized HashTable cells \n");
        return false;
    }

    HashTableCell* cell = &table->cells[hash % table->capacity];

    while (cell->key != NULL) {
        if (strcmp(cell->key, key) == 0) {
            break;
        }

        if (!cell->next) {
            if (!CreateHashTableCell(&cell->next)) {
                return false;
            }
            cell->next->value = 1;
        }

        cell = cell->next;
    }

    if (!cell->key) {
        if (!CreateHashTableCellKey(cell, key_len)) {
            return false;
        }
        strcpy(cell->key, key); // TODO: Make it safer
    }

    cell->value++;

    return true;
}

int HashTableGet(HashTable* table, const char* key, size_t key_len) {

    if (!table) {
        fprintf(stderr, "Failed to read from uninitialized HashTable \n");
        return -1;
    }

    if (!table->cells) {
        fprintf(stderr, "Failed to read from uninitialized HashTable cells \n");
        return -1;
    }

    uint32_t hash = hash_fnv(key, key_len);

    HashTableCell* cell = &table->cells[hash % table->capacity];
    while (cell) {
        if (!cell->key) return -1;
        if (strcmp(cell->key, key) == 0) return cell->value;
        cell = cell->next;
    }

    return -1;
}

void DisplayHashTable(HashTable* table) {
    if (!table) {
        fprintf(stderr, "Attempted to display uninitialized HashTable \n");
        return;
    }

    for (size_t i = 0; i < table->capacity; i++) {
        printf("%lu -> ", i);

        if (!table->cells[i].key) {
            printf("{}\n");
            continue;
        }

        HashTableCell* cell = &table->cells[i];
        do {
            printf("{%s: %d}", cell->key, cell->value);
            if (cell->next) printf(" -> ");
            cell = cell->next;
        } while(cell);

        printf("\n");
    }
}

int main(void) {
    HashTable table = {
        .cells = malloc(sizeof(*table.cells) * 1),
        .capacity = 1,
    };

    HashTableInsert(&table, "Hello", 5);
    HashTableInsert(&table, "Hello", 5);
    HashTableInsert(&table, "World", 5);
    HashTableInsert(&table, "World", 5);

    HashTableInsert(&table, "Fuck balls", 10);
    HashTableInsert(&table, "nigga", 5);

    HashTableInsert(&table, "the", 3);
    HashTableInsert(&table, "fitness", 7);
    HashTableInsert(&table, "gram", 4);

    DisplayHashTable(&table);
    printf("%d\n", HashTableGet(&table, "fitness", 7));
}
