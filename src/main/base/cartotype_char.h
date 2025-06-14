/*
cartotype_char.h
Copyright (C) 2004-2022 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_types.h>

namespace CartoTypeCore
{

/** The case of letters and strings in Latin, Greek, Cyrillic and other alphabetic scripts. */
enum class LetterCase
    {
    /** A code used when the letter case is to be left as it is, or is irrelevant. */
    None,
    /** The case of the letters 'a', 'b', c', etc. */
    Lower,
    /** The case of the letters 'A', 'B', 'C, etc. */
    Upper,
    /**
    The case of letters like U+01C8 'Lj' and a few others; also indicates
    that the first letter of each word should be changed to titlecase.
    */
    Title
    };

/**
Flags for the general category of a character as defined in the Unicode Standard
(http://www.unicode.org/versions/Unicode5.0.0/ch04.pdf).
Each constant is a single bit so that masks can be made by combining them.
*/
namespace CharCategoryFlag
    {
    constexpr uint32_t Lu = 1;              ///< A bit flag for the Unicode general character property Letter, uppercase.
    constexpr uint32_t Ll = 2;              ///< A bit flag for the Unicode general character property Letter, lowercase.
    constexpr uint32_t Lt = 4;              ///< A bit flag for the Unicode general character property Letter, titlecase.
    constexpr uint32_t Lm = 8;              ///< A bit flag for the Unicode general character property Letter, modifier.
    constexpr uint32_t Lo = 0x10;           ///< A bit flag for the Unicode general character property Letter, other.
    constexpr uint32_t Mn = 0x20;           ///< A bit flag for the Unicode general character property Mark, nonspacing.
    constexpr uint32_t Mc = 0x40;           ///< A bit flag for the Unicode general character property Mark, spacing combining.
    constexpr uint32_t Me = 0x80;           ///< A bit flag for the Unicode general character property Mark, enclosing.
    constexpr uint32_t Nd = 0x100;          ///< A bit flag for the Unicode general character property Number, decimal digit.
    constexpr uint32_t Nl = 0x200;          ///< A bit flag for the Unicode general character property Number, letter.
    constexpr uint32_t No = 0x400;          ///< A bit flag for the Unicode general character property Number, other.
    constexpr uint32_t Pc = 0x800;          ///< A bit flag for the Unicode general character property Punctuation, connector.
    constexpr uint32_t Pd = 0x1000;         ///< A bit flag for the Unicode general character property Punctuation, dash.
    constexpr uint32_t Ps = 0x2000;         ///< A bit flag for the Unicode general character property Punctuation, open.
    constexpr uint32_t Pe = 0x4000;         ///< A bit flag for the Unicode general character property Punctuation, close.
    constexpr uint32_t Pi = 0x8000;         ///< A bit flag for the Unicode general character property Punctuation, initial quote (may behave like Ps or Pe depending on usage).
    constexpr uint32_t Pf = 0x10000;        ///< A bit flag for the Unicode general character property Punctuation, final quote (may behave like Ps or Pe depending on usage).
    constexpr uint32_t Po = 0x20000;        ///< A bit flag for the Unicode general character property Punctuation, other.
    constexpr uint32_t Sm = 0x40000;        ///< A bit flag for the Unicode general character property Symbol, math.
    constexpr uint32_t Sc = 0x80000;        ///< A bit flag for the Unicode general character property Symbol, currency.
    constexpr uint32_t Sk = 0x100000;       ///< A bit flag for the Unicode general character property Symbol, modifier.
    constexpr uint32_t So = 0x200000;       ///< A bit flag for the Unicode general character property Symbol, other.
    constexpr uint32_t Zs = 0x400000;       ///< A bit flag for the Unicode general character property Separator, space.
    constexpr uint32_t Zl = 0x800000;       ///< A bit flag for the Unicode general character property Separator, line.
    constexpr uint32_t Zp = 0x1000000;      ///< A bit flag for the Unicode general character property Separator, paragraph.
    constexpr uint32_t Cc = 0x2000000;      ///< A bit flag for the Unicode general character property Other, control.
    constexpr uint32_t Cf = 0x4000000;      ///< A bit flag for the Unicode general character property Other, format.
    constexpr uint32_t Cs = 0x8000000;      ///< A bit flag for the Unicode general character property Other, surrogate.
    constexpr uint32_t Co = 0x10000000;     ///< A bit flag for the Unicode general character property Other, private use.
    constexpr uint32_t Cn = 0x20000000;     ///< A bit flag for the Unicode general character property Other, not assigned (including noncharacters).

    /**
    A bit flag to indicate a character outside the categories Lu and Lt that has a lower-case variant.
    It is used internally.
    */
    constexpr uint32_t UpperCase = 0x40000000;
    };

/**
The general category of a character as defined in the Unicode Standard
(http://www.unicode.org/versions/Unicode5.0.0/ch04.pdf).
Each constant is a single bit so that masks can be made by combining them.
*/
enum class CharCategory
    {
    Lu = CharCategoryFlag::Lu, ///< The Unicode general character property Letter, uppercase.
    Ll = CharCategoryFlag::Ll, ///< The Unicode general character property Letter, lowercase.
    Lt = CharCategoryFlag::Lt, ///< The Unicode general character property Letter, titlecase.
    Lm = CharCategoryFlag::Lm, ///< The Unicode general character property Letter, modifier.
    Lo = CharCategoryFlag::Lo, ///< The Unicode general character property Letter, other.
    Mn = CharCategoryFlag::Mn, ///< The Unicode general character property Mark, nonspacing.
    Mc = CharCategoryFlag::Mc, ///< The Unicode general character property Mark, spacing combining.
    Me = CharCategoryFlag::Me, ///< The Unicode general character property Mark, enclosing.
    Nd = CharCategoryFlag::Nd, ///< The Unicode general character property Number, decimal digit.
    Nl = CharCategoryFlag::Nl, ///< The Unicode general character property Number, letter.
    No = CharCategoryFlag::No, ///< The Unicode general character property Number, other.
    Pc = CharCategoryFlag::Pc, ///< The Unicode general character property Punctuation, connector.
    Pd = CharCategoryFlag::Pd, ///< The Unicode general character property Punctuation, dash.
    Ps = CharCategoryFlag::Ps, ///< The Unicode general character property Punctuation, open.
    Pe = CharCategoryFlag::Pe, ///< The Unicode general character property Punctuation, close.
    Pi = CharCategoryFlag::Pi, ///< The Unicode general character property Punctuation, initial quote (may behave like Ps or Pe depending on usage).
    Pf = CharCategoryFlag::Pf, ///< The Unicode general character property Punctuation, final quote (may behave like Ps or Pe depending on usage).
    Po = CharCategoryFlag::Po, ///< The Unicode general character property Punctuation, other.
    Sm = CharCategoryFlag::Sm, ///< The Unicode general character property Symbol, math.
    Sc = CharCategoryFlag::Sc, ///< The Unicode general character property Symbol, currency.
    Sk = CharCategoryFlag::Sk, ///< The Unicode general character property Symbol, modifier.
    So = CharCategoryFlag::So, ///< The Unicode general character property Symbol, other.
    Zs = CharCategoryFlag::Zs, ///< The Unicode general character property Separator, space.
    Zl = CharCategoryFlag::Zl, ///< The Unicode general character property Separator, line.
    Zp = CharCategoryFlag::Zp, ///< The Unicode general character property Separator, paragraph.
    Cc = CharCategoryFlag::Cc, ///< The Unicode general character property Other, control.
    Cf = CharCategoryFlag::Cf, ///< The Unicode general character property Other, format.
    Cs = CharCategoryFlag::Cs, ///< The Unicode general character property Other, surrogate.
    Co = CharCategoryFlag::Co, ///< The Unicode general character property Other, private use.
    Cn = CharCategoryFlag::Cn  ///< The Unicode general character property Other, not assigned (including noncharacters).
    };

namespace BidiTypeFlag
    {
    constexpr uint32_t L = 1;
    constexpr uint32_t LRE = 2;
    constexpr uint32_t LRO = 4;
    constexpr uint32_t R = 8;
    constexpr uint32_t AL = 0x10;
    constexpr uint32_t RLE = 0x20;
    constexpr uint32_t RLO = 0x40;
    constexpr uint32_t PDF = 0x80;
    constexpr uint32_t EN = 0x100;
    constexpr uint32_t ES = 0x200;
    constexpr uint32_t ET = 0x400;
    constexpr uint32_t AN = 0x800;
    constexpr uint32_t CS = 0x1000;
    constexpr uint32_t NSM = 0x2000;
    constexpr uint32_t BN = 0x4000;
    constexpr uint32_t B = 0x8000;
    constexpr uint32_t S = 0x10000;
    constexpr uint32_t WS = 0x20000;
    constexpr uint32_t ON = 0x40000;

    constexpr uint32_t StrongLeftToRight = L | LRE | LRO;
    constexpr uint32_t StrongRightToLeft = R | AL | RLE | RLO;
    constexpr uint32_t Strong = StrongLeftToRight | StrongRightToLeft;
    };

/** The bidirectional type of a character as used in the Unicode Bidirectional Algorithm. */
enum class BidiType
    {
    L   = BidiTypeFlag::L,
    LRE = BidiTypeFlag::LRE,
    LRO = BidiTypeFlag::LRO,
    R   = BidiTypeFlag::R,
    AL  = BidiTypeFlag::AL,
    RLE = BidiTypeFlag::RLE,
    RLO = BidiTypeFlag::RLO,
    PDF = BidiTypeFlag::PDF,
    EN  = BidiTypeFlag::EN,
    ES  = BidiTypeFlag::ES,
    ET  = BidiTypeFlag::ET,
    AN  = BidiTypeFlag::AN,
    CS  = BidiTypeFlag::CS,
    NSM = BidiTypeFlag::NSM,
    BN  = BidiTypeFlag::BN,
    B   = BidiTypeFlag::B,
    S   = BidiTypeFlag::S,
    WS  = BidiTypeFlag::WS,
    ON  = BidiTypeFlag::ON
    };

/**
A class to provide Unicode character properties.
A Char object holds a single character encoded using its Unicode
code point. Surrogates have no meaning in this context. All signed 32-bit
values are valid but not all are defined as characters. Non-character
values give consistent default values: for example, the uppercase version
of a non-character value is the same value.
*/
class Char
    {
    public:
    /**
    A case variant can be no longer than this number of UTF32 characters.
    For example, the upper-case variant of U+00DF is 'SS' and is
    two characters long.
    */
    static constexpr int KMaxCaseVariantLength = 2;

    /** The start of a range of private use characters to select fonts. */
    static constexpr uint32_t KFirstFontSelector = 0xF000;
    /** A private use character to select the default font. */
    static constexpr uint32_t KDefaultFontSelector = 0xF3FF;
    /** The end of a range of private use characters to select fonts. */
    static constexpr uint32_t KLastFontSelector = KDefaultFontSelector;
    /** The number of font selectors, excluding the default font selector. */
    static constexpr uint32_t KFontSelectorCount = KLastFontSelector - KFirstFontSelector;
    /** The start of a range of private use characters to select box styles (background and outline). */
    static constexpr uint32_t KFirstBoxStyleSelector = 0xF400;
    /** The end of a range of private use characters to select box styles (background and outline). */
    static constexpr uint32_t KLastBoxStyleSelector = 0xF7FE;
    /** The number of box style selectors. */
    static constexpr uint32_t KBoxStyleSelectorCount = KLastBoxStyleSelector - KFirstBoxStyleSelector + 1;
    /** A character to mark the end of a box. */
    static constexpr uint32_t KEndOfBox = 0xF7FF;
    /** A character to set the baseline of a box to the baseline of the current line. */
    static constexpr uint32_t KBaselineSelector = 0xF800;
    /** The first private use character used by CartoType. */
    static constexpr uint32_t KFirstCartoTypePrivateUseCharacter = KFirstFontSelector;
    /** The last private use character used by CartoType. */
    static constexpr uint32_t KLastCartoTypePrivateUseCharacter = KBaselineSelector;

    /** Create a Char with the character code 0. */
    Char() { }

    /** Construct a Char from a Unicode code point (without checking its validity as such). */
    Char(int32_t aCode): Code(aCode) { }

    void GetLowerCase(int32_t* aText,int32_t& aTextLength) const;
    void GetTitleCase(int32_t* aText,int32_t& aTextLength) const;
    void GetUpperCase(int32_t* aText,int32_t& aTextLength) const;
    CharCategory Category() const;
    CartoTypeCore::BidiType BidiType() const;
    int32_t Mirrored() const;
    int32_t AccentStripped() const;
    bool IsAlphanumeric() const;
    bool IsWhitespace() const;

    /**
    Return true if a character code is in a word. Words are defined as sequences
    of letters and combining marks.
    */
    static bool InWord(int32_t aCode)
        {
        if (aCode < 'A' && aCode != '\'' && aCode != ':')
            return false;
        if (aCode <= 'Z')
            return true;
        if (aCode < 'a')
            return false;
        if (aCode <= 'z')
            return true;
        if (aCode <= 0xBF)
            return false;
        if (aCode <= 0x17F)
            return true;
        return aCode == 0x2019 || // right single quote
            (uint32_t(Char(aCode).Category()) & (CharCategoryFlag::Lu | CharCategoryFlag::Ll | CharCategoryFlag::Lt | CharCategoryFlag::Lm |
                                                 CharCategoryFlag::Lo | CharCategoryFlag::Mn | CharCategoryFlag::Mc | CharCategoryFlag::Me)) != 0;
        }

    /** The Unicode code point of the character. */
    int32_t Code { 0 };
    };

}
