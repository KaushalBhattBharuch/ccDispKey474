#include <ccDisp474.h>

/*Check the functionality of keys on boot up etc..*/
//ccDisp474(data_pin, lat_pin, clk_pin, keys_pin);
ccDisp474 dispKey(12, 13, 4, 14);
float a=0;
uint8_t refTime=6;
void setup()
{
  Serial.begin(115200);
  // put your setup code here, to run once:
  /*Set 7 Segment refresh time and Key scan time in milli seconds
   *Default segment refresh time is 6 ms;
   *Default key scan time is 250 ms
   *If needed to change use following 
   */

  dispKey.setRefreshTime(refTime);
  dispKey.setKeyScanTime(100);
}

void loop()
{
  // put your main code here, to run repeatedly:
   
  /* 
   * dispKey.loopDisp(); method will update the disiplay every RefreshTime interval
   * in non-blocking mode.
   * dispKey.loopKeyScan(); method scans key at every ScanTime interval in non 
   * blocking mode.
   */
  dispKey.loopDisp();
  dispKey.loopKeyScan();
  if(dispKey.getPbRising(KI1))
  {
    refTime++;
    dispKey.setRefreshTime(refTime);
  }
  if(dispKey.getPbFalling(KI2))
  {
    refTime--;
    dispKey.setRefreshTime(refTime);
  }  
  dispKey.display(refTime);
}
