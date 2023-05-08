// Assignment 2 21T2 COMP1511: Pokedex
// pokedex.c
//
// This program was written by Gabriel Esquivel, (z5358503)
// in August 2021
//
// Version 1.0.0: Release

/*
    The following program creates a simple pokedex that is capable of recording
    details about the pokemon the user has caught or seen. The details it records
    include name, ID, height, weight and types. The pokedex is structured as a
    linked list, with the details of each pokemon recorded in "dexnodes".

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "ext_save.h"
#include "pokedex.h"

#define MAX_NAME_LENGTH 20
#define MAX_LENGTH 100

//Pokedex struct contains dexnodes.
struct pokedex {
    struct dexnode *head;
    struct dexnode *dexnode_select;
    struct dexnode *node_first;  
    int pokemon_number; 
};

//Dexnode struct contains pokemon.
struct dexnode {   
    struct dexnode *next; 
    struct dexnode *prev;
    int found;
    Pokemon pokemon;
};

//Function definitions
Pokedex add_pokedex(Pokedex pokedex, struct dexnode *new_dexnode);
static struct dexnode *create_new_dexnode(Pokemon pokemon, struct dexnode *node);
static int pokemon_check (Pokedex pokedex, int pokemon_id);
static void pokemon_finder(struct dexnode *node);
static void print_type(struct dexnode *node, int fail_type);
static int compare_strings(const char*one, const char*two);
static void print_type2(struct dexnode *node);

// Creates a new Pokedex, and returns a pointer to it.
Pokedex new_pokedex(void) {

    Pokedex new_pokedex = malloc(sizeof(struct pokedex));
    assert(new_pokedex != NULL);
    new_pokedex->pokemon_number = 0;
    new_pokedex->head = NULL;
    new_pokedex->dexnode_select = NULL;
    return new_pokedex;
}

// Create a new dexnode struct and returns a pointer to it.
static struct dexnode *create_new_dexnode(Pokemon pokemon, struct dexnode *node) {
    
    struct dexnode *n;
    n = malloc(sizeof(struct dexnode));
    assert(n != NULL);
    n->next = NULL;
    n->pokemon = pokemon;
    n->found = 1;
    n->prev = NULL;
    
    if (node != NULL) {
        node->next = n;
        n->prev = node;
    }
    return n;   
}

//////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                        //
//////////////////////////////////////////////////////////////////////

// Add a new Pokemon to the Pokedex.
void add_pokemon(Pokedex pokedex, Pokemon pokemon) {
 
    //Checks to see if pokemon already exists.
    if (pokemon_check(pokedex, pokemon_id(pokemon)) == 1) {
        printf ("Exiting because the pokemon already exists\n");
        exit(1);
    }
    
    //Checks the contents of the pokedex, whether empty or not.
    struct dexnode *node;
    if (pokedex->head == NULL) {
        node = NULL;
    } else {
        node = pokedex->node_first;
    }
    
    //Inserts pokemon into a new dexnode.
    struct dexnode *new_dexnode = create_new_dexnode(pokemon, node);
    if (pokedex->pokemon_number == 0) {
        pokedex->head = new_dexnode;
        pokedex->dexnode_select = new_dexnode;
    }
    
    //Add one to the tally of pokemon in the pokedex.
    pokedex->pokemon_number++;
    pokedex->node_first = new_dexnode;
}

// Print out all of the Pokemon in the Pokedex.
void print_pokedex(Pokedex pokedex) {

    //node_curr is a pointer to the pokemon to be printed.
    struct dexnode *node_curr = pokedex->head;
    while (node_curr != NULL) {
        if (node_curr->pokemon == pokedex->dexnode_select->pokemon) {
            //Shows user the current selected pokemon.
            printf ("--> ");
        } else {
            printf ("    ");
        }
        printf ("#%03d: ", pokemon_id(node_curr->pokemon));
        pokemon_finder(node_curr);
        printf("\n");
        node_curr = node_curr->next;
    }  
}

// Print out the details of the currently selected Pokemon.
void detail_current_pokemon(Pokedex pokedex) {
    
    //Dexnode_select is a pointer to the currently selected pokemon.
    struct dexnode *dexnode_select = pokedex->dexnode_select;
    if (dexnode_select != NULL) {
        if (dexnode_select->found == 0) {
            //If the pokemon has not been found, hide some of its information.
            printf ("ID: %03d\n", pokemon_id(dexnode_select->pokemon));
            printf ("Name: ");
            pokemon_finder(dexnode_select);
            printf ("\n");
            printf ("Height: --\n");
            printf ("Weight: --\n");
            printf ("Type: --\n");
        } else {
            //Otherwise, the pokemon is found and its information is displayed.
            printf ("ID: %03d\n", pokemon_id(dexnode_select->pokemon));
            printf ("Name: %s\n", pokemon_name(dexnode_select->pokemon));
            printf ("Height: %.6lf m\n", pokemon_height(dexnode_select->pokemon));
            printf ("Weight: %.6lf kg\n", pokemon_weight(dexnode_select->pokemon));
            print_type(dexnode_select, 1);
        }
    }   
}

// Return the currently selected Pokemon.
Pokemon get_current_pokemon(Pokedex pokedex) {
    
    Pokemon pokemon = NULL;
    //Checks if a pokemon is selected.
    if (pokedex->dexnode_select != NULL) {
        pokemon = pokedex->dexnode_select->pokemon;
    } else {
        printf ("Exiting because the current pokedex is empty\n");
        exit(1);
    }
    return pokemon;  
}

// Change the currently selected Pokemon to be the next Pokemon in the Pokedex.
void next_pokemon(Pokedex pokedex) {

    struct dexnode *pokemon_select = pokedex->dexnode_select;
    if (pokemon_select != NULL) {
        //The if statements check that the pointers are not NULL.
        if (pokemon_select->next != NULL) {
            pokedex->dexnode_select = pokedex->dexnode_select->next;
        }
    }   
}

// Change the currently selected Pokemon to be the previous Pokemon in the Pokedex.
void prev_pokemon(Pokedex pokedex) {

    struct dexnode *pokemon_select = pokedex->dexnode_select;
    if (pokemon_select != NULL) {
        //The if statements check that the pointers are not NULL.
        if (pokemon_select->prev != NULL) {
            pokedex->dexnode_select = pokedex->dexnode_select->prev;
        }
    }
}

// Change the currently selected Pokemon to be the Pokemon with the ID `id`.
void jump_pokemon(Pokedex pokedex, int id) {
   
    struct dexnode *node_first = pokedex->head;
    //The following while loop cycles through the dexnodes until the ID's match.
    while (node_first != NULL) {
        if (pokemon_id(node_first->pokemon) == id) {
            pokedex->dexnode_select = node_first;
        }
        node_first = node_first->next;
    }   
}

//////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                        //
//////////////////////////////////////////////////////////////////////

// Sets the currently selected pokemon's "found" status
void set_current_pokemon_found(Pokedex pokedex, int found) {
    
    if (pokedex->dexnode_select != NULL) {   
        if (found == 0) {  
            pokedex->dexnode_select->found = 0;        
        } else {
            pokedex->dexnode_select->found = 1;
        }       
    }    
}

// Add a pokemon before the current pokemon.
// INCOMPLETE
void insert_pokemon_before_current(Pokedex pokedex, Pokemon pokemon) {
    struct dexnode *node_first = pokedex->dexnode_select;
    
    if (node_first == NULL || node_first == pokedex->head) {
        //If the pokedex is empty or node_first is head, add pokemon as normal.
        add_pokemon(pokedex, pokemon);
    } else {
    
        //Create temp for node_first
        struct dexnode *temp = node_first;
        
        //Go to node_first->prev
        struct dexnode *temp_before = node_first->prev;

        //Inserts pokemon into a new dexnode.
        struct dexnode *new_dexnode = create_new_dexnode(node_first->pokemon, temp);
        pokedex->pokemon_number++;       
        
        //Link new_dexnode to node_first pointer
        new_dexnode->next = temp;
        
        //Link node_first->prev pointer to new dexnode
        temp_before->next = new_dexnode;       
        
        //Return to new dexnode
        node_first = new_dexnode;
    }
    
     
    
}

// Return the total number of Pokemon of a single type in the Pokedex.
int count_pokemon_of_type(Pokedex pokedex, char *type) {
       
    int count = 0;
    struct dexnode *node_first = pokedex->head;
    pokemon_type type_code = type_str_to_code(type);
    
    while (node_first != NULL) {

        pokemon_type type1 = pokemon_first_type(node_first->pokemon);      
        pokemon_type type2 = pokemon_second_type(node_first->pokemon); 

        //If types are the same, then increase count by one.
        if (type1 == type_code) {
            count++;
        } else if (type2 == type_code) {
            count++;
        }
 
        node_first = node_first->next;
    }
    return count;
}

//////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                        //
//////////////////////////////////////////////////////////////////////


// Remove the currently selected Pokemon from the Pokedex.
void remove_pokemon(Pokedex pokedex) {  
    struct dexnode *node = pokedex->dexnode_select;
    struct dexnode *next = node->next;
    struct dexnode *prev = node->prev;
    
    if (node != pokedex->head && node != NULL) {
        destroy_pokemon(node->pokemon);
        if (next == NULL) {
            pokedex->dexnode_select = prev;
        } else {
            pokedex->dexnode_select = next;
            node->next->prev = prev;
        }
        node->prev->next = next;
    }
    
    //If the current pokemon is first in the pokedex.
    if (node == pokedex->head) {
        pokedex->dexnode_select = next;
        destroy_pokemon(node->pokemon);
        
        if (pokedex->head->next != NULL) {
            pokedex->head = pokedex->head->next;
        } else {
            pokedex->head = NULL;
        }
    }
    free(node);
}

// Destroy the given Pokedex and free all associated memory.
void destroy_pokedex(Pokedex pokedex) {
    struct dexnode *node = pokedex->head;
    struct dexnode *node_next;
    
    //Cycle through pokedex and destroy all pokemon.
    while (node != NULL) {
        destroy_pokemon(node->pokemon);
        node_next = node->next;
        free(node);
        node = node_next;
    }
    free(pokedex);
    pokedex = NULL;
}

// Prints out all types of found Pokemon stored in the Pokedex
//INCOMPLETE
void show_types(Pokedex pokedex) {
    
    if (pokedex->head != NULL) {
        struct dexnode *node_first = pokedex->head;
        struct dexnode *temp = NULL;
        
        //Create a temporary struct that mirrors the pokedex in pokedex
        while (node_first != NULL) {
            temp = node_first;
            temp = temp->next;
            node_first = node_first->next;
        }
        
        node_first = pokedex->head; 
        int type_count = 0;
        
        while (node_first != NULL) { 
        
            temp = pokedex->head;
            while (temp != NULL) {
            
                //Count how many times the type at hand appears within the pokedex
                pokemon_type type1 = pokemon_first_type(temp->pokemon);
                pokemon_type type2 = pokemon_first_type(node_first->pokemon);
                
                if (type1 == type2) {
                    type_count++;
                }
                
                temp = temp->next;
            }
 
            //If the type does not appear multiple times, then print.
            if (type_count != 2) {      
                print_type2(node_first);  
            }                    
            node_first = node_first->next;
        }      
    }
}

// Return the number of Pokemon in the Pokedex that have been found.
int count_found_pokemon(Pokedex pokedex) {
    int count_found = 0;
    struct dexnode *node_first = pokedex->head;
    while (node_first != NULL) {
        if (node_first->found == 1) {
            count_found++;
        }
        node_first = node_first->next;
    }
    return count_found;
}

// Return the total number of Pokemon in the Pokedex.
int count_total_pokemon(Pokedex pokedex) {
    int count_total = 0;
    struct dexnode *node_first = pokedex->head;
    while (node_first != NULL) {
        count_total++;
        node_first = node_first->next;
    }
    return count_total;
}

//////////////////////////////////////////////////////////////////////
//                     Stage 4 Extension Functions                  //
//////////////////////////////////////////////////////////////////////

/*
 * Saves a pokedex to a text file
 * Use the functions in ext_save.h to save the contents to a file
 */
void save_pokedex(Pokedex pokedex, char *filename) {
    fprintf (stderr, "exiting because you have not implemented the save_pokedex function\n");
    exit(1);

}

/*
 * Loads a pokedex from a text file
 * Use the functions in ext_save.h to load the text from a file
 */
Pokedex load_pokedex(char *filename) {
    fprintf(stderr, "exiting because you have not implemented the load_pokedex function\n");
    exit(1);

    return NULL;
}

//////////////////////////////////////////////////////////////////////
//                     Stage 5 Extension Functions                  //
//////////////////////////////////////////////////////////////////////

// Create a new Pokedex which contains only the Pokemon of a specified
// type from the original Pokedex.
Pokedex get_pokemon_of_type(Pokedex pokedex, pokemon_type type) {
    fprintf(stderr, "exiting because you have not implemented the get_pokemon_of_type function\n");
    exit(1);
}

// Create a new Pokedex which contains only the Pokemon that have
// previously been 'found' from the original Pokedex.
Pokedex get_found_pokemon(Pokedex pokedex) {
    fprintf(stderr, "exiting because you have not implemented the get_found_pokemon function\n");
    exit(1);
}

// Create a new Pokedex containing only the Pokemon from the original
// Pokedex which have the given string appearing in its name.
Pokedex search_pokemon(Pokedex pokedex, char *text) {
    fprintf(stderr, "exiting because you have not implemented the search_pokemon function\n");
    exit(1);
}

// Free's the current sub-list and returns the original pokedex state, 
// prior to the search
Pokedex end_search(Pokedex sub_pokedex) {
    fprintf(stderr, "exiting because you have not implemented the end_search function\n");
    exit(1);
}

//Insert new functions below

//Checks if the pokemon has been added to the pokedex or not.
static int pokemon_check(Pokedex pokedex, int id) {
    struct dexnode *node_first = pokedex->head;
    //The following while loop checks the pokedex to see if the pokemon has been
    // added yet.
    while (node_first != NULL) {
        if (pokemon_id(node_first->pokemon) == id) {
            return 1;    
        }
        node_first = node_first->next;
    }
    return 0;
}

//Determines whether to print the pokemons name or to hide it. This depends
// on its status as found or not.
static void pokemon_finder(struct dexnode *node) {
    Pokemon pokemon = node->pokemon;
    char *name = pokemon_name(pokemon);
    char hidden[MAX_LENGTH] = {0};
    int loop = 0;
    
    //Determine the amount of asterisks to hide the pokemons name.
    if (node->found == 0) {
        while (name[loop] != '\0') {
            hidden[loop] = 42;
            loop++;
        }
    }
    
    //If the pokemon has not been found, let its name be asterisks.
    if (node->found == 0) {
        printf ("%s", hidden);
    } else {
        printf ("%s", name);
    }
}

//A function to print the types of the selected pokemon.
static void print_type(struct dexnode *node, int fail_type) {
    
    Pokemon pokemon = node->pokemon;
    pokemon_type type1 = pokemon_first_type(pokemon);
    pokemon_type type2 = pokemon_second_type(pokemon);
    
    const char *type1_str = type_code_to_str(type1);
    const char *type2_str = type_code_to_str(type2);
    const char *none = "None";
  
    if (fail_type == 1) {
        if (compare_strings(type2_str, none) == 1) {
            printf ("Type: %s\n", type1_str);
        } else {
            printf ("Type: %s %s\n", type1_str, type2_str);
        }
    } else if (fail_type == 2) {
        if (node->found == 1) {
            if (compare_strings(type2_str, none) == 1) {
                printf (" [%s]", type1_str);
            } else {
                printf (" [%s, %s]", type1_str, type2_str);
            }
        } else {
            printf (" [????]");
        }
    }
}

//Prints pokemon type in the format for the function print types.
static void print_type2(struct dexnode *node) {
    
    Pokemon pokemon = node->pokemon;
    
    pokemon_type type1 = pokemon_first_type(pokemon);
    const char *type1_str = type_code_to_str(type1);
    
    pokemon_type type2 = pokemon_second_type(pokemon);
    const char *type2_str = type_code_to_str(type2);  

    const char *none = "None";
  
    if (compare_strings(type2_str, none) == 1) {
        printf ("%s\n", type1_str);
    } else {
        printf ("%s\n%s\n", type1_str, type2_str);
    }
}

//Compares two strings to see if they are identical or not.
//If they match, return 1. Else return 0.
//Note: Concept taken from github - MacauleyDallas
static int compare_strings(const char *one, const char *two) {
    int loop = 0;
    int char2 = 0;
    int char1 = 0;
    int char_match = 0;
    
    //Determine the amount of characters that must match.
    while (one[loop] == two[loop] && one[loop] != '\0') {
        loop++;
    }
    char_match = loop;
    //Create an array consisting of string one
    loop = 0;
    while (one[loop] != '\0') {
        loop++;
    }
    char1 = loop;
    //Create an array consisting of string two.  
    loop = 0;
    while (two[loop] != '\0') {
        loop++;
    }
    char2 = loop;
    if (char1 == 0) {
        return 0;
    }
    //If the strings are the same, then the comparison is a success.
    if (char1 == char2 && char2 == char_match) {
        return 1;
    } else {
        return 0;
    }
    return 0;
}

