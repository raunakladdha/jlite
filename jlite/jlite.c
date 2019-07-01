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
 * @file jlite.c
 * @author Raunak Laddha
 * @date 24 February 2019
 * @brief This file contains the APIs definition which can be used to parse a JSON.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jlite.h"

#define JLITE_MAX_NUMBER_LEN 16

static int jlite_jsoneq(const char *jsonstr, jlitetok_t *token,
    const char *key)
{
  if (!jsonstr || !token || !key)
  {
    return -JLITE_E_INVALID_PARAM;
  }

#if JLITE_DEBUG
  char buffer[50] = "";
  snprintf(buffer, 50, "%s", jsonstr + token->start);
  printf("COMPARING KEY %s with value %s\r\n", key, buffer);
#endif

  if (token->type == JSMN_STRING &&
      (int) strlen(key) == token->end - token->start &&
      strncmp(jsonstr + token->start, key, token->end - token->start) == 0) 
  {
    return JLITE_OK;
  }
  return -JLITE_E_NO_MATCH;
}

static int jlite_skip_array(jliteobj_t *jsonobj, int *pos);

static int jlite_skip_object(jliteobj_t *jsonobj, int *pos)
{
  if (!jsonobj || !pos)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;
  int size = 0;
  int i = 0;

  cur_pos = *pos;
  size = jsonobj->tokens[cur_pos].size;
  for (i = 0; i < size; i++)
  {
    cur_pos++;
    if (jsonobj->tokens[cur_pos + 1].type == JSMN_OBJECT)
    {
      cur_pos++;
      ret = jlite_skip_object(jsonobj, &cur_pos);
      if (ret != JLITE_OK)
      {
        return ret;
      }
    }
    else if (jsonobj->tokens[cur_pos + 1].type == JSMN_ARRAY)
    {
      cur_pos++;
      ret = jlite_skip_array(jsonobj, &cur_pos);
      if (ret != JLITE_OK)
      {
        return ret;
      }
    }
    else
    {
      cur_pos = cur_pos + 1;
    }
  }
  *pos = cur_pos;
  return JLITE_OK;
}

static int jlite_skip_array(jliteobj_t *jsonobj, int *pos)
{
  if (!jsonobj || !pos)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;
  int size = 0;
  int i = 0;

  cur_pos = *pos;
  size = jsonobj->tokens[cur_pos].size;
  for (i = 0; i < size; i++)
  {
    if (jsonobj->tokens[cur_pos + 1].type == JSMN_OBJECT)
    {
      cur_pos++;
      ret = jlite_skip_object(jsonobj, &cur_pos);
      if (ret != JLITE_OK)
      {
        return ret;
      }
    }
    else if (jsonobj->tokens[cur_pos + 1].type == JSMN_ARRAY)
    {
      cur_pos++;
      ret = jlite_skip_array(jsonobj, &cur_pos);
      if (ret != JLITE_OK)
      {
        return ret;
      }
    }
    else
    {
      cur_pos++;
    }
  }
  *pos = cur_pos;
  return JLITE_OK;
}

static int jlite_find_key_position(jliteobj_t *jsonobj, const char *key,
    int *pos)
{
  if (!jsonobj || !key || !pos)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int i = 0;
  int ret = 0;
  int cur_pos = 0;
  jlitetok_t *tokens = NULL;
  char *jsonstr = NULL;
  int size = 0;

  cur_pos = jsonobj->cur_pos;
  tokens = jsonobj->tokens;
  jsonstr = jsonobj->jsonstr;

  if (tokens[cur_pos - 1].type != JSMN_OBJECT)
  {
    return -JLITE_E_NOT_OBJECT;
  }

  size = tokens[cur_pos - 1].size;

  for (i = 0; i < size; i++)
  {
    ret = jlite_jsoneq(jsonstr, &tokens[cur_pos], key);
    if (ret == JLITE_OK)
    {
      *pos = cur_pos + 1;
      return JLITE_OK;
    }
    else
    {
      if (tokens[cur_pos + 1].type == JSMN_OBJECT)
      {
        cur_pos++;
        ret = jlite_skip_object(jsonobj, &cur_pos);
        if (ret != JLITE_OK)
        {
          return ret;
        }
      }
      else if (tokens[cur_pos + 1].type == JSMN_ARRAY)
      {
        cur_pos++;
        ret = jlite_skip_array(jsonobj, &cur_pos);
        if (ret != JLITE_OK)
        {
          return ret;
        }
      }
      else
      {
        cur_pos++;
      }
      cur_pos++;
    }
  }

  return -JLITE_E_KEY_NOT_FOUND;
}

int jlite_parse_init(jliteobj_t *jsonobj, jlitetok_t *tokens,
    int num_tokens, char *jsonstr, int jsonstr_len)
{
  if (!jsonobj || !tokens || !num_tokens || !jsonstr || !jsonstr_len)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;

  memset(jsonobj, 0, sizeof(*jsonobj));
  jlite_parser *j_parser = &jsonobj->j_parser;

  jsmn_init(j_parser);

  ret = jsmn_parse(j_parser, jsonstr, jsonstr_len, tokens, num_tokens);
  if (ret < 0)
  {
    return -JLITE_ERR;
  }

#if JLITE_DEBUG
  for (int i = 0; i < ret; i++)
  {
    char buffer[16] = "";
    snprintf(buffer,((tokens[i].end - tokens[i].start) & 15) + 1,
        jsonstr + tokens[i].start);
    printf("TOKEN NUM %d\r\nPARENT TOKEN %d\r\nSize %d\r\nTYPE %d\r\nVALUE %s\r\n\r\n", i,
        tokens[i].parent,tokens[i].size, tokens[i].type, buffer);
  }
#endif

  jsonobj->jsonstr = jsonstr;
  jsonobj->jsonstr_len = jsonstr_len;
  jsonobj->tokens = tokens;
  jsonobj->num_tokens = ret;
  jsonobj->cur_pos = 1;

  return JLITE_OK;
}

static int jlite_get_value_str_by_pos(jliteobj_t *jsonobj, int pos,
    char *value, int max_value_len)
{
  if (!jsonobj || !value || !max_value_len)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int value_len = 0;
  char *jsonstr = NULL;
  jlitetok_t *token = NULL;

  jsonstr = jsonobj->jsonstr;
  token = &jsonobj->tokens[pos];
  value_len = token->end - token->start + 1;
  if (value_len > max_value_len)
  {
    return -JLITE_E_NO_BUFFER_MEM;
  }

  snprintf(value, value_len, "%s", jsonstr + token->start);

  return JLITE_OK;
}

static int jlite_get_value_int_by_pos(jliteobj_t *jsonobj, int pos,
    int *value)
{
  if (!jsonobj || !value)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  char value_str[JLITE_MAX_NUMBER_LEN + 1] = "";
  int max_value_len = JLITE_MAX_NUMBER_LEN;

  ret = jlite_get_value_str_by_pos(jsonobj, pos, value_str, max_value_len);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  *value = atoi(value_str);
  return JLITE_OK;
}

static int jlite_get_value_float_by_pos(jliteobj_t *jsonobj, int pos,
    float *value)
{
  if (!jsonobj || !value)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  char value_str[JLITE_MAX_NUMBER_LEN + 1] = "";
  int max_value_len = JLITE_MAX_NUMBER_LEN;

  ret = jlite_get_value_str_by_pos(jsonobj, pos, value_str, max_value_len);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  *value = atof(value_str);
  return JLITE_OK;
}

static int jlite_get_value_number_str(jliteobj_t *jsonobj,
    const char *key, char *value_str, int max_value_len)
{
  if (!jsonobj || !key || !value_str || !max_value_len)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_find_key_position(jsonobj, key, &cur_pos);
  if (ret != 0)
  {
    return ret;
  }

  if (jsonobj->tokens[cur_pos].type != JSMN_PRIMITIVE)
  {
    return -JLITE_E_NOT_NUMBER;
  }

  return jlite_get_value_str_by_pos(jsonobj, cur_pos, value_str,
      max_value_len);
}

int jlite_get_value_float(jliteobj_t *jsonobj, const char *key,
    float *value)
{
  if (!jsonobj || !key || !value)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  char value_str[JLITE_MAX_NUMBER_LEN + 1] = "";
  int max_value_len = JLITE_MAX_NUMBER_LEN;

  ret = jlite_get_value_number_str(jsonobj, key, value_str, max_value_len);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  *value = atof(value_str);

  return JLITE_OK;
}

int jlite_get_value_bool(jliteobj_t *jsonobj, const char *key, int *value)
{
  if (!jsonobj || !key || !value)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  char value_str[JLITE_MAX_NUMBER_LEN + 1] = "";
  int max_value_len = JLITE_MAX_NUMBER_LEN;

  ret = jlite_get_value_number_str(jsonobj, key, value_str, max_value_len);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  if (strncmp(value_str, "true", max_value_len) == 0)
  {
    *value = 1;
  }
  else if (strncmp(value_str, "1", max_value_len) == 0)
  {
    *value = 1;
  }
  else if (strncmp(value_str, "false", max_value_len) == 0)
  {
    *value = 0;
  }
  else if (strncmp(value_str, "0", max_value_len) == 0)
  {
    *value = 0;
  }
  else
  {
    return -JLITE_E_NOT_BOOLEAN;
  }

  return JLITE_OK;
}

int jlite_get_value_int(jliteobj_t *jsonobj, const char *key, int *value)
{
  if (!jsonobj || !key || !value)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  char value_str[JLITE_MAX_NUMBER_LEN + 1] = "";
  int max_value_len = JLITE_MAX_NUMBER_LEN;

  ret = jlite_get_value_number_str(jsonobj, key, value_str, max_value_len);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  *value = atoi(value_str);

  return JLITE_OK;

}

int jlite_get_value_str_len(jliteobj_t *jsonobj, const char *key,
    int *value_len)
{
  if (!jsonobj || !key || !value_len)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_find_key_position(jsonobj, key, &cur_pos);
  if (ret != 0)
  {
    return ret;
  }

  *value_len = jsonobj->tokens[cur_pos].end - jsonobj->tokens[cur_pos].start;
  return JLITE_OK;
}

int jlite_get_value_str(jliteobj_t *jsonobj, const char *key, char *value,
    int max_value_len)
{
  if (!jsonobj || !key || !value || !max_value_len)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_find_key_position(jsonobj, key, &cur_pos);
  if (ret != 0)
  {
    return ret;
  }

  if (jsonobj->tokens[cur_pos].type != JSMN_STRING)
  {
    return -JLITE_E_NOT_STRING;
  }

  return jlite_get_value_str_by_pos(jsonobj, cur_pos, value, max_value_len);
}

int jlite_get_json_object(jliteobj_t *jsonobj, const char *key)
{
  if (!jsonobj || !key)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_find_key_position(jsonobj, key, &cur_pos);
  if (ret != 0)
  {
    return ret;
  }

  if (jsonobj->tokens[cur_pos].type != JSMN_OBJECT)
  {
    return -JLITE_E_NOT_OBJECT;
  }

  cur_pos++;
  jsonobj->cur_pos = cur_pos;
  return JLITE_OK;
}

static int jlite_get_json_obj_by_pos(jliteobj_t *jsonobj, int cur_pos)
{
  if (!jsonobj)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  if (jsonobj->tokens[cur_pos].type != JSMN_OBJECT)
  {
    return -JLITE_E_NOT_OBJECT;
  }

  cur_pos++;
  jsonobj->cur_pos = cur_pos;
  return JLITE_OK;
}

int jlite_release_json_object(jliteobj_t *jsonobj)
{
  if (!jsonobj)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int cur_pos = 0;

  cur_pos = jsonobj->cur_pos;
  cur_pos = jsonobj->tokens[cur_pos].parent;
  if (jsonobj->tokens[cur_pos].type != JSMN_OBJECT)
  {
    return -JLITE_E_NOT_OBJECT;
  }

  cur_pos = jsonobj->tokens[cur_pos].parent;
  cur_pos = jsonobj->tokens[cur_pos].parent;
  cur_pos++;
  jsonobj->cur_pos = cur_pos;

  return JLITE_OK;
}

static int jlite_get_json_array_by_pos(jliteobj_t *jsonobj, int cur_pos, int *size)
{
  if (!jsonobj || !size)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  if (jsonobj->tokens[cur_pos].type != JSMN_ARRAY)
  {
    return -JLITE_E_NOT_ARRAY;
  }

  *size = jsonobj->tokens[cur_pos].size;
  cur_pos++;
  jsonobj->cur_pos = cur_pos;
  return JLITE_OK;
}

static int jlite_get_json_array_position(jliteobj_t *jsonobj,
    const char *key, int *pos)
{
  if (!jsonobj || !key || !pos)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_find_key_position(jsonobj, key, &cur_pos);
  if (ret != 0)
  {
    return ret;
  }

  if (jsonobj->tokens[cur_pos].type != JSMN_ARRAY)
  {
    return -JLITE_E_NOT_ARRAY;
  }

  *pos = cur_pos;
  return JLITE_OK;
}

int jlite_get_json_array(jliteobj_t *jsonobj, const char *key, int *size)
{
  if (!jsonobj || !key || !size)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_get_json_array_position(jsonobj, key, &cur_pos);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  *size = jsonobj->tokens[cur_pos].size;
  cur_pos++;
  jsonobj->cur_pos = cur_pos;
  return JLITE_OK;
}

int jlite_get_json_array_size(jliteobj_t *jsonobj, const char *key,
    int *size)
{
  if (!jsonobj || !key || !size)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_get_json_array_position(jsonobj, key, &cur_pos);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  *size = jsonobj->tokens[cur_pos].size;
  return JLITE_OK;
}

int jlite_release_json_array(jliteobj_t *jsonobj)
{
  if (!jsonobj)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int cur_pos = 0;

  cur_pos = jsonobj->cur_pos;
  cur_pos--;
  if (jsonobj->tokens[cur_pos].type != JSMN_ARRAY)
  {
    return -JLITE_E_NOT_ARRAY;
  }

  cur_pos = jsonobj->tokens[cur_pos].parent;
  cur_pos = jsonobj->tokens[cur_pos].parent;
  cur_pos++;
  jsonobj->cur_pos = cur_pos;
  return JLITE_OK;
}

static int jlite_get_json_array_element_offet_position(
    jliteobj_t *jsonobj, int element_pos, int *offset_pos)
{
  if (!jsonobj || !offset_pos)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int cur_pos = 0;
  int i = 0;
  jlitetok_t *tokens = NULL;

  tokens = jsonobj->tokens;
  cur_pos = jsonobj->cur_pos;
  cur_pos--;
  if (tokens[cur_pos].type != JSMN_ARRAY)
  {
    return -JLITE_E_NOT_ARRAY;
  }

  if (tokens[cur_pos].size < element_pos)
  {
    return -JLITE_E_OUT_OF_BOUND;
  }

  cur_pos++;

  for (i = 0; i < element_pos; i++)
  {
    if (tokens[cur_pos].type == JSMN_OBJECT)
    {
      cur_pos = cur_pos + (tokens[cur_pos].size * 2) + 1;
    }
    else if (tokens[cur_pos].type == JSMN_ARRAY)
    {
      cur_pos = cur_pos + (tokens[cur_pos].size) + 1;
    }
    else
    {
      cur_pos++;
    }
  }

  *offset_pos = cur_pos;
  return JLITE_OK;
}

int jlite_get_json_array_element_str(jliteobj_t *jsonobj,
    int element_pos, char *value, int max_value_len)
{
  if (!jsonobj || !value || !max_value_len)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_get_json_array_element_offet_position(jsonobj, element_pos,
      &cur_pos);
  if (ret != JLITE_OK)
  {
    return ret;
  }
  return jlite_get_value_str_by_pos(jsonobj, cur_pos, value, max_value_len);
}

int jlite_get_json_array_element_int(jliteobj_t *jsonobj,
    int element_pos, int *value)
{
  if (!jsonobj || !value)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_get_json_array_element_offet_position(jsonobj, element_pos,
      &cur_pos);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  return jlite_get_value_int_by_pos(jsonobj, cur_pos, value);
}

int jlite_get_json_array_element_float(jliteobj_t *jsonobj,
    int element_pos, float *value)
{
  if (!jsonobj || !value)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_get_json_array_element_offet_position(jsonobj, element_pos,
      &cur_pos);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  return jlite_get_value_float_by_pos(jsonobj, cur_pos, value);
}

int jlite_get_array_element_object(jliteobj_t *jsonobj, int element_pos)
{
  if (!jsonobj)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_get_json_array_element_offet_position(jsonobj, element_pos,
      &cur_pos);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  return jlite_get_json_obj_by_pos(jsonobj, cur_pos);
}

int jlite_release_array_element_object(jliteobj_t *jsonobj)
{
  if (!jsonobj)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int cur_pos = 0;

  cur_pos = jsonobj->cur_pos;
  cur_pos = jsonobj->tokens[cur_pos].parent;

  if (jsonobj->tokens[cur_pos].type != JSMN_OBJECT)
  {
    return -JLITE_E_NOT_OBJECT;
  }

  cur_pos = jsonobj->tokens[cur_pos].parent;

  if (jsonobj->tokens[cur_pos].type != JSMN_ARRAY)
  {
    return -JLITE_E_NOT_ARRAY;
  }

  jsonobj->cur_pos = cur_pos;
  return JLITE_OK;
}

int jlite_get_array_element_array(jliteobj_t *jsonobj, int element_pos, int *size)
{
  if (!jsonobj || !size)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int ret = 0;
  int cur_pos = 0;

  ret = jlite_get_json_array_element_offet_position(jsonobj, element_pos,
      &cur_pos);
  if (ret != JLITE_OK)
  {
    return ret;
  }

  return jlite_get_json_array_by_pos(jsonobj, cur_pos, size);
}

int jlite_release_array_element_array(jliteobj_t *jsonobj)
{
  if (!jsonobj)
  {
    return -JLITE_E_INVALID_PARAM;
  }

  int cur_pos = 0;

  cur_pos = jsonobj->cur_pos;
  cur_pos = jsonobj->tokens[cur_pos].parent;

  if (jsonobj->tokens[cur_pos].type != JSMN_ARRAY)
  {
    return -JLITE_E_NOT_OBJECT;
  }

  cur_pos = jsonobj->tokens[cur_pos].parent;

  if (jsonobj->tokens[cur_pos].type != JSMN_ARRAY)
  {
    return -JLITE_E_NOT_ARRAY;
  }

  cur_pos++;
  jsonobj->cur_pos = cur_pos;
  return JLITE_OK;
}
