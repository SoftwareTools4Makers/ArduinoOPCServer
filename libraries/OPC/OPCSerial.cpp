#include <OPC.h>
#include "OPCSerial.h"
#include <Arduino.h>
#include <HardwareSerial.h>

void OPCSerial::setup()
{
}

void OPCSerial::sendOPCItemsMap()
{ 
  Serial.print("<0");

  for(int k=0;k<OPCItemsCount;k++) {    
    Serial.print(",");
    Serial.print(OPCItemList[k].itemID);
    Serial.print(",");
    Serial.print(int(OPCItemList[k].opcAccessRight));
    Serial.print(",");
    Serial.print(int(OPCItemList[k].itemType));
  }

  Serial.println(">");
}

OPCSerial::OPCSerial()  {
    buffer[0] = '\0';
}

void OPCSerial::processOPCCommands() {
  bool matched = false;
  char *p,*j;

  bool (*bool_callback)(const char *itemID, const opcOperation opcOP, const bool value);
  byte (*byte_callback)(const char *itemID, const opcOperation opcOP, const byte value);  
  int (*int_callback)(const char *itemID, const opcOperation opcOP, const int value);
  float (*float_callback)(const char *itemID, const opcOperation opcOP, const float value);  

  while (Serial.available() > 0) {
    char inChar = Serial.read(); 
    
    if (inChar == '\r') {     
      if (buffer[0] == '\0') 
        sendOPCItemsMap();
      else {
        // Lets search for read
        for (int i = 0; i < OPCItemsCount; i++) {            
         if (!strncmp(buffer, OPCItemList[i].itemID, SERIALCOMMAND_MAXCOMMANDLENGTH)) {                      
                        
          // Execute the stored handler function for the command  
          switch (OPCItemList[i].itemType) {
            case opc_bool :
                      bool_callback = (bool (*)(const char *itemID, const opcOperation opcOP, const bool value))(OPCItemList[i].ptr_callback);
                      Serial.println(bool_callback(OPCItemList[i].itemID,opc_opread,NULL));                      
                      break;
            case opc_byte :
                      byte_callback = (byte (*)(const char *itemID, const opcOperation opcOP, const byte value))(OPCItemList[i].ptr_callback);
                      Serial.println(byte_callback(OPCItemList[i].itemID,opc_opread,NULL));
                      break;
            case opc_int : 
                      int_callback = (int (*)(const char *itemID, const opcOperation opcOP, const int value))(OPCItemList[i].ptr_callback);
                      Serial.println(int_callback(OPCItemList[i].itemID,opc_opread,NULL));
                      break;
            case opc_float : 
                      float_callback = (float (*)(const char *itemID, const opcOperation opcOP, const float value))(OPCItemList[i].ptr_callback);
                      Serial.println(float_callback(OPCItemList[i].itemID,opc_opread,NULL));
                      break;                      

          }          

          matched = true;
          break;
          } /* endif */
        } /* endfor */

        if (!matched) {
          // Lets search for write
          p = strtok_r(buffer,"=",&j);
          for (int i = 0; i < OPCItemsCount; i++) {            
            if (!strncmp(p, OPCItemList[i].itemID, SERIALCOMMAND_MAXCOMMANDLENGTH)) {                      

              // Call the stored handler function for the command                          
              switch (OPCItemList[i].itemType) {
              case opc_bool :
                      bool_callback = (bool (*)(const char *itemID, const opcOperation opcOP, const bool value))(OPCItemList[i].ptr_callback);
                      bool_callback(OPCItemList[i].itemID,opc_opwrite,atoi(j));
                      break;
              case opc_byte :
                      byte_callback = (byte (*)(const char *itemID, const opcOperation opcOP, const byte value))(OPCItemList[i].ptr_callback);
                      byte_callback(OPCItemList[i].itemID,opc_opwrite,atoi(j));
                      break;
              case opc_int : 
                      int_callback = (int (*)(const char *itemID, const opcOperation opcOP, const int value))(OPCItemList[i].ptr_callback);
                      int_callback(OPCItemList[i].itemID,opc_opwrite,atoi(j));
                      break;
              case opc_float : 
                      float_callback = (float (*)(const char *itemID, const opcOperation opcOP, const float))(OPCItemList[i].ptr_callback);
                      float_callback(OPCItemList[i].itemID,opc_opwrite,atof(j));
                      break;                      
              } 

              break;
            } /* endif */         
          } /* endfor */
        }
      }

      buffer[0] = '\0';
      bufPos = 0;
    }
    else {
      if (bufPos < SERIALCOMMAND_BUFFER) {
        buffer[bufPos++] = inChar;
        buffer[bufPos] = '\0';
      }
    }
  }
}


