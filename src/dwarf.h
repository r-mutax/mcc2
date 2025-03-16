#pragma once

// DWARF directives


typedef struct Dwarf_dstr Dwarf_dstr;

// debug_string table
struct Dwarf_dstr {
    int id;
    char* str;
    Dwarf_dstr* next;
};
