//
// Created by Gilles Sérasset on 09/10/2019.
//

#ifndef LR_ASSIGN_TABLE_SYMBOLS_H
#define LR_ASSIGN_TABLE_SYMBOLS_H

/**
 * Associe une valeur à une variable
 * @param var le nom de la variable.
 * @param val la valeur à associer.
 */
void set_value(char var[], float val);

/**
 * récupère la valeur associée à une variable.
 * @param var le nom de la variable.
 * @return la valeur associée ou 0 si aucune valeur n'est associée.
 */
float get_value(char var[]);

/**
 * Affiche la table des symboles associés à une valeur.
 */
void print_symbols();

#endif //LR_ASSIGN_TABLE_SYMBOLS_H
