// default gnome-terminal color scheme Tango:
// #2E2E34343636:#CCCC00000000:#4E4E9A9A0606:#C4C4A0A00000:
// #34346565A4A4:#757550507B7B:#060698209A9A:#D3D3D7D7CFCF:
// #555557575353:#EFEF29292929:#8A8AE2E23434:#FCFCE9E94F4F:
// #72729F9FCFCF:#ADAD7F7FA8A8:#3434E2E2E2E2:#EEEEEEEEECEC
int palette_[] = {
#if 0
  0,0,0, // 0x2e, 0x34, 0x36, // color 0 used to be dark grey but isn't anymore?
  0xcc, 0x00, 0x00,
  0x4e, 0x9a, 0x06,
  0xc4, 0xa0, 0x00,
  0x34, 0x65, 0xa4,
  0x75, 0x50, 0x7b,
  0x06, 0x98, 0x9a,
  0xd3, 0xd7, 0xcf,
  0x55, 0x75, 0x53,
  0xef, 0x29, 0x29,
  0x8a, 0xe2, 0x34,
  0xfc, 0xe9, 0x4f,
  0x72, 0x9f, 0xcf,
  0xad, 0x7f, 0xa8,
  0x34, 0xe2, 0xe2,
  0xee, 0xee, 0xec
#else
  // OS X terminal default colors
  0, 0, 0,
  153, 0, 0,
  0, 166, 0,
  153, 153, 0,
  0, 0, 178,
  178, 0, 178,
  0, 166, 178,
  191, 191, 191,
  102, 102, 102,
  229, 0, 0,
  0, 217, 0,
  229, 229, 0,
  0, 0, 255,
  229, 0, 229,
  0, 229, 229,
  229, 229, 229
#endif
};

