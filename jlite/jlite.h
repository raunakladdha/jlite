/*
 * MIT License
 *
 * Copyright (c) 2019 Raunak Laddha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * @file jsonLite.h
 * @author Raunak Laddha
 * @date 24 February 2019
 * @brief This file contains the APIs which can be used to parse a JSON.
 *
 */

#ifndef _JLITE_H
#define _JLITE_H

#include "jsmn/jsmn.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JLITE_VERSION "1.0.0"

#define JLITE_DEBUG 0

/**
 * @brief Error codes
 *
 */

enum jlite_err
{
  JLITE_OK = 0,
  JLITE_ERR,
  JLITE_E_NOT_STRING,
  JLITE_E_NOT_NUMBER,
  JLITE_E_NOT_BOOLEAN,
  JLITE_E_NOT_OBJECT,
  JLITE_E_NOT_ARRAY,
  JLITE_E_OUT_OF_BOUND,
  JLITE_E_KEY_NOT_FOUND,
  JLITE_E_NO_BUFFER_MEM,
  JLITE_E_NO_MATCH,
  JLITE_E_INVALID_PARAM,
};

typedef jsmntok_t jlitetok_t;
typedef jsmn_parser jlite_parser;

typedef struct
{
  char *jsonstr;
  int jsonstr_len;
  jlite_parser j_parser;
  jlitetok_t *tokens;
  int num_tokens;
  int cur_pos;
} jliteobj_t;

/**
 * @brief Initializes the JSON object and parse the given string.
 *
 * This function will initialize the JSON object which will be used by
 * subsequent APIs. This will parse the given string, and fill the tokens
 * with parsed elements of JSON.
 *
 * @param jsonobj Pointer to JSON object.
 * @param tokens Pointer to array of JSON tokens.
 * @param num_tokens Number of tokens in tokens array.
 * @param jsonstr Pointer to JSON string to parse.
 * @param jsonstr_len Length of JSON string.
 *
 * @return errcode
 *
 */

int jlite_parse_init(jliteobj_t *jsonobj, jlitetok_t *tokens,
    int num_tokens, char *jsonstr, int jsonstr_len);

/**
 * @brief Get JSON float element value.
 *
 * This will get value of JSON float element of the given key.
 *
 * @param jsonobj Pointer to JSON object.
 * @param key Name of JSON float element.
 * @param value Pointer to a float variable.
 *
 * @return errcode
 *
 */

int jlite_get_value_float(jliteobj_t *jsonobj, const char *key,
    float *value);

/**
 * @brief Get JSON int element value.
 *
 * This will get value of JSON int element of the given key.
 *
 * @param jsonobj Pointer to JSON object.
 * @param key Name of JSON int element.
 * @param value Pointer to an int variable.
 *
 * @return errcode
 *
 */

int jlite_get_value_int(jliteobj_t *jsonobj, const char *key,
    int *value);

/**
 * @brief Get JSON string element value
 *
 * This will get value of JSON string element of the given key.
 *
 * @param jsonobj Pointer to JSON object.
 * @param key Name of JSON string element.
 * @param value Pointer to a char array.
 * @param max_value_len Max length of char array.
 *
 * @return errcode
 *
 */

int jlite_get_value_str(jliteobj_t *jsonobj, const char *key,
    char *value, int max_value_len);

/**
 * @brief Get JSON boolean element value.
 *
 * This will get value of JSON boolean element of the given key.
 *
 * @param jsonobj Pointer to JSON object.
 * @param key Name of JSON boolean element.
 * @param value Pointer to an int variable which will store boolean value;
 *
 * @return errcode
 *
 */

int jlite_get_value_bool(jliteobj_t *jsonobj, const char *key,
    int *value);

/**
 * @brief Get JSON object element.
 *
 * This will get JSON object element of the given key.
 *
 * @param jsonobj Pointer to JSON object.
 * @param key Name of JSON object element.
 *
 * @return errcode
 *
 */

int jlite_get_json_object(jliteobj_t *jsonobj, const char *key);

/**
 * @brief Release JSON object element.
 *
 * This will release already held JSON object element.
 *
 * @param jsonobj Pointer to JSON object.
 *
 * @return errcode
 *
 */

int jlite_release_json_object(jliteobj_t *jsonobj);

/**
 * @brief Get JSON array element.
 *
 * This will get JSON array element of the given key.
 *
 * @param jsonobj Pointer to JSON object.
 * @param key Name of JSON array element.
 * @param size Pointer to an int variable to get size of the array.
 *
 * @return errcode
 *
 */

int jlite_get_json_array(jliteobj_t *jsonobj, const char *key,
    int *size);

/**
 * @brief Release JSON array element.
 *
 * This will release already held JSON array element.
 *
 * @param jsonobj Pointer to JSON array.
 *
 * @return errcode
 *
 */

int jlite_release_json_array(jliteobj_t *jsonobj);

/**
 * @brief Get JSON array element size.
 *
 * This will get JSON array element size of the given key.
 *
 * @param jsonobj Pointer to JSON object.
 * @param key Name of JSON array element.
 * @param size Pointer to an int variable to get size of the JSON array.
 *
 * @return errcode
 *
 */

int jlite_get_json_array_size(jliteobj_t *jsonobj, const char *key,
    int *size);

/**
 * @brief Get JSON string element length.
 *
 * This will get JSON string element length of the given key.
 *
 * @param jsonobj Pointer to JSON object.
 * @param key Name of JSON string element.
 * @param size Pointer to an int variable to get length of the JSON string element.
 *
 * @return errcode
 *
 */

int jlite_get_value_str_len(jliteobj_t *jsonobj, const char *key,
    int *value_len);

/**
 * @brief Get JSON string element from JSON array based on position.
 *
 * This will get JSON string element at the input position from already held JSON array element.
 *
 * @param jsonobj Pointer to JSON object.
 * @param element_pos Position of JSON string element to get.
 * @param value Pointer to the char array.
 * @param max_value_len Max length of char array.
 *
 * @return errcode
 *
 */

int jlite_get_json_array_element_str(jliteobj_t *jsonobj,
    int element_pos, char *value, int max_value_len);

/**
 * @brief Get JSON int element from JSON array based on position.
 *
 * This will get JSON int element at the input position from already held JSON array element.
 *
 * @param jsonobj Pointer to JSON object.
 * @param element_pos Position of JSON int element to get.
 * @param value Pointer to the int variable.
 *
 * @return errcode
 *
 */

int jlite_get_json_array_element_int(jliteobj_t *jsonobj,
    int element_pos, int *value);

/**
 * @brief Get JSON float element from JSON array based on position.
 *
 * This will get JSON float element at the input position from already held JSON array element.
 *
 * @param jsonobj Pointer to JSON object.
 * @param element_pos Position of JSON float element to get.
 * @param value Pointer to the float variable.
 *
 * @return errcode
 *
 */

int jlite_get_json_array_element_float(jliteobj_t *jsonobj,
    int element_pos, float *value);

/**
 * @brief Get JSON object element from JSON array based on position.
 *
 * This will get JSON object element at the input position from already held JSON array element.
 *
 * @param jsonobj Pointer to JSON object.
 * @param element_pos Position of JSON object element to get.
 *
 * @return errcode
 *
 */

int jlite_get_array_element_object(jliteobj_t *jsonobj, int element_pos);

/**
 * @brief Release already held JSON object element in JSON array.
 *
 * This will release already held JSON object element from already held
 * JSON array element.
 *
 * @param jsonobj Pointer to JSON object.
 *
 * @return errcode
 *
 */

int jlite_release_array_element_object(jliteobj_t *jsonobj);

/**
 * @brief Get JSON array element from JSON array based on position.
 *
 * This will get JSON array element at the input position from already held
 * JSON array element.
 *
 * @param jsonobj Pointer to JSON object.
 * @param element_pos Position of JSON array element to get.
 * @param size Pointer to an int variable to get size of the array.
 *
 * @return errcode
 *
 */

int jlite_get_array_element_array(jliteobj_t *jsonobj, int element_pos,
    int *size);

/**
 * @brief Release already held JSON object element in JSON array.
 *
 * This will release already held JSON array element from already held
 * JSON array element.
 *
 * @param jsonobj Pointer to JSON object.
 *
 * @return errcode
 *
 */

int jlite_release_array_element_array(jliteobj_t *jsonobj);

#ifdef __cplusplus
}
#endif

#endif /* _JLITE_H */
