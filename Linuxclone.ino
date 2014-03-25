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
    //empty
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
    //empty
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {

    //empty

}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
    //empty
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
    
    if(key < 30) {
        TV.println();
//do command
        if (cmd == "ls") {
            char filename[cd.length()+1];
            cd.toCharArray(filename, sizeof(filename));
            File root = SD.open(filename);
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
            String temp = cd+opts;
            char filename[temp.length()+1];
            temp.toCharArray(filename, sizeof(filename));
            File dataFile = SD.open(filename);
            if (dataFile) {
   		 while (dataFile.available()) {
   		   TV.write(dataFile.read());
  		  }
  		  dataFile.close();
 	      } else {
 	       	TV.println("File not found");
 	       }
  
        }
        else if (cmd == "cd") {
            cd = opts;
        }
       // Serial.println(cmd);
        //TV.println();
        char filename[cd.length()+1];
        cd.toCharArray(filename, sizeof(filename));
        TV.print("root@arduino:");
        TV.print(filename);
        TV.print("$ ");
        cmd = "";
        opts = "";
        op = false;
    } else {
    	if ((char)key == ' ') {
    	  op = true;
    	}
    	if (op) {
    	  opts += (char)key;
    	} else {
          cmd += (char)key;
        }
        TV.print(key);
        
      
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
    TV.print("root@arduino:/$ ");

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

