#include <hidboot.h>
#include <usbhub.h>

#include <TVout.h>
#include <fontALL.h>

#include <SD.h>


TVout TV;

String cmd;
String opts;
boolean op;

String cd;

class KbdRptParser : public KeyboardReportParser
{
        void PrintKey(uint8_t mod, uint8_t key);

protected:
        virtual void OnControlKeysChanged(uint8_t before, uint8_t after);

	virtual void OnKeyDown	(uint8_t mod, uint8_t key);
	virtual void OnKeyUp	(uint8_t mod, uint8_t key);
	virtual void OnKeyPressed(uint8_t key);
};

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
    MODIFIERKEYS mod;
    *((uint8_t*)&mod) = m;
    Serial.print((mod.bmLeftCtrl   == 1) ? "C" : " ");
    Serial.print((mod.bmLeftShift  == 1) ? "S" : " ");
    Serial.print((mod.bmLeftAlt    == 1) ? "A" : " ");
    Serial.print((mod.bmLeftGUI    == 1) ? "G" : " ");

    Serial.print(" >");
    PrintHex<uint8_t>(key, 0x80);
    Serial.print("< ");

    Serial.print((mod.bmRightCtrl   == 1) ? "C" : " ");
    Serial.print((mod.bmRightShift  == 1) ? "S" : " ");
    Serial.print((mod.bmRightAlt    == 1) ? "A" : " ");
    Serial.println((mod.bmRightGUI    == 1) ? "G" : " ");
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
    Serial.print("DN ");
    PrintKey(mod, key);
    uint8_t c = OemToAscii(mod, key);

    if (c)
        OnKeyPressed(c);
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {

    MODIFIERKEYS beforeMod;
    *((uint8_t*)&beforeMod) = before;

    MODIFIERKEYS afterMod;
    *((uint8_t*)&afterMod) = after;

    if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl) {
        Serial.println("LeftCtrl changed");
    }
    if (beforeMod.bmLeftShift != afterMod.bmLeftShift) {
        Serial.println("LeftShift changed");
    }
    if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt) {
        Serial.println("LeftAlt changed");
    }
    if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI) {
        Serial.println("LeftGUI changed");
    }

    if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl) {
        Serial.println("RightCtrl changed");
    }
    if (beforeMod.bmRightShift != afterMod.bmRightShift) {
        Serial.println("RightShift changed");
    }
    if (beforeMod.bmRightAlt != afterMod.bmRightAlt) {
        Serial.println("RightAlt changed");
    }
    if (beforeMod.bmRightGUI != afterMod.bmRightGUI) {
        Serial.println("RightGUI changed");
    }

}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
    Serial.print("UP ");
    PrintKey(mod, key);
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
    
    if(key < 30) {
        TV.println();
//do command
        if (cmd == "ls") {
            File root = SD.open(cd);
            while(true) {
     
             File entry =  root.openNextFile();
             if (! entry) {
               // no more files
               //Serial.println("**nomorefiles**");
               break;
             }
             TV.print(entry.name());
             if (entry.isDirectory()) {
               TV.println("/");
             } else {
               // files have sizes, directories do not
               TV.print("      ");
               TV.println(entry.size(), DEC);
             }
             entry.close();
            }

        }
        else if (cmd == "df") {
            TV.println("Stats for /:");
            File root = SD.open("/");
            TV.print("size: ");
            TV.println(root.size());
            root.close();
        }
        else if (cmd == "uptime") {
            TV.print("Up for ");
            TV.print(millis());
            TV.println("ms");
        }
        else if (cmd == "halt") {
            TV.print("[");
            TV.print(millis()/1000.0);
            TV.print("]: System halted");
            exit(0);
        }
        else if (cmd == "cat") {
            File f = SD.open(cd + opts);
            if (dataFile) {
   		 while (dataFile.available()) {
   		   TV.write(dataFile.read());
  		  }
  		  dataFile.close();
 	   } else {
 	   	TV.print("File not found");
 	   }
  
        }
       // Serial.println(cmd);
        //TV.println();
        TV.print("root@arduino:"+cd+"$ ");
        cmd = "";
    } else {
    	if ((char)key == ' ') {
    	  op = true;
    	} else {
    	if (op) {
    	  opts += (char)key;
    	} else {
          cmd += (char)key;
        }
        TV.print(key);
        
      }
    }
};

USB     Usb;
//USBHub     Hub(&Usb);
HIDBoot<HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

uint32_t next_time;

KbdRptParser Prs;

void setup()
{
    Serial.begin( 115200 );
   // while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
    Serial.println("Start");

    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");

    delay( 200 );

    next_time = millis() + 5000;

    HidKeyboard.SetReportParser(0, (HIDReportParser*)&Prs);

    TV.begin(_NTSC,140,96);
    TV.select_font(font6x8);
    TV.print("root@arduino:~$ ");

    cmd = "";
    opts = "";
    op = false;
    
    cd = "/";

    pinMode(53,OUTPUT);

    if (!SD.begin(44)) {
        return;
    }
}

void loop()
{
    Usb.Task();
}

