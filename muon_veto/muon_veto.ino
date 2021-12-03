// Pins on Arduino which can be used
const int pinArray[] = {33, 34, 35,     // 1110 -- 0000000E -- Line C
                        36, 37, 38, 39, // 1111 -- 000000F0 -- Line C
                        40, 41,         // 0011 -- 00000300 -- Line C
                        51, 50, 49, 48, // 1111 -- 0000F000 -- Line C
                        47, 46, 45, 44, // 1111 -- 000F0000 -- Line C
                        9, 8, 7,        // 1110 -- 00E00000 -- Line C
                        6, 5, 4,        // 0111 -- 07000000 -- Line C
                        3, 10,          // 0011 -- 30000000 -- Line C
                        25, 26, 27,     // 0111 -- 00000007 -- Line D
                        24};            // 1000 -- 00008000 -- Line A
                        
// Pins on SAM3U (Entrys 0 - 24: Line C, Entrys 25 - 27: Line D, Entry 28: Line A)
const unsigned long int pinBit[] = {(1u << 1), (1u << 2), (1u << 3),                // 1110 -- E -- Line C
                                   (1u << 4), (1u << 5), (1u << 6), (1u << 7),      // 1111 -- F -- Line C
                                   (1u << 8), (1u << 9),                            // 0011 -- 3 -- Line C
                                   (1u << 12), (1u << 13), (1u << 14), (1u << 15),  // 1111 -- F -- Line C
                                   (1u << 16), (1u << 17), (1u << 18), (1u << 19),  // 1111 -- F -- Line C
                                   (1u << 21), (1u << 22), (1u << 23),              // 1110 -- E -- Line C
                                   (1u << 24), (1u << 25), (1u << 26),              // 0111 -- 7 -- Line C
                                   (1u << 28), (1u << 29),                          // 0011 -- 3 -- Line C
                                   (1u << 0), (1u << 1), (1u << 2),                 // 0111 -- 7 -- Line D
                                   (1u << 15)};                                     // 1000 -- 8 -- Line A

// Pins of box:
// 0: 00000002 -- Line C    1 + 2 = 3
// 1: 00000004 -- Line C    1 + 4 = 5
// 2: 00000008 -- Line C    2 + 4 = 6
                        
// 3: 00000010 -- Line C    1 + 2 + 4 = 7
// 4: 00000020 -- Line C    1 + 8 = 9
// 5: 00000040 -- Line C    2 + 8 = A
// 6: 00000080 -- Line C    1 + 2 + 8 = B

// 7: 00000100 -- Line C    4 + 8 = C
// 8: 00000200 -- Line C    1 + 4 + 8 = D

// 9:  00001000 -- Line C   2 + 4 + 8 = E
// 10: 00002000 -- Line C   1 + 2 + 4 + 8 = F
// 11: 00004000 -- Line C
// 12: 00008000 -- Line C

// 13: 00010000 -- Line C
// 14: 00020000 -- Line C
// 15: 00040000 -- Line C
// 16: 00080000 -- Line C

// 17: 00200000 -- Line C
// 18: 00400000 -- Line C
// 19: 00800000 -- Line C

// 20: 01000000 -- Line C
// 21: 02000000 -- Line C
// 22: 04000000 -- Line C

// 23: 10000000 -- Line C
// 24: 20000000 -- Line C

// 25: 00000001 -- Line D 
// 26: 00000002 -- Line D 
// 27: 00000004 -- Line D 

// 28: Out
                                   
// Pin on Arduino which is used as Output
const int MyOut = 24;
// Pin on SAM3U (Line A)
const unsigned long int outPin = (1u << 15);
// First Pin in arrays which belongs to Line C
const int firstPC = 0;
// First Pin in arrays which belongs to Line D
const int firstPD = 25;
// Length of the arrays
const int arrayLength = 29;
// Last Pin in arrays which belongs to Line C
const int maxPin = 27 - 3;
// An Error-Message
const char errorText[] = "No valid Pin-Number. Pin has to be between 0 and 35 \n";

// Logical masks, pinBit will be saved here in two values (Line C and Line D)
#define MyPORTC 0x37EFF3FE
#define MyPORTD 0x00000007

void setup() {
  //Serial.begin(9600);

  // Setup of the usable pins
  for (int i = 0; i < arrayLength; ++i) {
    pinMode(pinArray[i], OUTPUT);
  }
  
  // ********************
  // Enable pins as input
  // ********************

  // Top-Layer
  for (int i = 0; i < 3; i++){
    pinMode(pinArray[i], INPUT);
  }
  // Bottom-Layer
  for (int i = 3; i < 7; i++){
    pinMode(pinArray[i], INPUT);
  }
  // Front-Layer
  
  // Back-Layer
  for (int i = 7; i < 11; i++){
    pinMode(pinArray[i], INPUT);
  }
  // Left-Layer
  for (int i = 11; i < 15; i++){
    pinMode(pinArray[i], INPUT);
  }
  // Right-Layer
  for (int i = 15; i < 19; i++){
    pinMode(pinArray[i], INPUT);
  }
}

#define top 0x0000001E
#define bottom 0x000001E0
#define front 0x17000000
#define back 0x00007200
#define left 0x00078000
#define right 0x00E80000

// top | bottom | front | back | left
#define all5 0x000FF3FE
// bottom | front | back | left
#define all4 0x000FF3E0
// front | back | left
#define all3 0x000FF200
// back | left
#define all2 0x000FF200

void loop() {
  // Read the Pin-Status considering the usable pins
  const unsigned long int ReadC = REG_PIOC_PDSR & MyPORTC;
  
  // Port D is currently not used. 25 Panels can be read with Port C. 24 Panels are used.
  // const unsigned long int ReadD = REG_PIOD_PDSR & MyPORTD;

  // More than one layer must be high:
  // if ((all5 & ReadC) && ((right & ReadD) ||    // If the right side trigggerd ant any of the other 5 sides,
  //     ((top & ReadC) && (all4 & ReadC)) ||     // or the top side and any of the other 4 sides (not right),
  //     ((bottom & ReadC) && (all3 & ReadC)) ||  // or the bottom side and any of the other 3 sides (not right and not top),
  //     ((front & ReadC) && (all2 & ReadC)) ||   // or the front side and one of the left or back sides,
  //     ((back & ReadC) && (left & ReadC))))     // or the left and the back sides.
  // {REG_PIOA_SODR = outPin;}

  // Port D is currently not used
  if ((all5 & ReadC) && ((right & ReadC) ||
      ((top & ReadC) && (all4 & ReadC)) ||
       ((bottom & ReadC) && (all3 & ReadC)) ||
       ((front & ReadC) && (all2 & ReadC)) ||
       ((back & ReadC) && (left & ReadC))))
  {
    REG_PIOA_SODR = outPin;
  }else REG_PIOA_CODR = outPin;

  REG_PIOA_SODR = outPin;
  REG_PIOA_CODR = outPin;
  //Serial.print("Hello");
}
