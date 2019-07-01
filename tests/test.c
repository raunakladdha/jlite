#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jlite/jlite.h"

#define TEST_J_STR "{\"test_obj\":{\"array_array\":[[\"array1_str_0\",\"array1_str_1\",\"array1_str_2\",\"array1_str_3\"],[\"array1_str_4\",\"array1_str_5\",\"array1_str_6\",\"array1_str_7\"]],\"str_test\":\"johndoe1\",\"int_test\":315,\"float_test\":115.16,\"bool_test\":false,\"obj_test\":{\"obj_int_test\":11994,\"obj_str_test\":\"ra1unak\",\"obj_float_test\":161.04},\"array_test_str\":[\"array_str_0\",\"array_str_1\",\"array_str_2\",\"array_str_3\"],\"array_test_obj\":[{\"obj_int_test\":16,\"obj_str_test\":\"array_obj_1\",\"obj_float_test\":94.19},{\"obj_int_test\":4,\"obj_str_test\":\"array_obj_2\",\"obj_float_test\":16.04},{\"obj_int_test\":25,\"obj_str_test\":\"array_obj_3\",\"obj_float_test\":19.16}],\"array_test_int\":[16,4,1994],\"array_test_float\":[19.94,4.16,94.1604]},\"array_array\":[[\"array_str_0\",\"array_str_1\",\"array_str_2\",\"array_str_3\"],[\"array_str_4\",\"array_str_5\",\"array_str_6\",\"array_str_7\"]],\"str_test\":\"johndoe\",\"int_test\":35,\"float_test\":15.16,\"bool_test\":true,\"obj_test\":{\"obj_int_test\":1994,\"obj_str_test\":\"raunak\",\"obj_float_test\":16.04},\"array_test_str\":[\"array_str_0\",\"array_str_1\",\"array_str_2\",\"array_str_3\"],\"array_test_obj\":[{\"obj_int_test\":16,\"obj_str_test\":\"array_obj_1\",\"obj_float_test\":94.19},{\"obj_int_test\":4,\"obj_str_test\":\"array_obj_2\",\"obj_float_test\":16.04},{\"obj_int_test\":25,\"obj_str_test\":\"array_obj_3\",\"obj_float_test\":19.16}],\"array_test_int\":[16,4,1994],\"array_test_float\":[19.94,4.16,94.1604]}"

int main(void)
{
  int ret = 0;
  jliteobj_t jsonobj;
  jlitetok_t tokens[200];
  char *j_str = TEST_J_STR;
  int j_str_len = strlen(j_str);
  char value_str[50] = "";
  int value_int = 0;
  float value_float = 0;
  int bool_test = 0;
  int len = 0;
  int size = 0;

  ret = jlite_parse_init(&jsonobj, tokens, 200, j_str, j_str_len);
  if (ret != JLITE_OK)
  {
    printf("PARSE INIT RET IS %d\r\n", ret);
  }

  ret = jlite_get_json_array(&jsonobj, "array_array", &size);
  if (ret != JLITE_OK)
  {
    printf("Array test obj ret %d\r\n", ret);
  }
  else
  {
    printf("Array test obj success with size %d\r\n", size);
  }

  ret = jlite_get_array_element_array(&jsonobj,1, &size);
  if (ret != JLITE_OK)
  {
    printf("Array test array ret %d\r\n", ret);
  }
  else
  {
    printf("Array test array success with size %d\r\n", size);
  }

  ret = jlite_get_json_array_element_str(&jsonobj, 1, value_str, 50);
  if (ret != JLITE_OK)
  {
    printf("Array test array str ret %d\r\n", ret);
  }
  else
  {
    printf("Array test array str %s\r\n", value_str);
  }

  ret = jlite_release_array_element_array(&jsonobj);
  if (ret != JLITE_OK)
  {
    printf("Array test array release obj ret %d\r\n", ret);
  }
  else
  {
    printf("Array test array release success\r\n");
  }

  ret = jlite_release_json_array(&jsonobj);
  if (ret != JLITE_OK)
  {
    printf("Array test release ret %d\r\n", ret);
  }
  else
  {
    printf("Array release success\r\n");
  }

  ret = jlite_get_value_bool(&jsonobj, "bool_test", &bool_test);
  if (ret != JLITE_OK)
  {
    printf("BOOL TEST RET IS %d\r\n", ret);
  }
  else
  {
    printf("BOOL TEST LEN VALUE %d\r\n", bool_test);
  }
  bool_test = 0;

  ret = jlite_get_value_str_len(&jsonobj, "str_test", &len);
  if (ret != JLITE_OK)
  {
    printf("STR_TEST LEN RET IS %d\r\n", ret);
  }
  else
  {
    printf("STR_TEST LEN VALUE %d\r\n", len);
  }
  len = 0;

  ret = jlite_get_value_str(&jsonobj, "str_test", value_str, 50);
  if (ret != JLITE_OK)
  {
    printf("STR_TEST RET IS %d\r\n", ret);
  }
  else
  {
    printf("STR_TEST VALUE %s\r\n", value_str);
  }
  memset(value_str, 0, 50);

  ret = jlite_get_value_int(&jsonobj, "int_test", &value_int);
  if (ret != JLITE_OK)
  {
    printf("INT_TEST RET IS %d\r\n", ret);
  }
  else
  {
    printf("INT_TEST VALUE_INT %d\n", value_int);
  }
  value_int = 0;

  ret = jlite_get_value_float(&jsonobj, "float_test", &value_float);
  if (ret != JLITE_OK)
  {
    printf("FLOAT_TEST RET IS %d\r\n", ret);
  }
  else
  {
    printf("FLOAT_TEST VALUE_FLOAT %f\n", value_float);
  }
  value_float = 0;

  ret = jlite_get_json_object(&jsonobj, "obj_test");
  if (ret == JLITE_OK)
  {
    printf("OBJ_TEST Object found\r\n");
    ret = jlite_get_value_str(&jsonobj, "obj_str_test", value_str, 50);
    if (ret != JLITE_OK)
    {
      printf("STR_TEST RET IS %d\r\n", ret);
    }
    else
    {
      printf("STR_TEST VALUE %s\r\n", value_str);
    }
    memset(value_str, 0, 50);

    ret = jlite_get_value_int(&jsonobj, "obj_int_test", &value_int);
    if (ret != JLITE_OK)
    {
      printf("INT_TEST RET IS %d\r\n", ret);
    }
    else
    {
      printf("INT_TEST VALUE_INT %d\n", value_int);
    }
    value_int = 0;

    ret = jlite_get_value_float(&jsonobj, "obj_float_test", &value_float);
    if (ret != JLITE_OK)
    {
      printf("FLOAT_TEST RET IS %d\r\n", ret);
    }
    else
    {
      printf("FLOAT_TEST VALUE_FLOAT %f\n", value_float);
    }
    value_float = 0;

    ret = jlite_release_json_object(&jsonobj);
    if (ret != JLITE_OK)
    {
      printf("Object release RET %d\r\n", ret);
    }
    else
    {
      printf("Object released\r\n");
    }
  }
  else
  {
    printf("GET Object RET %d\r\n", ret);
  }

  ret = jlite_get_value_str_len(&jsonobj, "str_test", &len);
  if (ret != JLITE_OK)
  {
    printf("STR_TEST LEN RET IS %d\r\n", ret);
  }
  else
  {
    printf("STR_TEST LEN VALUE %d\r\n", len);
  }
  len = 0;

  ret = jlite_get_value_str(&jsonobj, "str_test", value_str, 50);
  if (ret != JLITE_OK)
  {
    printf("STR_TEST RET IS %d\r\n", ret);
  }
  else
  {
    printf("STR_TEST VALUE %s\r\n", value_str);
  }
  memset(value_str, 0, 50);

  ret = jlite_get_value_int(&jsonobj, "int_test", &value_int);
  if (ret != JLITE_OK)
  {
    printf("INT_TEST RET IS %d\r\n", ret);
  }
  else
  {
    printf("INT_TEST VALUE_INT %d\n", value_int);
  }
  value_int = 0;

  ret = jlite_get_value_float(&jsonobj, "float_test", &value_float);
  if (ret != JLITE_OK)
  {
    printf("FLOAT_TEST RET IS %d\r\n", ret);
  }
  else
  {
    printf("FLOAT_TEST VALUE_FLOAT %f\n", value_float);
  }
  value_float = 0;

  ret = jlite_get_json_array(&jsonobj, "array_test_float", &size);
  if (ret != JLITE_OK)
  {
    printf("Array test obj ret %d\r\n", ret);
  }
  else
  {
    printf("Array test obj success with size %d\r\n", size);
  }

 ret = jlite_get_json_array_element_float(&jsonobj, 1, &value_float);
  if (ret != JLITE_OK)
  {
    printf("Get Array float ret %d\r\n", ret);
  }
  else
  {
    printf("FLOAT_TEST VALUE_FLOAT %f\n", value_float);
  }

  ret = jlite_release_json_array(&jsonobj);
  if (ret != JLITE_OK)
  {
    printf("Array release RET IS %d\r\n", ret);
  }
  else
  {
    printf("Array release successful\r\n");
  }

  ret = jlite_get_json_array(&jsonobj, "array_test_int", &size);
  if (ret != JLITE_OK)
  {
    printf("Array test int ret %d\r\n", ret);
  }
  else
  {
    printf("Array test int success with size %d\r\n", size);
  }

 ret = jlite_get_json_array_element_int(&jsonobj, 0, &value_int);
  if (ret != JLITE_OK)
  {
    printf("Get Array int ret %d\r\n", ret);
  }
  else
  {
    printf("INT_TEST VALUE_INT %d\n", value_int);
  }

  ret = jlite_release_json_array(&jsonobj);
  if (ret != JLITE_OK)
  {
    printf("Array release RET IS %d\r\n", ret);
  }
  else
  {
    printf("Array release successful\r\n");
  }

  return 0;
}
