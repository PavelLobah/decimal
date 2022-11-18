#ifndef DECIMAL_H_
#define DECIMAL_H_

#include <limits.h>
#include <math.h>
#include <stdio.h>
typedef enum { SUCCESS,
               CONVERTING_ERROR } e_cast_code_t;

typedef struct {
    unsigned int bits[4];
} decimal;

typedef struct {
    unsigned int bits[6];
} big_decimal;

enum { LOW,
       MID,
       HIGH,
       SCALE };
enum { NORMAL_VALUE,
       MY_INFINITY,
       NEGATIVE_INFINITY,
       MY_NAN };

void clean_decimal(decimal *dec);
void copy_decimal(decimal *dest, decimal *src);
// bits
int get_bit(decimal dec, int index);
int set_bit(decimal *dec, int index);
int set_bit_to_int(int number, int index);
// sign
int get_scale(decimal *dec);
void set_scale(decimal *dec, int scale);
int get_sign(int val);
void set_sign(unsigned int *val);
void set_minus_sign(unsigned int *val);
int get_exp_float(float src);
decimal negtive(decimal dec);
// scale
void equal_scale(decimal *dec1, decimal *dec2);
void upper_scale(decimal *dec);
void lower_scale(decimal *dec);
//  конвертер
int from_int_to_decimal(int src, decimal *dst);
int from_decimal_to_int(decimal src, int *dst);
int from_float_to_decimal(float src, decimal *dst);
int from_decimal_to_float(decimal src, float *dst);
// умножение
int mul(decimal value_1, decimal value_2, decimal *result);
void shift_left(decimal *dec);
void shift_righ(decimal *dec);
int sum(decimal value_1, decimal value_2, decimal *result);
int check_decimal_on_null(decimal *dec);
// сравнение
int is_less(decimal val_1, decimal val_2);
int is_greater(decimal val_1, decimal val_2);
int is_equal(decimal val_1, decimal val_2);
int is_not_equal(decimal a, decimal b);
int is_less_or_equal(decimal val_1, decimal val_2);
int is_greater_or_equal(decimal val_1, decimal val_2);
int is_greater_by_abc(decimal val_1, decimal val_2);
//  сложение и вычитание
int add(decimal value_1, decimal value_2, decimal *result);
int sub(decimal value_1, decimal value_2, decimal *result);
//  деление
decimal div_no_scale(decimal dividend, decimal divisor);
int div(decimal value_1, decimal value_2, decimal *result);
//   округление
int check_zero(decimal val);
int my_round(decimal value, decimal *result);
int truncate(decimal value, decimal *result);
int negate(decimal value, decimal *result);
int my_floor(decimal value, decimal *result);
// big_decimal
void clean_big_decimal(big_decimal *big);
void decimal_to_big_decimal(big_decimal *dst, decimal *src);
void big_to_decimal(decimal *dst, big_decimal *src);
int get_bit_big(big_decimal big, int index);
void set_bit_big(big_decimal *big, int index);
big_decimal sum_big_decimal(big_decimal big1, big_decimal big2);
void shift_left_big(big_decimal *big);
void shift_righ_big(big_decimal *big);
big_decimal div_big_on_ten(big_decimal big1);
void copy_big(big_decimal *big_dst, big_decimal *big_src);
int is_greater_or_equal_big(big_decimal big1, big_decimal big2);
void big_dop_code(big_decimal *big);
//  mod
int mod(decimal value_1, decimal value_2, decimal *result);

#endif  //  DECIMAL_H_
