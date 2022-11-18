#include "decimal.h"
int div(decimal value_1, decimal value_2, decimal *result) {
    int error = NORMAL_VALUE;
    equal_scale(&value_1, &value_2);
    decimal dec_null;
    clean_decimal(&dec_null);
    int scale_result = 0;
    int sign = 0;
    if (get_sign(value_1.bits[SCALE]) && get_sign(value_2.bits[SCALE])) {
        sign = 0;
        set_minus_sign(&value_1.bits[SCALE]);
        set_minus_sign(&value_2.bits[SCALE]);
    } else if (get_sign(value_1.bits[SCALE]) || get_sign(value_2.bits[SCALE])) {
        sign = 1;
        set_minus_sign(&value_1.bits[SCALE]);
        set_minus_sign(&value_2.bits[SCALE]);
    }
    if (is_equal(value_2, dec_null)) {
        error = MY_NAN;
        clean_decimal(result);
    } else if (is_equal(value_1, dec_null)) {
        clean_decimal(result);
    } else if (is_equal(value_1, value_2)) {
        clean_decimal(result);
        result->bits[LOW] = 1;
        set_scale(result, scale_result);
        if (sign) set_sign(&result->bits[SCALE]);
    } else {
        clean_decimal(result);
        decimal mask;
        clean_decimal(&mask);
        mask.bits[LOW] = 1;
        decimal ten;
        clean_decimal(&ten);
        ten.bits[LOW] = 10;
        decimal temp;
        copy_decimal(&temp, &value_2);
        decimal temp_value_2;
        clean_decimal(&temp_value_2);

        while (is_not_equal(value_1, dec_null) && scale_result < 28) {
            if (is_less(value_1, value_2)) {
                scale_result++;
                mul(value_1, ten, &value_1);
            }
            copy_decimal(&temp_value_2, &value_2);
            shift_left(&temp_value_2);
            if (is_greater_or_equal(value_1, temp_value_2) && is_greater(temp_value_2, dec_null)) {
                shift_left(&value_2);
                shift_left(&mask);
            } else {
                sum(*result, mask, result);
                sub(value_1, value_2, &value_1);
                copy_decimal(&value_2, &temp);
                clean_decimal(&mask);
                mask.bits[LOW] = 1;
            }
        }
        set_scale(result, scale_result);
        if (sign) set_sign(&result->bits[SCALE]);
    }
    return error;
}

void equal_scale(decimal *dec1, decimal *dec2) {
    int scale_dec1 = 0, scale_dec2 = 0;
    scale_dec1 = get_scale(dec1);
    scale_dec2 = get_scale(dec2);
    decimal five;
    clean_decimal(&five);
    five.bits[LOW] = 5;
    if (scale_dec1 < scale_dec2) {
        while (scale_dec1 < scale_dec2) {
            if (get_bit(*dec1, 92) || get_bit(*dec1, 93) || get_bit(*dec1, 94)
            || get_bit(*dec1, 95) || scale_dec1 > 28) {
                if (scale_dec2 - scale_dec1 == 1) {
                    sum(*dec2, five, dec2);
                    set_scale(dec2, scale_dec2);
                }
                lower_scale(dec2);
                scale_dec2 = get_scale(dec2);
            } else {
                upper_scale(dec1);
                scale_dec1 = get_scale(dec1);
            }
        }
    } else if (scale_dec2 < scale_dec1) {
        while (scale_dec2 < scale_dec1) {
            if (get_bit(*dec2, 92) || get_bit(*dec2, 93) ||
                get_bit(*dec2, 94) || get_bit(*dec2, 95) || scale_dec2 > 28) {
                if (scale_dec1 - scale_dec2 == 1) {
                    sum(*dec1, five, dec1);
                    set_scale(dec1, scale_dec1);
                }
                lower_scale(dec1);
                scale_dec1 = get_scale(dec1);
            } else {
                upper_scale(dec2);
                scale_dec2 = get_scale(dec2);
            }
        }
    }
}

void upper_scale(decimal *dec) {
    int scale = 0;
    unsigned int sign = 0;
    sign = get_sign(dec->bits[SCALE]);
    scale = get_scale(dec) + 1;
    decimal ten;
    clean_decimal(&ten);
    ten.bits[LOW] = 10;
    mul(*dec, ten, dec);
    set_scale(dec, scale);
    if (sign) set_sign(&dec->bits[SCALE]);
}

void lower_scale(decimal *dec) {
    int scale = 0;
    unsigned int sign = 0;
    sign = get_sign(dec->bits[SCALE]);
    scale = get_scale(dec) - 1;
    decimal ten;
    clean_decimal(&ten);
    ten.bits[LOW] = 10;
    *dec = div_no_scale(*dec, ten);
    set_scale(dec, scale);
    if (sign) set_sign(&dec->bits[SCALE]);
}

int add(decimal value_1, decimal value_2, decimal *result) {
    int err = NORMAL_VALUE;
    int sign_1 = get_sign(value_1.bits[SCALE]);
    int sign_2 = get_sign(value_2.bits[SCALE]);
    equal_scale(&value_1, &value_2);
    int scale = get_scale(&value_1);
    clean_decimal(result);
    int flag = 0;
    int sign = 0;
    if (sign_1 == 1 && sign_2 == 1) {
        err = sum(value_1, value_2, result);
        if (err) {
            clean_decimal(result);
            err = NEGATIVE_INFINITY;
        }
        sign = 1;
    } else if (sign_1 == 0 && sign_2 == 0) {
        err = sum(value_1, value_2, result);
        if (err) {
            clean_decimal(result);
            err = MY_INFINITY;
        }
    } else if (sign_1 == 1 && sign_2 == 0) {
        if (is_greater_by_abc(value_1, value_2)) {
            flag = 1;
        }
        value_1 = negtive(value_1);
        sum(value_1, value_2, result);
        if (flag) {
            *result = negtive(*result);
            sign = 1;
        }
    } else if (sign_1 == 0 && sign_2 == 1) {
        if (is_greater_by_abc(value_2, value_1)) {
            flag = 1;
        }
        value_2 = negtive(value_2);
        sum(value_1, value_2, result);

        if (flag) {
            *result = negtive(*result);
            sign = 1;
        }
    }
    if (err == 0) {
        set_scale(result, scale);
        if (sign) {
            set_sign(&result->bits[SCALE]);
        }
    }
    return err;
}
int is_greater_by_abc(decimal val_1, decimal val_2) {
    int a, b, res = 0;
    for (int i = 95; i >= 0; i--) {
        a = get_bit(val_1, i);
        b = get_bit(val_2, i);

        if (a == b) continue;
        res = a > b ? 1 : 0;
        break;
    }
    return res;
}

int sub(decimal value_1, decimal value_2, decimal *result) {
    int err = 0;
    if (get_sign(value_1.bits[SCALE]) == 1 && get_sign(value_2.bits[SCALE]) == 1) {
        value_2.bits[SCALE] &= 0b01111111111111111111111111111111;
        err = add(value_1, value_2, result);
    } else if (get_sign(value_1.bits[SCALE]) == 0 && get_sign(value_2.bits[SCALE]) == 0) {
        set_sign(&value_2.bits[SCALE]);
        err = add(value_1, value_2, result);
    } else if (get_sign(value_1.bits[SCALE]) == 1 && get_sign(value_2.bits[SCALE]) == 0) {
        set_sign(&value_2.bits[SCALE]);
        err = add(value_1, value_2, result);
    } else if (get_sign(value_1.bits[SCALE]) == 0 && get_sign(value_2.bits[SCALE]) == 1) {
        value_2.bits[SCALE] &= 0b01111111111111111111111111111111;
        err = add(value_1, value_2, result);
    }
    return err;
}

int is_less_or_equal(decimal val_1, decimal val_2) {
    int x = is_equal(val_1, val_2);
    int y = is_less(val_1, val_2);
    return x || y;
}

int is_greater_or_equal(decimal val_1, decimal val_2) {
    int x = is_equal(val_1, val_2);
    int y = is_greater(val_1, val_2);
    return (x || y);
}

int is_not_equal(decimal a, decimal b) {
    return !is_equal(a, b);
}

int is_equal(decimal val_1, decimal val_2) {
    int s_a, s_b, res = 1;
    equal_scale(&val_1, &val_2);
    if (check_zero(val_1) && check_zero(val_2)) {
        res = 1;
    } else {
        s_a = get_sign(val_1.bits[SCALE]);
        s_b = get_sign(val_2.bits[SCALE]);
        if ((s_a ^ s_b) == 0) {
            for (int i = 0; i < 3; i++) {
                if (val_1.bits[i] != val_2.bits[i]) {
                    res = 0;
                }
            }
        } else {
            res = 0;
        }
    }

    return res;
}

int is_greater(decimal val_1, decimal val_2) {
    return is_less(val_2, val_1);
}

int is_less(decimal val_1, decimal val_2) {
    int a, b, res = 0;
    equal_scale(&val_1, &val_2);
    if (check_zero(val_1) && check_zero(val_2)) {
        res = 0;
    } else {
        if (get_sign(val_1.bits[SCALE]) > get_sign(val_2.bits[SCALE])) {
            res = 1;
        } else if (get_sign(val_1.bits[SCALE]) == 0 && get_sign(val_2.bits[SCALE]) == 0) {
            for (int i = 95; i >= 0; i--) {
                a = get_bit(val_1, i);
                b = get_bit(val_2, i);
                if (a == b) continue;
                res = a < b ? 1 : 0;
                break;
            }
        } else if (get_sign(val_1.bits[SCALE]) == 1 && get_sign(val_2.bits[SCALE]) == 1) {
            for (int i = 95; i >= 0; i--) {
                a = get_bit(val_1, i);
                b = get_bit(val_2, i);
                if (a == b) continue;
                res = a > b ? 1 : 0;
                break;
            }
        }
    }

    return res;
}

decimal negtive(decimal dec) {
    decimal negtive_dec, one_dec, result;
    clean_decimal(&negtive_dec);
    clean_decimal(&one_dec);
    one_dec.bits[LOW] = 0b1;
    for (int i = 0; i < 3; i++) {
        negtive_dec.bits[i] = ~dec.bits[i];
    }
    sum(negtive_dec, one_dec, &result);
    return result;
}

int sum(decimal value_1, decimal value_2, decimal *result) {
    clean_decimal(result);
    int a, b, res;
    int temp = 0, err = 0;
    for (int i = 0; i < 96; i++) {
        a = get_bit(value_1, i);
        b = get_bit(value_2, i);
        res = a + b + temp;
        if (i == 95 && res > 1) {
            err = MY_INFINITY;
            // break;
        }
        if (res == 0) {
            temp = 0;
        } else if (res == 1) {
            temp = 0;
            set_bit(result, i);
        } else if (res == 2) {
            temp = 1;
        } else if (res == 3) {
            temp = 1;
            set_bit(result, i);
        }
    }
    return err;
}

void shift_righ(decimal *dec) {
    int bit_64 = 0;
    int bit_32 = 0;
    bit_64 = get_bit(*dec, 64);
    bit_32 = get_bit(*dec, 32);
    dec->bits[HIGH] >>= 1;
    dec->bits[MID] >>= 1;
    if (bit_64) set_bit(dec, 63);
    dec->bits[LOW] >>= 1;
    if (bit_32) set_bit(dec, 31);
}

void shift_left(decimal *dec) {
    int bit_31 = 0;
    int bit_63 = 0;
    bit_31 = get_bit(*dec, 31);
    bit_63 = get_bit(*dec, 63);
    dec->bits[LOW] <<= 1;
    dec->bits[MID] <<= 1;
    dec->bits[MID] += bit_31;
    dec->bits[HIGH] <<= 1;
    dec->bits[HIGH] += bit_63;
}

int get_sign(int val) {
    return (val & 0x80000000) ? 1 : 0;
}

void set_sign(unsigned int *val) {
    *val |= 0x80000000;
}

void set_minus_sign(unsigned int *val) {
    *val &= 0x7FFFFFFF;
}

int get_scale(decimal *dec) {
    int mask = 0;
    mask = dec->bits[SCALE] & 0xFF0000;
    return mask >>= 16;
}

void set_scale(decimal *dec, int scale) {
    scale <<= 16;
    dec->bits[SCALE] = scale;
}

int get_exp_float(float src) {
    unsigned int mask = 0x7F800000;
    unsigned int src2 = *((unsigned int *)&src);
    mask &= src2;
    mask >>= 23;
    return mask - 127;
}

int from_int_to_decimal(int src, decimal *dst) {
    int err = 0;
    clean_decimal(dst);
    if (src == -MY_INFINITY || src == MY_INFINITY || src == MY_NAN ||
        src > INT_MAX || src < INT_MIN) {
        err = 1;
    } else {
        if (src < 0) {
            src = ~src + 1;
            set_sign(&dst->bits[SCALE]);
        }
        dst->bits[LOW] = src;
    }
    return err;
}

int from_decimal_to_int(decimal src, int *dst) {
    int err = 0;
    int sign = 0;
    int scale = 0;

    scale = get_scale(&src);
    sign = get_sign(src.bits[SCALE]);
    while (scale > 0) {
        lower_scale(&src);
        scale = get_scale(&src);
    }
    *dst = src.bits[LOW];
    if (sign) *dst = -*dst;

    return err;
}

int set_bit_to_int(int number, int index) {
    int mask = 1 << index;
    return number |= mask;
}

int set_bit(decimal *dec, int index) {
    int err = 0;
    if (index >= 0 && index < 96) {
        int mask = 1 << index;
        dec->bits[index / 32] |= mask;
    } else {
        err = 1;
    }
    return err;
}

int get_bit(decimal dec, int index) {
    int res = -1;
    if (index >= 0 && index < 128) {
        int mask = 1 << index;
        res = !!(dec.bits[index / 32] & mask);
    }
    return res;
}

decimal div_no_scale(decimal dividend, decimal divisor) {
    decimal quotient;
    decimal dec_null;
    clean_decimal(&dec_null);
    dividend.bits[SCALE] = 0;
    if (is_equal(dividend, divisor)) {
        clean_decimal(&quotient);
        quotient.bits[LOW] = 1;
    } else if (is_equal(dividend, dec_null)) {
        clean_decimal(&quotient);
    } else {
        clean_decimal(&quotient);
        decimal mask;
        clean_decimal(&mask);
        mask.bits[LOW] = 1;
        decimal one;
        clean_decimal(&one);
        one.bits[LOW] = 1;
        decimal temp;
        copy_decimal(&temp, &divisor);
        decimal t_divisor;
        clean_decimal(&t_divisor);
        decimal ten;
        clean_decimal(&ten);
        ten.bits[LOW] = 10;
        int flag = 0;
        while (is_greater_or_equal(dividend, divisor)) {
            copy_decimal(&t_divisor, &divisor);
            shift_left(&t_divisor);
            if (get_bit(t_divisor, 95)) flag = 1;
            if (is_greater_or_equal(dividend, t_divisor) &&
                is_greater(t_divisor, dec_null) && flag == 0) {
                shift_left(&divisor);
                shift_left(&mask);
            } else {
                sum(quotient, mask, &quotient);
                sub(dividend, divisor, &dividend);
                copy_decimal(&divisor, &temp);
                clean_decimal(&mask);
                mask.bits[LOW] = 1;
                flag = 0;
            }
        }
    }
    return quotient;
}

void copy_decimal(decimal *dest, decimal *src) {
    dest->bits[LOW] = src->bits[LOW];
    dest->bits[MID] = src->bits[MID];
    dest->bits[HIGH] = src->bits[HIGH];
    dest->bits[SCALE] = src->bits[SCALE];
}

int check_zero(decimal val) {
    int flag = 1;
    for (int i = 0; i < 3; i++) {
        if (val.bits[i] != 0) {
            flag = 0;
            break;
        }
    }
    if (flag == 1) {
        val.bits[3] = (val.bits[3] & 0b01111111111111111111111111111111);
    }
    return flag;
}

int truncate(decimal value, decimal *result) {
    int err = 0;
    int res_scale = get_scale(&value);
    if (res_scale < 0 || res_scale > 28) {
        err = 1;
    } else {
        while (res_scale > 0) {
            lower_scale(&value);
            res_scale = get_scale(&value);
        }
        copy_decimal(result, &value);
    }
    return err;
}

int my_round(decimal value, decimal *result) {
    int err = 0;
    if (result == NULL) {
        err = 1;
    } else {
        clean_decimal(result);
        int scale = 0;
        int sign = 0;
        scale = get_scale(&value);
        sign = get_sign(value.bits[SCALE]);
        if (scale > 0) {
            while (scale > 1) {
                lower_scale(&value);
                scale = get_scale(&value);
            }
            decimal five;
            clean_decimal(&five);
            five.bits[LOW] = 5;
            sum(value, five, &value);
            set_scale(&value, scale);
            lower_scale(&value);
        }
        copy_decimal(result, &value);
        if (sign) set_sign(&result->bits[SCALE]);
    }
    return err;
}

int negate(decimal value, decimal *result) {
    int err = 0;
    if (result == NULL) {
        err = 1;
    } else {
        int sign = 0;
        sign = get_sign(value.bits[SCALE]);
        if (sign) {
            set_minus_sign(&value.bits[SCALE]);
        } else {
            set_sign(&value.bits[SCALE]);
        }
        copy_decimal(result, &value);
    }
    return err;
}
int my_floor(decimal value, decimal *result) {
    int err = 0;
    if (result == NULL) {
        err = 1;
    } else {
        int scale = 0;
        scale = get_scale(&value);
        int sign = 0;
        sign = get_sign(value.bits[SCALE]);
        if (scale > 0 && !check_zero(value)) {
            truncate(value, result);
            if (sign) {
                decimal one;
                clean_decimal(&one);
                one.bits[LOW] = 1;
                one.bits[SCALE] = 0x80000000;
                add(*result, one, result);
            }
        } else {
            set_scale(&value, 0);
            copy_decimal(result, &value);
            if (sign) set_sign(&result->bits[SCALE]);
        }
    }
    return err;
}
int mul(decimal value_1, decimal value_2, decimal *result) {
    int err = NORMAL_VALUE;
    int sign_1 = 0;
    sign_1 = get_sign(value_1.bits[SCALE]);
    int sign_2 = 0;
    sign_2 = get_sign(value_2.bits[SCALE]);
    if ((check_zero(value_1) == 1 && value_1.bits[SCALE] == 0) ||
    (check_zero(value_2) == 1 && value_2.bits[SCALE] == 0)) {
        clean_decimal(result);
        if (sign_1 == 1 && sign_2 == 0) {
            set_sign(&result->bits[SCALE]);
        } else if (sign_1 == 0 && sign_2 == 1) {
            set_sign(&result->bits[SCALE]);
        }
    } else {
        clean_decimal(result);
        int scale = 0;
        scale = get_scale(&value_1) + get_scale(&value_2);
        big_decimal v1, v2, res;
        clean_big_decimal(&v1);
        clean_big_decimal(&v2);
        clean_big_decimal(&res);
        decimal_to_big_decimal(&v1, &value_1);
        decimal_to_big_decimal(&v2, &value_2);
        while (v2.bits[0] != 0 || v2.bits[1] != 0 || v2.bits[2] != 0 ||
        v2.bits[3] != 0 || v2.bits[4] != 0 || v2.bits[5] != 0) {
            if (v2.bits[0] & 1) {
                res = sum_big_decimal(v1, res);
            }
            shift_righ_big(&v2);
            shift_left_big(&v1);
        }
        big_decimal five;
        clean_big_decimal(&five);
        five.bits[0] = 5;
        while ((res.bits[3] != 0 || res.bits[4] != 0 || res.bits[5] != 0) || scale > 28) {
            res = sum_big_decimal(res, five);
            res = div_big_on_ten(res);
            scale--;
        }
        if ((res.bits[3] == 0 && res.bits[4] == 0 && res.bits[5] == 0) && (scale >= 0 && scale <= 28)) {
            big_to_decimal(result, &res);
            set_scale(result, scale);
            if (sign_1 == 1 && sign_2 == 0) {
                set_sign(&result->bits[SCALE]);
            } else if (sign_1 == 0 && sign_2 == 1) {
                set_sign(&result->bits[SCALE]);
            }
        } else {
            if (sign_1 == 1 && sign_2 == 0) {
                err = NEGATIVE_INFINITY;
            } else if (sign_1 == 0 && sign_2 == 1) {
                err = NEGATIVE_INFINITY;
            } else {
                err = MY_INFINITY;
            }
            clean_decimal(result);
        }
    }
    return err;
}

void copy_big(big_decimal *big_dst, big_decimal *big_src) {
    for (int i = 0; i < 6; ++i) {
        big_dst->bits[i] = big_src->bits[i];
    }
}

int is_greater_or_equal_big(big_decimal big1, big_decimal big2) {
    int res = 0;
    if (big1.bits[0] == big2.bits[0] && big1.bits[1] == big2.bits[1] &&
        big1.bits[2] == big2.bits[2] && big1.bits[3] == big2.bits[3] &&
        big1.bits[4] == big2.bits[4] && big1.bits[5] == big2.bits[5]) {
        res = 1;
    } else {
        int a = 0, b = 0;
        for (int i = 191; i >= 0; --i) {
            a = get_bit_big(big1, i);
            b = get_bit_big(big2, i);
            if (a == b) continue;
            res = a > b ? 1 : 0;
            break;
        }
    }
    return res;
}

void big_dop_code(big_decimal *big) {
    for (int i = 0; i < 6; ++i) {
        big->bits[i] = ~big->bits[i];
    }
    big_decimal one;
    clean_big_decimal(&one);
    one.bits[0] = 1;
    *big = sum_big_decimal(*big, one);
}

big_decimal div_big_on_ten(big_decimal big1) {
    big_decimal ten, result, mask, temp;
    clean_big_decimal(&ten);
    ten.bits[0] = 10;
    clean_big_decimal(&result);
    clean_big_decimal(&mask);
    mask.bits[0] = 1;
    clean_big_decimal(&temp);
    int flag = 0;
    while (is_greater_or_equal_big(big1, ten)) {
        copy_big(&temp, &ten);
        shift_left_big(&temp);
        if (get_bit_big(temp, 191)) flag = 1;
        if (is_greater_or_equal_big(big1, temp) && flag == 0) {
            shift_left_big(&ten);
            shift_left_big(&mask);
        } else {
            result = sum_big_decimal(result, mask);
            big_dop_code(&ten);
            big1 = sum_big_decimal(big1, ten);
            clean_big_decimal(&ten);
            ten.bits[0] = 10;
            clean_big_decimal(&mask);
            mask.bits[0] = 1;
            flag = 0;
        }
    }
    return result;
}

void shift_righ_big(big_decimal *big) {
    int bit_160 = 0;
    int bit_128 = 0;
    int bit_96 = 0;
    int bit_64 = 0;
    int bit_32 = 0;
    bit_160 = get_bit_big(*big, 160);
    bit_128 = get_bit_big(*big, 128);
    bit_96 = get_bit_big(*big, 96);
    bit_64 = get_bit_big(*big, 64);
    bit_32 = get_bit_big(*big, 32);
    big->bits[5] >>= 1;
    big->bits[4] >>= 1;
    if (bit_160) set_bit_big(big, 159);
    big->bits[3] >>= 1;
    if (bit_128) set_bit_big(big, 127);
    big->bits[2] >>= 1;
    if (bit_96) set_bit_big(big, 95);
    big->bits[1] >>= 1;
    if (bit_64) set_bit_big(big, 63);
    big->bits[0] >>= 1;
    if (bit_32) set_bit_big(big, 31);
}

void shift_left_big(big_decimal *big) {
    int bit_31 = 0;
    int bit_63 = 0;
    int bit_95 = 0;
    int bit_127 = 0;
    int bit_159 = 0;
    bit_31 = get_bit_big(*big, 31);
    bit_63 = get_bit_big(*big, 63);
    bit_95 = get_bit_big(*big, 95);
    bit_127 = get_bit_big(*big, 127);
    bit_159 = get_bit_big(*big, 159);
    big->bits[0] <<= 1;
    big->bits[1] <<= 1;
    big->bits[1] += bit_31;
    big->bits[2] <<= 1;
    big->bits[2] += bit_63;
    big->bits[3] <<= 1;
    big->bits[3] += bit_95;
    big->bits[4] <<= 1;
    big->bits[4] += bit_127;
    big->bits[5] <<= 1;
    big->bits[5] += bit_159;
}

big_decimal sum_big_decimal(big_decimal big1, big_decimal big2) {
    int a, b, res, temp;
    big_decimal result;
    clean_big_decimal(&result);
    temp = 0;
    for (int i = 0; i < 192; i++) {
        a = get_bit_big(big1, i);
        b = get_bit_big(big2, i);
        res = a + b + temp;
        if (res == 0) {
            temp = 0;
        } else if (res == 1) {
            temp = 0;
            set_bit_big(&result, i);
        } else if (res == 2) {
            temp = 1;
        } else if (res == 3) {
            temp = 1;
            set_bit_big(&result, i);
        }
    }
    return result;
}

void set_bit_big(big_decimal *big, int index) {
    int mask = 1 << index;
    big->bits[index / 32] |= mask;
}

int get_bit_big(big_decimal big, int index) {
    int res = -1;
    int mask = 1 << index;
    res = !!(big.bits[index / 32] & mask);
    return res;
}

void decimal_to_big_decimal(big_decimal *dst, decimal *src) {
    for (int i = 0; i < 3; ++i) {
        dst->bits[i] = src->bits[i];
    }
}

void big_to_decimal(decimal *dst, big_decimal *src) {
    for (int i = 0; i < 3; ++i) {
        dst->bits[i] = src->bits[i];
    }
}

void clean_big_decimal(big_decimal *big) {
    for (int i = 0; i < 6; i++) {
        big->bits[i] = 0;
    }
}

int mod(decimal value_1, decimal value_2, decimal *result) {
    int err = NORMAL_VALUE;
    clean_decimal(result);
    if (check_zero(value_2)) {
        err = MY_NAN;
    } else {
        int sign_1 = 0;
        sign_1 = get_sign(value_1.bits[SCALE]);
        set_minus_sign(&value_1.bits[SCALE]);
        set_minus_sign(&value_2.bits[SCALE]);
        decimal n;
        clean_decimal(&n);
        n = div_no_scale(value_1, value_2);
        mul(n, value_2, &value_2);
        sub(value_1, value_2, result);
        if (sign_1) set_sign(&result->bits[SCALE]);
    }
    return err;
}

int from_float_to_decimal(float src, decimal *dst) {
    int err = 0;
    int sign = 0;
    if (src < 0) {
        sign = 1;
        src = -src;
    }
    if (dst == NULL || src == MY_NAN || src == MY_INFINITY || src == -MY_INFINITY) {
        err = 1;
        clean_decimal(dst);
    } else if (src == 0.0) {
        clean_decimal(dst);
    } else {
        clean_decimal(dst);
        int exp = get_exp_float(src);
        if (exp > 95 || exp < -94) {
            err = 1;
        } else {
            int scale = 0;
            int scale_2 = 0;
            double src2 = src;
            for (; !(int)src2; src2 *= 10, ++scale) {
            }
            for (; src2 > 10; src2 /= 10, scale--) {
            }
            for (int i = 0; i < 6; ++i, ++scale_2) {
                src2 = src2 * 10.0;
            }
            src = src2;
            if (scale > 28) {
                err = 1;
            } else {
                long int mask = 1;
                while ((unsigned int)src % 10 == 0) {
                    src /= 10;
                    scale_2--;
                }
                long int src_tmp = src;
                for (int i = 0; i < 96; ++i, mask <<= 1) {
                    if (!!(mask & src_tmp)) set_bit(dst, i);
                }
                if (scale + scale_2 < 0) {
                    int temp = 0;
                    from_decimal_to_int(*dst, &temp);
                    for (; scale + scale_2 < 0; scale++) {
                        temp *= 10;
                    }
                    from_int_to_decimal(temp, dst);
                }
                set_scale(dst, scale + scale_2);
                if (sign) set_sign(&dst->bits[SCALE]);
            }
        }
    }
    if (sign) set_sign(&dst->bits[SCALE]);
    return err;
}

int from_decimal_to_float(decimal src, float *dst) {
    int err = 0;
    int sign = 0;
    sign = get_sign(src.bits[SCALE]);
    set_minus_sign(&src.bits[SCALE]);
    int scale = 0;
    scale = get_scale(&src);
    *dst = 0.0;
    double dst2 = *dst;
    for (int i = 0; i < 96; ++i) {
        if (get_bit(src, i)) {
            dst2 += pow(2., (double)i);
        }
    }
    while (scale > 0) {
        dst2 = dst2 / 10.;
        scale--;
    }
    *dst = dst2;
    if (sign) *dst = -*dst;
    return err;
}

void clean_decimal(decimal *dec) {
    for (int i = 0; i < 4; i++) {
        dec->bits[i] = 0;
    }
}
