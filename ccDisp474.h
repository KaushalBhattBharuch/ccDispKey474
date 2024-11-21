#ifndef CCDISP474_H
#define CCDISP474_H

#include <Arduino.h>

#define NO_OF_REGS 2

enum pushButon
{
  KI1 = 1,
  KI2,
  KI3,
  KI4
};

class ccDisp474
{
  //  public:
  uint8_t data_Pin;
  uint8_t lat_Pin;
  uint8_t clk_Pin;
  uint8_t keys_Pin;
  uint8_t sregs[NO_OF_REGS];

  const uint8_t digitCode[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
  uint8_t refreshTime = 6; // in ms
  unsigned long int preMillsRefrsh = 0;
  unsigned long int curMillsRefrsh = 0;
  uint8_t digitTurn = 0;
  uint8_t segmentData[4];
  uint16_t scanTime = 250; // in ms

  typedef struct
  {
    uint8_t pb1Cur : 1;
    uint8_t pb1Pre : 1;
    uint8_t pb2Cur : 1;
    uint8_t pb2Pre : 1;
    uint8_t pb3Cur : 1;
    uint8_t pb3Pre : 1;
    uint8_t pb4Cur : 1;
    uint8_t pb4Pre : 1;
  } pb_bits;
  typedef union
  {
    uint8_t pbByte;
    pb_bits pbbits;
  } pb_pins;
  pb_pins pbPins;
  typedef struct
  {
    uint8_t pb1Rise : 1;
    uint8_t pb1Fall : 1;
    uint8_t pb2Rise : 1;
    uint8_t pb2Fall : 1;
    uint8_t pb3Rise : 1;
    uint8_t pb3Fall : 1;
    uint8_t pb4Rise : 1;
    uint8_t pb4Fall : 1;
  } pb_resultBits;
  typedef union
  {
    /* data */
    uint8_t pb_result8;
    pb_resultBits pbResultBits;
  } pb_results;
  pb_results pbResult;
  unsigned long int preMillsScan = 0;
  unsigned long int curMillsScan = 0;

  void sendToShiftReg(const uint8_t *sregs, uint8_t noOfRegs);
  void hardDelay125ns(void);

public:
  ccDisp474(const int data_pin, const int lat_pin, const int clk_pin);
  ccDisp474(const int data_pin, const int lat_pin, const int clk_pin, const int keys_pin);

  void loopDisp(void);
  template <typename T>
  bool display(T num)
  {
    uint16_t intpart;
    uint16_t numf;
    int8 tempi;

    uint16_t whole;
    float frac;
    whole = (uint16_t)num;
    frac = num - whole;
    numf = num;

    segmentData[0] = segmentData[1] = segmentData[2] = segmentData[3] = digitCode[0];
    if (num == 0)
    {
      return 1;
    }
    if (num > 9999)
    {
      segmentData[0] = segmentData[1] = segmentData[2] = segmentData[3] = 0x40;
      return 0;
    }
    intpart = (uint16_t)num;
    tempi = 3;
    while (intpart)
    {
      segmentData[tempi] = digitCode[(intpart % 10)];
      intpart = intpart / 10;
      tempi--;
    }
    if (tempi >= 0)
    {
      for (int8_t i = tempi; i >= 0; i--)
      {
        for (int8_t j = i; j <= 3; j++)
        {
          segmentData[j] = segmentData[j + 1];
        }
      }

      if (tempi == 3)
      {
        segmentData[0] = digitCode[0] | 0x80;
        tempi--;
      }
      else
      {
        segmentData[2 - tempi] |= 0x80;
      }
      for (int8_t j = 3 - tempi; j <= 3; j++)
      {
        segmentData[j] = digitCode[0];
      }
    }
    if (frac)
    {
      uint8_t fraci = 3 - tempi;
      while (fraci <= 3)
      {
        num = num * 10;
        numf = num;
        segmentData[fraci] = digitCode[(numf % 10)];
        fraci++;
      }
    }
    return 1;
  }
  void loopKeyScan(void);
  bool getPbRising(pushButon);
  bool getPbFalling(pushButon);
  void setRefreshTime(uint8_t rR);
  void setKeyScanTime(uint16_t sR);
};

#endif
