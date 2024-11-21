#include "ccDisp474.h"

ccDisp474::ccDisp474(const int data_pin, const int lat_pin, const int clk_pin)
{
    data_Pin = data_pin;
    lat_Pin = lat_pin;
    clk_Pin = clk_pin;
    pinMode(data_Pin, OUTPUT);
    pinMode(lat_Pin, OUTPUT);
    pinMode(clk_Pin, OUTPUT);
    digitalWrite(data_Pin, LOW);
    digitalWrite(lat_Pin, LOW);
    digitalWrite(clk_Pin, LOW);
}
ccDisp474::ccDisp474(const int data_pin, const int lat_pin, const int clk_pin, const int keys_pin)
{
    data_Pin = data_pin;
    lat_Pin = lat_pin;
    clk_Pin = clk_pin;
    keys_Pin = keys_pin;

    pinMode(data_Pin, OUTPUT);
    pinMode(lat_Pin, OUTPUT);
    pinMode(clk_Pin, OUTPUT);
    pinMode(keys_Pin, INPUT);

    digitalWrite(data_Pin, LOW);
    digitalWrite(lat_Pin, LOW);
    digitalWrite(clk_Pin, LOW);

    pbPins.pbByte = 0;
    pbResult.pb_result8 = 0;
}
void ccDisp474::sendToShiftReg(const uint8_t *sregs, uint8_t noOfRegs)
{
    // shiftOut(data_Pin,clk_Pin,MSBFIRST,sregs.sbyte);
    for (uint8_t i = 0; i < noOfRegs; i++)
    {
        for (int8 j = 0; j < 8; j++)
        {
            // digitalWrite(data_Pin, !!((*sregs) & (1 << (7 - j)))); // or below equivelent line
            digitalWrite(data_Pin, ((*sregs) & (1 << (7 - j))) ? HIGH : LOW); // msb first
            // digitalWrite(data_Pin,((*sregs) & (1 << j))?HIGH:LOW);//lsb first
            //ccDisp474::hardDelay125ns();
            digitalWrite(clk_Pin, HIGH);
            //ccDisp474::hardDelay125ns();
            digitalWrite(clk_Pin, LOW);
        }
        sregs++;
    }
    digitalWrite(lat_Pin, HIGH);
    ccDisp474::hardDelay125ns();
    digitalWrite(lat_Pin, LOW);
}
void ccDisp474::hardDelay125ns(void)
{
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP(); // 1/80 microsecond from 1 nop
    // 10/80 = 1/8 = 0.125 us = 125 ns which is greater than mentioned in datasheet of 74HC595 100 ns
    // branching and return will add extra time
    //_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();
}
void ccDisp474::loopDisp(void)
{
    curMillsRefrsh = millis();
    if (curMillsRefrsh - preMillsRefrsh >= refreshTime)
    {
        preMillsRefrsh = curMillsRefrsh;
        switch (digitTurn)
        {
        case 0:
            sregs[0] = ((sregs[0] & 0x0F) | 0x10);
            sregs[1] = segmentData[0];
            break;
        case 1:
            sregs[0] = ((sregs[0] & 0x0F) | 0x20);
            sregs[1] = segmentData[1];
            break;
        case 2:
            sregs[0] = ((sregs[0] & 0x0F) | 0x40);
            sregs[1] = segmentData[2];
            break;
        case 3:
            sregs[0] = ((sregs[0] & 0x0F) | 0x80);
            sregs[1] = segmentData[3];
            break;
        default:
            digitTurn = 0;
            break;
        }
        sendToShiftReg(sregs, NO_OF_REGS);
        if (++digitTurn > 3)
        {
            digitTurn = 0;
        }
    }
}
void ccDisp474::loopKeyScan(void)
  {
    curMillsScan = millis();
    if (curMillsScan - preMillsScan >= scanTime)
    {
      pbResult.pb_result8 = 0;

      pbPins.pbbits.pb1Pre = pbPins.pbbits.pb1Cur;
      pbPins.pbbits.pb2Pre = pbPins.pbbits.pb2Cur;
      pbPins.pbbits.pb3Pre = pbPins.pbbits.pb3Cur;
      pbPins.pbbits.pb4Pre = pbPins.pbbits.pb4Cur;

      sregs[0] = (sregs[0] & 0xF0) | 0x01;
      sendToShiftReg(sregs,NO_OF_REGS);
      pbPins.pbbits.pb1Cur = digitalRead(keys_Pin);

      sregs[0] = (sregs[0] & 0xF0) | 0x02;
      sendToShiftReg(sregs,NO_OF_REGS);
      pbPins.pbbits.pb2Cur = digitalRead(keys_Pin);

      sregs[0] = (sregs[0] & 0xF0) | 0x04;
      sendToShiftReg(sregs,NO_OF_REGS);
      pbPins.pbbits.pb3Cur = digitalRead(keys_Pin);

      sregs[0] = (sregs[0] & 0xF0) | 0x08;
      sendToShiftReg(sregs,NO_OF_REGS);
      pbPins.pbbits.pb4Cur = digitalRead(keys_Pin);

      if(pbPins.pbbits.pb1Cur^pbPins.pbbits.pb1Pre)
      {
        if(pbPins.pbbits.pb1Cur)
        {
          pbResult.pbResultBits.pb1Rise=1;
        }
        else 
        {
          pbResult.pbResultBits.pb1Fall=1;
        }
      }

      if(pbPins.pbbits.pb2Cur^pbPins.pbbits.pb2Pre)
      {
        if(pbPins.pbbits.pb2Cur)
        {
          pbResult.pbResultBits.pb2Rise=1;
        }
        else 
        {
          pbResult.pbResultBits.pb2Fall=1;
        }
      }

      if(pbPins.pbbits.pb3Cur^pbPins.pbbits.pb3Pre)
      {
        if(pbPins.pbbits.pb3Cur)
        {
          pbResult.pbResultBits.pb3Rise=1;
        }
        else 
        {
          pbResult.pbResultBits.pb3Fall=1;
        }
      }

      if(pbPins.pbbits.pb4Cur^pbPins.pbbits.pb4Pre)
      {
        if(pbPins.pbbits.pb4Cur)
        {
          pbResult.pbResultBits.pb4Rise=1;
        }
        else 
        {
          pbResult.pbResultBits.pb4Fall=1;
        }
      }
    }
  }
bool ccDisp474::getPbRising(pushButon pbNo)
{
    switch (pbNo)
    {
    case 1:
        return pbResult.pbResultBits.pb1Rise;
        break;
    case 2:
        return pbResult.pbResultBits.pb2Rise;
        break;
    case 3:
        return pbResult.pbResultBits.pb3Rise;
        break;    
    case 4:
        return pbResult.pbResultBits.pb4Rise;
        break;    
    default:
        return 0;
        break;
    }
}
bool ccDisp474::getPbFalling(pushButon pbNo)
{
    switch (pbNo)
    {
    case 1:
        return pbResult.pbResultBits.pb1Fall;
        break;
    case 2:
        return pbResult.pbResultBits.pb2Fall;
        break;
    case 3:
        return pbResult.pbResultBits.pb3Fall;
        break;    
    case 4:
        return pbResult.pbResultBits.pb4Fall;
        break;    
    default:
        return 0;
        break;
    }
}
void ccDisp474::setRefreshTime(uint8_t rR)
{
    refreshTime = rR;
}
void ccDisp474::setKeyScanTime(uint16_t sR)
{
    scanTime = sR;
}

