// Protocol description ver 1.0
// data[0] HEADER  0xAF    8-bit unsigned integer (unsigned char)
// data[1] LENG    0x03    8-bit unsigned integer (unsigned char) min = 3
// data[2] COMMAND 0x00    8-bit unsigned integer (unsigned char)
// data[3] DATA    0x00    8-bit unsigned integer (unsigned char)
// data[4] CRC     0x00    8-bit unsigned integer (unsigned char) 
// CRC - data[4] XOR data[0...3]