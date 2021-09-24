#ifndef Term11x16_font_h
#define Term11x16_font_h

const uint8_t Term7x14PL[]  =
{
7, 16, 32, '~'+1+18,   // -width, height, firstChar, lastChar
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
        0x04, 0x3E, 0x00, 0xFF, 0x33, 0xFF, 0x33, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // !
        0x06, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00,  // "
        0x07, 0x20, 0x01, 0xFE, 0x1F, 0xFE, 0x1F, 0x20, 0x01, 0xFE, 0x1F, 0xFE, 0x1F, 0x20, 0x01,  // #
        0x07, 0x38, 0x06, 0x3C, 0x0E, 0x64, 0x08, 0xFF, 0x3F, 0xFF, 0x3F, 0x84, 0x09, 0x18, 0x07,  // $
        0x07, 0x1E, 0x30, 0x33, 0x1C, 0x1E, 0x07, 0xC0, 0x01, 0x70, 0x1E, 0x1C, 0x33, 0x07, 0x1E,  // %
        0x07, 0x1E, 0x1F, 0xF3, 0x31, 0xE1, 0x20, 0xB3, 0x31, 0x1E, 0x1F, 0x00, 0x1B, 0x80, 0x31,  // &
        0x02, 0x0B, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '
        0x04, 0xF0, 0x07, 0xFE, 0x3F, 0x0F, 0x78, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // (
        0x04, 0x01, 0x40, 0x0F, 0x78, 0xFE, 0x3F, 0xF0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // )
        0x07, 0x80, 0x00, 0xA0, 0x02, 0xE0, 0x03, 0xC0, 0x01, 0xE0, 0x03, 0xA0, 0x02, 0x80, 0x00,  // *
        0x06, 0x80, 0x00, 0x80, 0x00, 0xE0, 0x03, 0xE0, 0x03, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00,  // +
        0x02, 0x00, 0xB0, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ,
        0x06, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00,  // -
        0x02, 0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // .
        0x07, 0x00, 0x38, 0x00, 0x3E, 0x80, 0x07, 0xE0, 0x01, 0x78, 0x00, 0x1F, 0x00, 0x07, 0x00,  // /
        0x07, 0xFC, 0x0F, 0xFE, 0x1F, 0x03, 0x33, 0xC1, 0x20, 0x33, 0x30, 0xFE, 0x1F, 0xFC, 0x0F,  // 0
        0x06, 0x04, 0x20, 0x06, 0x20, 0xFF, 0x3F, 0xFF, 0x3F, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00,  // 1
        0x07, 0x0C, 0x3C, 0x0E, 0x3E, 0x03, 0x23, 0x81, 0x21, 0xC3, 0x20, 0x7E, 0x20, 0x3C, 0x20,  // 2
        0x07, 0x0C, 0x0C, 0x0E, 0x1C, 0x43, 0x30, 0x41, 0x20, 0xE3, 0x30, 0xBE, 0x1F, 0x1C, 0x0F,  // 3
        0x07, 0x00, 0x0F, 0xE0, 0x0F, 0xFC, 0x08, 0x1F, 0x08, 0x83, 0x3F, 0x80, 0x3F, 0x00, 0x08,  // 4
        0x07, 0x3F, 0x0C, 0x3F, 0x1C, 0x21, 0x30, 0x21, 0x20, 0x61, 0x30, 0xC1, 0x1F, 0x81, 0x0F,  // 5
        0x07, 0xE0, 0x0F, 0xF8, 0x1F, 0x5C, 0x30, 0x46, 0x20, 0xC3, 0x30, 0x81, 0x1F, 0x01, 0x0F,  // 6
        0x07, 0x01, 0x00, 0x01, 0x00, 0x01, 0x3C, 0x81, 0x3F, 0xF1, 0x03, 0x7F, 0x00, 0x0F, 0x00,  // 7
        0x07, 0x1C, 0x0F, 0xBE, 0x1F, 0xE3, 0x30, 0x41, 0x20, 0xE3, 0x30, 0xFE, 0x1F, 0x1C, 0x0F,  // 8
        0x07, 0x3C, 0x20, 0x7E, 0x20, 0xC3, 0x30, 0x81, 0x18, 0x83, 0x0E, 0xFE, 0x07, 0xFC, 0x01,  // 9
        0x02, 0x60, 0x30, 0x60, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // :
        0x02, 0x60, 0xB0, 0x60, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ;
        0x07, 0x80, 0x00, 0xC0, 0x01, 0x60, 0x03, 0x30, 0x06, 0x18, 0x0C, 0x0C, 0x18, 0x04, 0x10,  // <
        0x06, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x00, 0x00,  // =
        0x07, 0x04, 0x10, 0x0C, 0x18, 0x18, 0x0C, 0x30, 0x06, 0x60, 0x03, 0xC0, 0x01, 0x80, 0x00,  // >
        0x07, 0x06, 0x00, 0x07, 0x00, 0x03, 0x33, 0xC1, 0x33, 0xF3, 0x00, 0x3F, 0x00, 0x1E, 0x00,  // ?
        0x07, 0xFC, 0x0F, 0xFE, 0x1F, 0x03, 0x30, 0xE1, 0x23, 0x11, 0x22, 0x13, 0x21, 0xFE, 0x33,  // @
        0x07, 0xFC, 0x3F, 0xFE, 0x3F, 0x03, 0x01, 0x01, 0x01, 0x03, 0x01, 0xFE, 0x3F, 0xFC, 0x3F,  // A
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x41, 0x20, 0x41, 0x20, 0xE3, 0x30, 0xBE, 0x1F, 0x1C, 0x0F,  // B
        0x07, 0xFC, 0x0F, 0xFE, 0x1F, 0x03, 0x30, 0x01, 0x20, 0x03, 0x30, 0x0E, 0x1C, 0x0C, 0x0C,  // C
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x07, 0x38, 0xFE, 0x1F, 0xF8, 0x07,  // D
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x41, 0x20, 0x41, 0x20, 0x41, 0x20, 0x41, 0x20, 0x01, 0x20,  // E
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x01, 0x00,  // F
        0x07, 0xFC, 0x0F, 0xFE, 0x1F, 0x03, 0x30, 0x81, 0x20, 0x83, 0x30, 0x8E, 0x1F, 0x8C, 0x0F,  // G
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0xFF, 0x3F, 0xFF, 0x3F,  // H
        0x06, 0x01, 0x20, 0x01, 0x20, 0xFF, 0x3F, 0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x00, 0x00,  // I
        0x07, 0x00, 0x0C, 0x00, 0x1C, 0x00, 0x30, 0x00, 0x20, 0x00, 0x30, 0xFF, 0x1F, 0xFF, 0x0F,  // J
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0xC0, 0x00, 0xE0, 0x01, 0x3C, 0x0F, 0x1F, 0x3E, 0x03, 0x30,  // K
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20,  // L
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x78, 0x00, 0xC0, 0x03, 0x78, 0x00, 0xFF, 0x3F, 0xFF, 0x3F,  // M
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x1C, 0x00, 0x70, 0x00, 0xC0, 0x01, 0xFF, 0x3F, 0xFF, 0x3F,  // N
        0x07, 0xFC, 0x0F, 0xFE, 0x1F, 0x03, 0x30, 0x01, 0x20, 0x03, 0x30, 0xFE, 0x1F, 0xFC, 0x0F,  // O
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x81, 0x00, 0x81, 0x00, 0xC3, 0x00, 0x7E, 0x00, 0x3C, 0x00,  // P
        0x07, 0xFC, 0x0F, 0xFE, 0x1F, 0x03, 0x30, 0x01, 0x60, 0x03, 0xF0, 0xFE, 0x9F, 0xFC, 0x0F,  // Q
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x81, 0x00, 0x81, 0x01, 0xC3, 0x1F, 0x7E, 0x3E, 0x3C, 0x20,  // R
        0x07, 0x3C, 0x0C, 0x7E, 0x1C, 0x63, 0x30, 0xE1, 0x20, 0xC3, 0x31, 0x8E, 0x1F, 0x0C, 0x0F,  // S
        0x06, 0x01, 0x00, 0x01, 0x00, 0xFF, 0x3F, 0xFF, 0x3F, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,  // T
        0x07, 0xFF, 0x0F, 0xFF, 0x1F, 0x00, 0x30, 0x00, 0x20, 0x00, 0x30, 0xFF, 0x1F, 0xFF, 0x0F,  // U
        0x07, 0x3F, 0x00, 0xFF, 0x01, 0xC0, 0x0F, 0x00, 0x3C, 0xC0, 0x0F, 0xFF, 0x03, 0x3F, 0x00,  // V
        0x07, 0xFF, 0x01, 0xFF, 0x3F, 0x00, 0x3E, 0xC0, 0x01, 0x00, 0x3E, 0xFF, 0x3F, 0xFF, 0x01,  // W
        0x07, 0x0F, 0x3C, 0x3F, 0x3F, 0xF0, 0x03, 0xC0, 0x00, 0xF0, 0x03, 0x3F, 0x3F, 0x0F, 0x3C,  // X
        0x07, 0x3F, 0x00, 0xFF, 0x00, 0xC0, 0x01, 0x80, 0x3F, 0xC0, 0x3F, 0xFF, 0x00, 0x7F, 0x00,  // Y
        0x07, 0x01, 0x3C, 0x01, 0x3F, 0xC1, 0x23, 0xE1, 0x20, 0x31, 0x20, 0x1F, 0x20, 0x0F, 0x20,  // Z
        0x04, 0xFF, 0x3F, 0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // [
        0x07, 0x07, 0x00, 0x1F, 0x00, 0x78, 0x00, 0xE0, 0x01, 0x80, 0x07, 0x00, 0x3E, 0x00, 0x38,  // BackSlash
        0x04, 0x01, 0x20, 0x01, 0x20, 0xFF, 0x3F, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ]
        0x06, 0x04, 0x00, 0x06, 0x00, 0x03, 0x00, 0x03, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00, 0x00,  // ^
        0x07, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80,  // _
        0x06, 0x06, 0x00, 0x0F, 0x00, 0x09, 0x00, 0x09, 0x00, 0x0F, 0x00, 0x06, 0x00, 0x00, 0x00,  // `
        0x07, 0x00, 0x1E, 0x40, 0x3F, 0x20, 0x21, 0x20, 0x21, 0x20, 0x11, 0xE0, 0x3F, 0xC0, 0x3F,  // a
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x40, 0x10, 0x20, 0x20, 0x60, 0x30, 0xC0, 0x1F, 0x80, 0x0F,  // b
        0x07, 0x80, 0x0F, 0xC0, 0x1F, 0x60, 0x30, 0x20, 0x20, 0x20, 0x20, 0x60, 0x30, 0x40, 0x10,  // c
        0x07, 0x80, 0x0F, 0xC0, 0x1F, 0x60, 0x30, 0x20, 0x20, 0x40, 0x10, 0xFF, 0x3F, 0xFF, 0x3F,  // d
        0x07, 0x80, 0x0F, 0xC0, 0x1F, 0x60, 0x32, 0x20, 0x22, 0x60, 0x22, 0xC0, 0x33, 0x80, 0x13,  // e
        0x06, 0x20, 0x00, 0xFE, 0x3F, 0xFF, 0x3F, 0x21, 0x00, 0x23, 0x00, 0x02, 0x00, 0x00, 0x00,  // f
        0x07, 0x80, 0x07, 0xC0, 0x4F, 0x60, 0xD8, 0x20, 0x90, 0x40, 0xC8, 0xE0, 0x7F, 0xE0, 0x3F,  // g
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x40, 0x00, 0x20, 0x00, 0x60, 0x00, 0xC0, 0x3F, 0x80, 0x3F,  // h
        0x06, 0x20, 0x20, 0x20, 0x20, 0xE3, 0x3F, 0xE3, 0x3F, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00,  // i
        0x05, 0x00, 0x40, 0x20, 0xC0, 0x20, 0x80, 0xE3, 0xFF, 0xE3, 0x7F, 0x00, 0x00, 0x00, 0x00,  // j
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x00, 0x07, 0x80, 0x0D, 0xC0, 0x18, 0x60, 0x30, 0x20, 0x20,  // k
        0x06, 0x01, 0x20, 0x01, 0x20, 0xFF, 0x3F, 0xFF, 0x3F, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00,  // l
        0x07, 0xE0, 0x3F, 0xE0, 0x3F, 0x60, 0x00, 0xC0, 0x1F, 0x60, 0x00, 0xE0, 0x3F, 0xC0, 0x3F,  // m
        0x07, 0xE0, 0x3F, 0xE0, 0x3F, 0x40, 0x00, 0x20, 0x00, 0x60, 0x00, 0xC0, 0x3F, 0x80, 0x3F,  // n
        0x07, 0x80, 0x0F, 0xC0, 0x1F, 0x60, 0x30, 0x20, 0x20, 0x60, 0x30, 0xC0, 0x1F, 0x80, 0x0F,  // o
        0x07, 0xE0, 0xFF, 0xE0, 0xFF, 0x40, 0x10, 0x20, 0x20, 0x60, 0x30, 0xC0, 0x1F, 0x80, 0x0F,  // p
        0x07, 0x80, 0x0F, 0xC0, 0x1F, 0x20, 0x20, 0x20, 0x20, 0x40, 0x10, 0xE0, 0xFF, 0xE0, 0xFF,  // q
        0x07, 0xE0, 0x3F, 0xE0, 0x3F, 0xC0, 0x00, 0x40, 0x00, 0x60, 0x00, 0x60, 0x00, 0x40, 0x00,  // r
        0x07, 0xC0, 0x10, 0xE0, 0x31, 0x20, 0x21, 0x20, 0x23, 0x20, 0x26, 0x60, 0x3C, 0x40, 0x18,  // s
        0x06, 0x20, 0x00, 0x20, 0x00, 0xFE, 0x1F, 0xFE, 0x3F, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00,  // t
        0x07, 0xE0, 0x1F, 0xE0, 0x3F, 0x00, 0x20, 0x00, 0x20, 0x00, 0x10, 0xE0, 0x3F, 0xE0, 0x3F,  // u
        0x07, 0xE0, 0x03, 0xE0, 0x07, 0x00, 0x1C, 0x00, 0x30, 0x00, 0x1C, 0xE0, 0x07, 0xE0, 0x03,  // v
        0x07, 0xE0, 0x07, 0xE0, 0x3F, 0x00, 0x38, 0xC0, 0x07, 0x00, 0x38, 0xE0, 0x3F, 0xE0, 0x07,  // w
        0x07, 0x60, 0x30, 0xE0, 0x3D, 0x80, 0x0F, 0x00, 0x07, 0x80, 0x0F, 0xE0, 0x3D, 0x60, 0x30,  // x
        0x07, 0xE0, 0x83, 0xE0, 0x8F, 0x00, 0xDC, 0x00, 0x70, 0x00, 0x3C, 0xE0, 0x0F, 0xE0, 0x03,  // y
        0x07, 0x20, 0x38, 0x20, 0x3C, 0x20, 0x26, 0x20, 0x23, 0xA0, 0x21, 0xE0, 0x20, 0x60, 0x20,  // z
        0x05, 0x80, 0x00, 0xFE, 0x3F, 0x7F, 0x7F, 0x01, 0x40, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00,  // {
        0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // |
        0x05, 0x01, 0x40, 0x01, 0x40, 0x7F, 0x7F, 0xFE, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,  // }
        0x07, 0x02, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x01, 0x00,  // ~
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x03, 0x30, 0x03, 0x30, 0x03, 0x30, 0xFF, 0x3F, 0xFF, 0x3F,  // 

        0x07, 0xFC, 0x3F, 0xFE, 0x3F, 0x03, 0x01, 0x01, 0x01, 0x03, 0x41, 0xFE, 0xFF, 0xFC, 0xBF,  // €
        0x07, 0xF8, 0x0F, 0xFC, 0x1F, 0x02, 0x30, 0x06, 0x20, 0x03, 0x30, 0x1C, 0x1C, 0x18, 0x0C,  // 
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x41, 0x20, 0x41, 0x20, 0x41, 0x60, 0x41, 0xE0, 0x01, 0xA0,  // ‚
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0xC0, 0x20, 0x60, 0x20, 0x30, 0x20, 0x10, 0x20, 0x00, 0x20,  // ƒ
        0x07, 0xFF, 0x3F, 0xFF, 0x3F, 0x1C, 0x00, 0x73, 0x00, 0xC0, 0x01, 0xFF, 0x3F, 0xFF, 0x3F,  // „
        0x07, 0xF8, 0x0F, 0xFC, 0x1F, 0x02, 0x30, 0x06, 0x20, 0x03, 0x30, 0xFC, 0x1F, 0xF8, 0x0F,  // …
        0x07, 0x38, 0x0C, 0x7C, 0x1C, 0x62, 0x30, 0xE6, 0x20, 0xC3, 0x31, 0x8C, 0x1F, 0x08, 0x0F,  // †
        0x07, 0x02, 0x3C, 0x06, 0x3F, 0x82, 0x23, 0xC3, 0x20, 0x62, 0x20, 0x3E, 0x20, 0x1E, 0x20,  // ‡
        0x07, 0x41, 0x38, 0x41, 0x3E, 0xC1, 0x27, 0xE1, 0x21, 0x79, 0x20, 0x5F, 0x20, 0x47, 0x20,  // ˆ

        0x07, 0x00, 0x1E, 0x40, 0x3F, 0x20, 0x21, 0x20, 0x21, 0x20, 0x51, 0xE0, 0xFF, 0xC0, 0xBF,  // ‰
        0x07, 0x80, 0x0F, 0xC0, 0x1F, 0x60, 0x30, 0x24, 0x20, 0x26, 0x20, 0x62, 0x30, 0x40, 0x10,  // Š
        0x07, 0x80, 0x0F, 0xC0, 0x1F, 0x60, 0x32, 0x20, 0x22, 0x60, 0x62, 0xC0, 0xF3, 0x80, 0x93,  // ‹
        0x06, 0x01, 0x26, 0x01, 0x23, 0xFF, 0x3F, 0xFF, 0x3F, 0x60, 0x20, 0x30, 0x20, 0x00, 0x00,  // Œ
        0x07, 0xE0, 0x3F, 0xE0, 0x3F, 0x40, 0x00, 0x24, 0x00, 0x66, 0x00, 0xC2, 0x3F, 0x80, 0x3F,  // 
        0x07, 0x80, 0x0F, 0xC0, 0x1F, 0x60, 0x30, 0x24, 0x20, 0x66, 0x30, 0xC2, 0x1F, 0x80, 0x0F,  // Ž
        0x07, 0xC0, 0x10, 0xE0, 0x31, 0x20, 0x21, 0x24, 0x23, 0x26, 0x26, 0x62, 0x3C, 0x40, 0x18,  // 
        0x07, 0x20, 0x38, 0x20, 0x3C, 0x24, 0x26, 0x26, 0x23, 0xA2, 0x21, 0xE0, 0x20, 0x60, 0x20,  // 
        0x07, 0x20, 0x38, 0x20, 0x3C, 0x26, 0x26, 0x26, 0x23, 0xA0, 0x21, 0xE0, 0x20, 0x60, 0x20,  // ‘
};

#endif