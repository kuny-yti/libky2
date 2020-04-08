#include "maths/ky_bigint.h"
#include "ky_maths.h"

#define PLUS 1
#define MINUS -1

int subtract(int32 *num1, int32 len1, int32 *num2, int32 len2);

_bigint_::_bigint_():value()
{
    sign = PLUS;
}

_bigint_::_bigint_(const _bigint_& num):value()
{
    value = num.value;
    sign = num.sign;
}

_bigint_::_bigint_(const longlong& num):value()
{
    longlong tmp = num;

    sign = PLUS;
    if (tmp < 0)
    {
        sign = MINUS;
        tmp = -tmp;
    }

    while (tmp > 0)
    {
        value.append(tmp % 10);
        tmp /= 10;
    }
    trimmed();
}

_bigint_::_bigint_(const ky_string &num):value()
{
    if (num.is_empty())
        return;

    ky_string tmp = num;
    bool is_pn = tmp[0].is_digit();
    sign = PLUS;
    if (!is_pn)
    {
        if (tmp[0] == '-')
            sign = MINUS;
        else if (tmp[0] != '+')
            return;
        tmp = tmp.extract(1);
    }
    if (!tmp.is_number())
        return;
    for (int32 i = tmp.size()-1; i >=0; --i)
        value.append(tmp[i] - '0');

    trimmed();
}

_bigint_& _bigint_::operator=(const _bigint_& num)
{
    value = num.value;
    sign = num.sign;
    return *this;
}

_bigint_& _bigint_::operator=(const longlong& num)
{
    _bigint_ temp(num);
    value = temp.value;
    sign = temp.sign;
    return *this;
}

_bigint_& _bigint_::operator=(const ky_string& num)
{
    _bigint_ temp(num);
    value = temp.value;
    sign = temp.sign;
    return *this;
}

ky_string _bigint_::to_string() const
{
    if (value.is_empty())
        return ky_string("0");

    ky_string r;
    for (int32 i = value.size()-1; i >= 0; --i)
    {
        char buf[2] = {(char)(value[i] + '0'), '\0'};
        r.append(buf, 1);
    }
    int32 i = 0;
    for (; i < r.count(); ++i)
        if (r[i] != '0')
            break;

    return (sign == MINUS) ? (ky_string("-") + r.extract(i)) : r.extract(i);
}

int _bigint_::to_int() const
{
    return to_longlong();
}

long _bigint_::to_long() const
{
    return to_longlong();
}

longlong _bigint_::to_longlong() const
{
    if (value.is_empty())
        return 0;
    ulonglong r = 0;
    ulonglong pow1 = 1;
    for (int32 i = 0; i < value.size(); ++i)
    {
        r += value[i] * pow1;
        pow1 *= 10;
    }
    return (sign == MINUS) ? -r : r;
}

_bigint_ _bigint_::operator+() const
{
    _bigint_ r = *this;
    r.sign = PLUS;
    return r;
}
_bigint_ _bigint_::operator-() const
{
    _bigint_ r = *this;
    r.sign = (r.sign == MINUS) ? PLUS : MINUS;
    return r;
}

_bigint_ _bigint_::operator+(const _bigint_ &n2) const
{
    _bigint_ result;
    if (sign == n2.sign)
        result.sign = sign;
    else
    {
        if (sign == MINUS)
        {
            _bigint_ sn = *this;
            sn.sign = PLUS;
            return n2 - sn;
        }
        else
        {
            _bigint_ sn = n2;
            sn.sign = PLUS;
            return *this - sn;
        }
    }

    uint8 *b1 = value.data();
    uint8 *e1 = value.data() + value.size();
    uint8 *o1 = b1;
    uint8 *b2 = n2.value.data();
    uint8 *e2 = n2.value.data() + n2.value.size();
    uint8 *o2 = b2;

    int16 carry = 0, sum;
    bool is_v1 = (o1 < e1), is_v2 = (o2 < e2);
    for ( ;is_v1 || is_v2; ++o1, ++o2,
                           is_v1 = (o1 < e1),
                           is_v2 = (o2 < e2))
    {
        uint8 x1 = (is_v1 ? *o1 : 0);
        uint8 x2 = (is_v2 ? *o2 : 0);

        sum = x1 + x2 + carry;
        carry = sum / 10;

        result.value.append(sum % 10);
    }
    if (carry)
        result.value.append(carry);

    result.trimmed();
    return result;
}
_bigint_ _bigint_::operator-(const _bigint_ &n2) const
{
    if (sign == MINUS || n2.sign == MINUS)
    {
        _bigint_ nh = n2;
        nh.sign = MINUS * nh.sign;
        return *this + nh;
    }

    if (compare(n2) < 0)
    {
        _bigint_ nh = n2 - *this;
        nh.sign = MINUS;
        return nh;
    }

    _bigint_ result;
    uint8 *b1 = value.data();
    uint8 *e1 = value.data() + value.size();
    uint8 *o1 = b1;
    uint8 *b2 = n2.value.data();
    uint8 *e2 = n2.value.data() + n2.value.size();
    uint8 *o2 = b2;
    int16 borrow = 0, x1 = 0, x2 = 0;

    bool is_v1 = (o1 < e1), is_v2 = (o2 < e2);
    for ( ;is_v1 || is_v2; ++o1, ++o2,
                           is_v1 = (o1 < e1),
                           is_v2 = (o2 < e2))
    {
        x1 = (is_v1 ? *o1 : 0);
        x2 = (is_v2 ? *o2 : 0);
        x1 = x1 - x2 - borrow;
        borrow = (x1 < 0) ? 1 : 0;
        if (borrow)
            x1 += 10;

        result.value.append(x1 % 10);
    }
    result.trimmed();
    return result;
}
_bigint_ _bigint_::operator*(const _bigint_ &n2) const
{
    if (value.is_empty() || n2.value.is_empty())
        return _bigint_();
    if (*this == 1)
        return n2;
    if (n2 == 1)
        return *this;

    _bigint_ result;
    uint8 *b1 = value.data();
    uint8 *e1 = value.data() + value.size();
    uint8 *o1 = b1;
    uint8 *b2 = n2.value.data();
    uint8 *e2 = n2.value.data() + n2.value.size();
    uint8 *o2 = b2;

    int32 bufsize = value.size() + n2.value.size() + 5;
    int32 tmpbuf[bufsize];
    ky_memory::zero(tmpbuf, sizeof(tmpbuf));

    int32 i = 0, j = 0;
    while (o1 < e1)
    {
        int32 x1 = *o1++;
        while (o2 < e2)
        {
            int32 x2 = *o2++;
            tmpbuf[i + j] += x2 * x1;
            j++;
        }
        i++;
        o2 = b2;
        j = 0;
    }

    for(int32 i = 0; i < bufsize - 1; ++i)
    {
        tmpbuf[i+1] = tmpbuf[i+1] + tmpbuf[i] / 10;
        tmpbuf[i] = tmpbuf[i] % 10;
    }
    for (i = bufsize - 1; i >= 0; --i)
    {
        if (!tmpbuf[i])
            tmpbuf[i] = -1;
        else
            break;
    }

    for (i = 0; i < bufsize-1; ++i)
    {
        if (tmpbuf[i] == -1)
            break;
        result.value.append(tmpbuf[i] % 10);
    }

    result.sign = sign * n2.sign;
    result.trimmed();
    return result;
}

_bigint_ _bigint_::operator/(const _bigint_ &n2) const
{
    int32 len1 = value.size();
    int32 len2 = n2.value.size();

    if (len1 < len2)
        return _bigint_();

    _bigint_ result;
    uint8 *b1 = value.data();
    uint8 *e1 = value.data() + len1;
    uint8 *o1 = b1;
    uint8 *b2 = n2.value.data();
    uint8 *e2 = n2.value.data() + len2;
    uint8 *o2 = b2;

    const int32 lenmax = ky_max(len1, len2);
    const int32 ofd = len1 - len2;

    int32 num1[lenmax];
    int32 num2[lenmax];
    int32 ans[lenmax];
    ky_memory::zero(num1, sizeof(num1));
    ky_memory::zero(num2, sizeof(num2));
    ky_memory::zero(ans, sizeof(ans));

    for (int32 i = 0; o1 < e1; ++i, ++o2, ++o1)
    {
        if (o2 < e2)
            num2[i + ofd] = *o2;
        num1[i] = *o1;
    }

    len2 = len1;
    for (int32 i = 0; i <= ofd; i++)
    {
        int32 count = 0;
        while ((count = subtract(num1, len1, num2 +i, len2 - i)) >= 0)
        {
            ans[ofd - i] ++;
            len1 = count;
        }
    }

    for (int32 i = lenmax-1; i >= 0; --i)
    {
        if (ans[i] == 0)
            ans[i] = -1;
        else
            break;
    }
    for (int32 i = 0; i < lenmax; ++i)
    {
        if (ans[i] == -1)
            break;
        result.value.append(ans[i] % 10);
    }
    result.trimmed();
    result.sign = sign * n2.sign;
    return result;
}

_bigint_ _bigint_::operator%(const _bigint_ &n2) const
{
    _bigint_ divTmp = *this / n2;
    return (*this - n2 * divTmp);
}

_bigint_ _bigint_::operator+(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this + nh;
}
_bigint_ _bigint_::operator-(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this - nh;
}
_bigint_ _bigint_::operator*(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this * nh;
}
_bigint_ _bigint_::operator/(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this / nh;
}
_bigint_ _bigint_::operator%(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this % nh;
}

_bigint_ _bigint_::operator+(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this + nh;
}
_bigint_ _bigint_::operator-(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this - nh;
}
_bigint_ _bigint_::operator*(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this * nh;
}
_bigint_ _bigint_::operator/(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this / nh;
}
_bigint_ _bigint_::operator%(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this % nh;
}

bool _bigint_::operator<(const _bigint_ &n2) const
{
    return (compare(n2) < 0);
}
bool _bigint_::operator>(const _bigint_ &n2) const
{
    return (compare(n2) > 0);
}
bool _bigint_::operator<=(const _bigint_ &n2) const
{
    return (compare(n2) <= 0);
}
bool _bigint_::operator>=(const _bigint_ &n2) const
{
    return (compare(n2) >= 0);
}
bool _bigint_::operator==(const _bigint_ &n2) const
{
    return (compare(n2) == 0);
}
bool _bigint_::operator!=(const _bigint_ &n2) const
{
    return (compare(n2) != 0);
}
bool _bigint_::operator<(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this < nh;
}
bool _bigint_::operator>(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this > nh;
}
bool _bigint_::operator<=(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this <= nh;
}
bool _bigint_::operator>=(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this >= nh;
}
bool _bigint_::operator==(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this == nh;
}
bool _bigint_::operator!=(const longlong &n2) const
{
    _bigint_ nh(n2);
    return *this != nh;
}
bool _bigint_::operator<(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this < nh;
}
bool _bigint_::operator>(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this > nh;
}
bool _bigint_::operator<=(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this <= nh;
}
bool _bigint_::operator>=(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this >= nh;
}
bool _bigint_::operator==(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this == nh;
}
bool _bigint_::operator!=(const ky_string &n2) const
{
    _bigint_ nh(n2);
    return *this != nh;
}
int _bigint_::compare(const _bigint_&n2)const
{
    if (sign == MINUS && n2.sign == PLUS)
        return MINUS;
    if (sign == PLUS && n2.sign == MINUS)
        return PLUS;

    _bigint_ v1 = *this;
    _bigint_ v2 = n2;
    v1.trimmed();
    v2.trimmed();

    if (v2.value.size() > v1.value.size())
        return (MINUS * v1.sign);
    if (v1.value.size() > v2.value.size())
        return (PLUS * v1.sign);

    for (int32 i = v1.value.size()-1; i >= 0; --i)
    {
        if (v1.value[i] > v2.value[i])
            return (PLUS * v1.sign);
        if (v2.value[i] > v1.value[i])
            return (MINUS * v1.sign);
    }

    return (0);
}
/* multiply n by 10^d */
_bigint_ &_bigint_::pow10(int32 const &d)
{
    if (value.is_empty() || value[0] == 0)
        return *this;
    value.expand(d);

    for (int32 i = value.size() -1 -d; i >= 0; --i)
        value[i + d] = value[i];

    for (int32 i=0; i < d; i++)
        value[i] = 0;
    return *this;
}
void _bigint_::trimmed()
{
    int32 pos = value.size() -1;
    while (pos >= 0)
    {
        if (value[pos] != 0)
            break;
         --pos;
    }
    ++pos;
    if (pos == value.size())
        return ;

    if (pos == 0)
        value = ky_byte();
    else
        value.remove(pos, value.size() - pos);
}
_bigint_ ky_abs(const _bigint_ &d)
{
    _bigint_ r = d;
    r.sign = PLUS;
    return r;
}

_bigint_ ky_pow(const _bigint_ &d, const int32 &exp)
{
    if (exp < 0) {
        if (d == 0)
            throw std::logic_error("Cannot divide by zero");
        return ky_abs(d) == 1 ? d : _bigint_();
    }
    if (exp == 0) {
        if (d == 0)
            throw std::logic_error("Zero cannot be raised to zero");
        return _bigint_(1);
    }

    int32 two = (exp);
    _bigint_ result = d, result_odd = 1;
    while (two > 1)
    {
        if (two % 2)
            result_odd *= result;
        result *= result;
        two /= 2;
    }

    return result * result_odd;
}

_bigint_ ky_sqrt(const _bigint_& num)
{
    if (num < 0)
        throw std::invalid_argument("Cannot compute square root of a negative integer");

    if (num == 0)
        return _bigint_(0);
    else if (num < 4)
        return _bigint_(1);
    else if (num < 9)
        return _bigint_(2);
    else if (num < 16)
        return _bigint_(3);

    _bigint_ sqrt_prev(-1);
    _bigint_ sqrt_current(1);
    sqrt_current.pow10(num.value.size() / 2 -1);
    //_bigint_ sqrt_current = big_pow10(num.to_string().size() / 2 - 1);

    while (ky_abs(sqrt_current - sqrt_prev) > 1)
    {
        sqrt_prev = sqrt_current;
        sqrt_current = (num / sqrt_prev + sqrt_prev) / 2;
    }

    return sqrt_current;
}

_bigint_ ky_gcd(const _bigint_ &num1, const _bigint_ &num2)
{
    _bigint_ abs_num1 = ky_abs(num1);
    _bigint_ abs_num2 = ky_abs(num2);

    /*
    if ((abs_num1 % abs_num2) ==0)
        return abs_num2;
    return ky_gcd(abs_num2, abs_num1 % abs_num2);*/

    _bigint_ remainder = abs_num2;
    while (remainder != 0)
    {
        remainder = abs_num1 % abs_num2;
        abs_num1 = abs_num2;
        abs_num2 = remainder;
    }
    return abs_num1;
}

_bigint_ ky_lcm(const _bigint_& num1, const _bigint_& num2)
{
    if (num1 == 0 || num2 == 0)
        return _bigint_(0);

    return ky_abs(num1 * num2) / ky_gcd(num1, num2);
}

int subtract(int32 *num1, int32 len1, int32 *num2, int32 len2)
{
    if(len1 < len2)
        return -1;

    if(len1 == len2)
    {
        for(int32 i = len1 -1; i >= 0; --i)
        {
            if(num1[i] > num2[i])
                break;
            if(num1[i] < num2[i])
                return -1;
        }
    }

    for(int32 i = 0; i < len2; ++i)
    {
        num1[i] -= num2[i];
        if (num1[i] < 0)
        {
            num1[i] += 10;
            num1[i+1]--;
        }
    }
    for (int32 i = len1 - 1; i >= 0; i--) {
        if (num1[i])
            return i + 1;
    }
    return 0;
}
