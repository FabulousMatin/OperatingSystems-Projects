#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <string.h>
#include "defs.h"

typedef struct
{
    char* name;
    int amount;
    
} Ingredient;


typedef struct
{
    char* name;
    
    Ingredient* ingredients;
    int n_ingredients;

} Recipe;



int get_recipes(Recipe* recipesOut[]) {
    Recipe* recipes = NULL;
    // Read the JSON data from file
    FILE *file = fopen("recipes.json", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *json_data = (char *)malloc(file_size + 1);
    fread(json_data, 1, file_size, file);
    fclose(file);
    json_data[file_size] = '\0';

    // Parse the JSON data
    cJSON *root = cJSON_Parse(json_data);
    if (root == NULL) {
        printf("Failed to parse JSON data.\n");
        free(json_data);
        return -1;
    } 

    int n_recipes = 0;
    int n_ingredients;
    // Access the parsed data
    cJSON *dish = NULL;
    cJSON_ArrayForEach(dish, root) {
        n_ingredients = 0;
        char* dish_name = dish->string;

        recipes = realloc(recipes, (n_recipes + 1) * sizeof(Recipe));
        recipes[n_recipes].name = (char*)malloc(strlen(dish_name) + 1);
        strcpy(recipes[n_recipes].name, dish_name);
        
        cJSON *ingredients = cJSON_GetObjectItemCaseSensitive(root, dish_name);
        cJSON *ingredient = NULL;
        cJSON_ArrayForEach(ingredient, ingredients) {
            const char *ingredient_name = ingredient->string;
            int quantity = cJSON_GetObjectItemCaseSensitive(ingredients, ingredient_name)->valueint;
            recipes[n_recipes].ingredients = realloc(recipes[n_recipes].ingredients, (n_ingredients + 1) * sizeof(Ingredient));
            recipes[n_recipes].ingredients[n_ingredients].amount = quantity;
            recipes[n_recipes].ingredients[n_ingredients].name = (char*)malloc(strlen(ingredient_name) + 1);
            strcpy(recipes[n_recipes].ingredients[n_ingredients].name, ingredient_name);
            n_ingredients++;
        }
        recipes[n_recipes].n_ingredients = n_ingredients;
        n_recipes++;
    }


    // Clean up
    cJSON_Delete(root);
    free(json_data);

    *recipesOut = recipes;
    return n_recipes;
}

void show_recipes(Recipe* recipes, int mode, int n_recipes) {
    printf(BLUE"ingredient/amount"COL_RESET"\n");
    printf("-----------------------------\n");
    for (int i = 0; i < n_recipes; i++) {
        printf("%d. %s\n", i + 1, recipes[i].name);
        if (mode == REST_GET_RECIPES) {
            for(int j = 0; j < recipes[i].n_ingredients; j++) {
                printf(YELLOW"%s"PURPLE" %d"COL_RESET"\n", recipes[i].ingredients[j].name, recipes[i].ingredients[j].amount);
            }
        }
        printf("-----------------------------\n");
    }
}

int is_food_exist(Recipe* recipes, int n_recipes, char food_name[]){
    for(int i = 0; i < n_recipes; i++) {
        if(!strcmp(recipes[i].name, food_name))
            return 1;
    }
    return 0;
}