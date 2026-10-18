// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"
#include <stdexcept>

// Fake variables used as placeholders in tests
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);

TBitField::TBitField(int len)
{
    if (len < 0) {
      throw std::invalid_argument("bitfield length cannot be negative");
    }

    BitLen = len;
    MemLen = len / (sizeof(TELEM) * 8);
    if (len % (sizeof(TELEM) * 8) != 0) {
      ++MemLen;
    }
    pMem = new TELEM[MemLen]();
}

TBitField::TBitField(const TBitField &bf):BitLen(bf.BitLen), MemLen(bf.MemLen) // конструктор копирования
{
    pMem = new TELEM[MemLen]{};
    for (int i{}; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }
}

TBitField::~TBitField()
{
    delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    return n / (sizeof(TELEM) * 8);
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    return static_cast<TELEM>(1) << (n % (sizeof(TELEM) * 8));
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
    return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n < 0 || n >= BitLen) {
      throw std::out_of_range("bit index is outside the bitfield");
    }
    pMem[GetMemIndex(n)] |= GetMemMask(n);
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if (n < 0 || n >= BitLen) {
      throw std::out_of_range("bit index is outside the bitfield");
    }
    pMem[GetMemIndex(n)] &= ~GetMemMask(n);
}

int TBitField::GetBit(const int n) const // получить значение бита
{
    if (n < 0 || n >= BitLen) {
      throw std::out_of_range("bit index is outside the bitfield");
    }
    return (pMem[GetMemIndex(n)] & GetMemMask(n)) != 0;
}

// битовые операции

TBitField& TBitField::operator=(const TBitField& bf)
{
    if (this == &bf) {
        return *this;
    }

    TELEM* newMem = new TELEM[bf.MemLen];

    for (int i = 0; i < bf.MemLen; ++i) {
        newMem[i] = bf.pMem[i];
    }

    delete[] pMem;

    pMem = newMem;
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;

    return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение
{
    if (BitLen != bf.BitLen) return false;
    else {
        for (int i = 0; i < MemLen; i++) {
            if (pMem[i] != bf.pMem[i]) return 0;
        }
    }
    return 1;
}

int TBitField::operator!=(const TBitField &bf) const // сравнение
{
    return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField &bf) // операция "или"
{
    const int maxBitLen = BitLen > bf.BitLen ? BitLen : bf.BitLen;
    TBitField tmp(maxBitLen);
    for (int i = 0; i < tmp.MemLen; ++i) {
        const TELEM left = i < MemLen ? pMem[i] : 0;
        const TELEM right = i < bf.MemLen ? bf.pMem[i] : 0;
        tmp.pMem[i] = left | right;
    }
    return tmp;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
    const int maxBitLen = BitLen > bf.BitLen ? BitLen : bf.BitLen;
    TBitField tmp(maxBitLen);
    const int commonWords = MemLen < bf.MemLen ? MemLen : bf.MemLen;
    for (int i = 0; i < commonWords; ++i) {
        tmp.pMem[i] = pMem[i] & bf.pMem[i];
    }
    return tmp;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField tmp(*this);
    for (int i{}; i < tmp.MemLen; i++) {
        tmp.pMem[i] = ~tmp.pMem[i];
    }
    const int remainingBits = BitLen % (sizeof(TELEM) * 8);
    if (remainingBits != 0) {
        const TELEM validBitsMask = (static_cast<TELEM>(1) << remainingBits) - 1;
        tmp.pMem[tmp.MemLen - 1] &= validBitsMask;
    }
    return tmp;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{
    int i = 0;
    char ch;
    while (i < bf.GetLength() && istr.get(ch)) {
        //istr >> ch;
        if (ch == '0') bf.ClrBit(i++);
        else if (ch == '1') bf.SetBit(i++);
        else {
            istr.putback(ch);
            break;
        }
    }
    return istr;
}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
    for (int i = 0;i < bf.GetLength();(bf.GetBit(i++)) ? ostr << '1' : ostr << '0');
    return ostr;
}
