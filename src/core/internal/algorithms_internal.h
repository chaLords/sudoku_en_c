// src/core/internal/algorithms_internal.h
#ifndef ALGORITHMS_INTERNAL_H
#define ALGORITHMS_INTERNAL_H

/**
 * @file algorithms_internal.h
 * @brief Internal declarations of auxiliary algorithms
 * 
 * This header is part of the internal implementation of the library
 * and should not be included by external code. Functions declared here
 * are used by the Sudoku generator but are not part of the public API.
 */

/**
 * @brief Generates a uniformly random permutation of a sequence
 * 
 * This function fills an array with a consecutive sequence of integers
 * starting from start, then applies the Fisher-Yates algorithm to
 * reorganize the elements in random order.
 * 
 * Guaranteed properties:
 * - Each number in [start, start+size) appears exactly once
 * - Each of the size! possible permutations has equal probability
 * - Time complexity: O(size)
 * - Space complexity: O(1) additional
 * 
 * @param array Pointer to the array where the permutation will be generated
 * @param size Size of the array (number of elements)
 * @param start First number of the sequence
 * 
 * @note This function assumes array has capacity for at least size elements
 * @note This function uses rand() for randomness, ensure srand() is called before
 * 
 * @warning This is an internal function. Do not use from external code.
 */
void sudoku_generate_permutation(int *array, int size, int start);

#endif // ALGORITHMS_INTERNAL_H
