/*
cartotype_color.h
Copyright (C) 2013-2022 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_types.h>
#include <vector>

namespace CartoTypeCore
{

/** Opaque black. */
const uint32_t KBlack = 0xFF000000;
/** Opaque dark grey. */
const uint32_t KDarkGrey = 0xFF555555;
/** Opaque dark gray. */
const uint32_t KDarkGray = 0xFF555555;
/** Opaque dark red. */
const uint32_t KDarkRed = 0xFF000080;
/** Opaque dark green. */
const uint32_t KDarkGreen = 0xFF008000;
/** Opaque dark yellow. */
const uint32_t KDarkYellow = 0xFF008080;
/** Opaque dark blue. */
const uint32_t KDarkBlue = 0xFF800000;
/** Opaque dark magenta. */
const uint32_t KDarkMagenta = 0xFF800080;
/** Opaque dark cyan. */
const uint32_t KDarkCyan = 0xFF808000;
/** Opaque red. */
const uint32_t KRed = 0xFF0000FF;
/** Opaque green. */
const uint32_t KGreen = 0xFF00FF00;
/** Opaque yellow. */
const uint32_t KYellow = 0xFF00FFFF;
/** Opaque blue. */
const uint32_t KBlue = 0xFFFF0000;
/** Opaque magenta. */
const uint32_t KMagenta = 0xFFFF00FF;
/** Opaque cyan. */
const uint32_t KCyan = 0xFFFFFF00;
/** Opaque grey. */
const uint32_t KGrey = 0xFFAAAAAA;
/** Opaque gray. */
const uint32_t KGray = KGrey;
/** Opaque white. */
const uint32_t KWhite = 0xFFFFFFFF;

/** The 'null color' transparent black. */
const uint32_t KTransparentBlack = 0x00000000;

/**
A color.

Colors are represented by 32-bit integers containing 8 bits each
of red, green, blue and alpha channel (transparency) data.
*/
class Color
    {
    public:
    /** Creates a color and set it to opaque black. */
    Color() { }
    /** Creates a color from an integer value. */
    Color(uint32_t aValue): Value(aValue) { }
    /** Creates a color from red, green, blue and alpha values. */
    Color(int32_t aRed,int32_t aGreen,int32_t aBlue,int32_t aAlpha = 0xFF):
        Value(((aAlpha & 0xFF) << 24) | ((aBlue & 0xFF) << 16) | ((aGreen & 0xFF) << 8) | (aRed & 0xFF))
        {
        }
    explicit Color(const MString& aColor);
    explicit Color(const String& aColor);
    /** The equality operator. */
    bool operator==(const Color& aColor) const { return Value == aColor.Value; }
    /** The inequality operator. */
    bool operator!=(const Color& aColor) const { return Value != aColor.Value; }
    /** Returns the red component as a value in the range 0...255. */
    int32_t Red() const { return (Value & 0xFF); }
    /** Returns the green component as a value in the range 0...255. */
    int32_t Green() const { return (Value & 0xFF00) >> 8; }
    /** Returns the blue component as a value in the range 0...255. */
    int32_t Blue() const { return (Value & 0xFF0000) >> 16; }
    /**
    Returns a grey level as a value in the range 0...255, calculated from red, green and blue levels in that range.
    Uses the formula Grey = (Red * 0.299 + Green * 0.587 + Blue * 0.114), derived from the BT.601 standard.
    */
    static int32_t Grey(int32_t aRed,int32_t aGreen,int32_t aBlue)
        {
        return (aRed * 77 + aGreen * 150 + aBlue * 29) >> 8;
        }
    /**
    Returns the grey level as a value in the range 0...255.
    Uses the formula Grey = (Red * 0.299 + Green * 0.587 + Blue * 0.114), derived from the BT.601 standard.
    */
    int32_t Grey() const { return Grey(Red(),Green(),Blue()); }
    /** A synonym for Grey() */
    int32_t Gray() const { return Grey(); }
    /** Returns the alpha (transparency) level as a value in the range 0...255: 0 = transparent, 255 = opaque. */
    int32_t Alpha() const { return (Value & 0xFF000000) >> 24; }
    /** Returns true if the colour is null. */
    bool IsNull() const { return (Value & 0xFF000000) == 0; }
    /** Sets the alpha (transparency) level to a value in the range 0...255: 0 = transparent, 255 = opaque. */
    void SetAlpha(int32_t aAlpha) { Value = (aAlpha << 24) | (Value & 0xFFFFFF); }
    void CombineAlpha(int32_t aAlpha);
    void PremultiplyAlpha();
    void Blend(Color aOtherColor,int32_t aOtherColorAlpha);
    void Blend(Color aOtherColor);
    size_t Set(const MString& aColor);

    /**
    The color value, containing, starting with the least significant byte, red, green, blue and alpha levels,
    each stored in eight bits. The color levels represent intensity. The alpha level represents opacity.
    */
    uint32_t Value { KBlack };
    };

/**
A rule to modify a certain style or styles.
The colors are blended with colors in a style, using their alpha values as the blend fraction.
*/
class BlendStyle
    {
    public:
    /** The equality operator. */
    bool operator==(const BlendStyle& aOther) const
        {
        return Styles == aOther.Styles &&
               MainColor == aOther.MainColor &&
               BorderColor == aOther.BorderColor &&
               TextColor == aOther.TextColor &&
               TextGlowColor == aOther.TextGlowColor &&
               IconColor == aOther.IconColor;
        }
    /** The inequality operator. */
    bool operator!=(const BlendStyle& aOther) const { return !(*this == aOther); }

    /**
    A space or comma separated list of wild-card style names.
    The special name '[legend]' is used for the legend, scale, copyright notice and turn instructions.
    */
    String Styles;
    /** The color to be blended with the colors in the style. */
    Color MainColor = KTransparentBlack;
    /** If not null, the color to be blended with border colors. */
    Color BorderColor = KTransparentBlack;
    /** If not null, the color to be blended with text colors. */
    Color TextColor = KTransparentBlack;
    /** If not null, the color to be blended with text glow colors. */
    Color TextGlowColor = KTransparentBlack;
    /** If not null, the forced color for icons. */
    Color IconColor = KTransparentBlack;
    };

/**
A set of rules for modifying style sheet colors, for example to add contrast, lighten or darken.
The rules are applied in order.
*/
class BlendStyleSet: public std::vector<BlendStyle>
    {
    public:
    const BlendStyle* FindStyle(const MString& aStyleName) const;
    };

}
