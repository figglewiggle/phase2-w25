#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/parser.h"
#include "../../include/lexer.h"
#include "../../include/tokens.h"
#include "../../include/semantic.h"

// Initialize a new symbol table
// Creates an empty symbol table structure with scope level set to 0
SymbolTable* init_symbol_table(){

    SymbolTable *table = malloc(sizeof(SymbolTable));
    table->current_scope=0;
    return table;

}

// Add a symbol to the table
// Inserts a new variable with given name, type, and line number into the current scope
Symbol* add_symbol(SymbolTable* table, const char* name, int type, int line){
    
    // Does that symbol already exist?
    Symbol *exists = lookup_symbol(table, name);
    if (exists){
        return; //REPLACE WITH APPROPRIATE ERROR
    }

    // If not, let's add it!
    Symbol *new = malloc(sizeof(Symbol));
    if (new){
        strcpy(new->name, name);
        new->type=type;
        new->line_declared=line;
    }

    // Add to start of table if empty
    if (!(table->head)){
        table->head=new;
        return new;
    }

    // Or end of table if not empty
    Symbol *curr = table->head;
    while (curr->next){
        curr = curr->next;
    }
    curr->next=new;

    return new;
}

// Look up a symbol in the table
// Searches for a variable by name across all accessible scopes
// Returns the symbol if found, NULL otherwise
Symbol* lookup_symbol(SymbolTable* table, const char* name){
    
    // Is the table empty?
    if (!(table->head)){
        return NULL;
    }

    // If not, search the table
    Symbol *curr = table->head;
    while (curr->next){
        if (curr->name == name){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// Enter a new scope level
// Increments the current scope level when entering a block (e.g., if, while)
void enter_scope(SymbolTable* table){
    table->current_scope = (table->current_scope + 1);
}

// Exit the current scope
// Decrements the current scope level when leaving a block
// Optionally removes symbols that are no longer in scope
void exit_scope(SymbolTable* table){
    remove_symbols_in_current_scope(table);
    table->current_scope = (table->current_scope) - 1;
}

// Remove symbols from the current scope
// Cleans up symbols that are no longer accessible after leaving a scope
void remove_symbols_in_current_scope(SymbolTable* table){
    
    Symbol *prev;
    Symbol *curr;
    Symbol *next;

    // Are there any symbols?
    if (!(table->head)){
        return;
    }

    // Special case: table head is being removed
    curr = table->head;
    while (curr->scope_level == table->current_scope){
        next = curr->next;
        free(curr);
        curr = next;
        table->head = curr;
    }

    // Are there any symbols left?
    if (!(table->head)){
        return;
    }

    // Regular case
    curr = table->head; // Known to be in a broader scope
    while (curr->next){ // Only triggers if |table| > 1

        // Remove symbol of current scope and adjust links
        if (curr->scope_level == table->current_scope){
            next = curr->next;
            free(curr);
            curr = next;
            prev->next = curr;
        }

        // Shift further down the list
        // Will always happen during the first loop
        else {
            prev = curr;
            curr = curr->next;
        }
    }

    // Special case: the last symbol is being removed
    if (curr->scope_level == table->current_scope){
        prev->next=NULL;
        free(curr);
    }
}

// Free the symbol table memory
// Releases all allocated memory when the symbol table is no longer needed
void free_symbol_table(SymbolTable* table){

    Symbol *curr = table->head;
    Symbol *next;

    // Clear the table's symbols
    while (curr){
        next = curr->next;
        free(curr);
        curr = next;
    }

    // Clear the table itself
    free(table);
}