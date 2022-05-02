/* 
* FULL CREDIT OF THIS SOURCE FILE TO @gr3gthepilot
* https://www.unknowncheats.me/forum/valorant/469268-yolo-aim-augmentation-v2-0-a.html
*/

#include <Mouse.h>
#include <Wire.h>
#include <SPI.h>
 
#include <usbhub.h>
USB     Usb;
USBHub     Hub(&Usb);
 
#include <hidboot.h>
HIDBoot<USB_HID_PROTOCOL_MOUSE>    HidMouse(&Usb);
 

String myString;
int j = 0;
int c = 0;
int e = 0;
 
int lmb = 0;
int rmb = 0;
int mmb = 0;
int scroll = 0;
 
int dx;
int dy;
 
int arr[2];
int arrv[8];

// ----- Mouse Report Parser
class MouseRptParser : public MouseReportParser{
  protected:
    //void OnWheelMove  (MOUSEINFO *mi);
    void OnMouseMove  (MOUSEINFO *mi);
    void OnLeftButtonUp (MOUSEINFO *mi);
    void OnLeftButtonDown (MOUSEINFO *mi);
    void OnRightButtonUp  (MOUSEINFO *mi);
    void OnRightButtonDown  (MOUSEINFO *mi);
    void OnMiddleButtonUp (MOUSEINFO *mi);
    void OnMiddleButtonDown (MOUSEINFO *mi);
};
 
 
void MouseRptParser::OnMouseMove(MOUSEINFO *mi){
  dx = mi->dX;
  dy = mi->dY;};
 
 
void MouseRptParser::OnLeftButtonUp (MOUSEINFO *mi){
  lmb = 0;};
 
 
void MouseRptParser::OnLeftButtonDown (MOUSEINFO *mi){
  lmb = 1;};
 
 
void MouseRptParser::OnRightButtonUp  (MOUSEINFO *mi){
  rmb = 0;};
 
 
void MouseRptParser::OnRightButtonDown  (MOUSEINFO *mi){
  rmb = 1;};
 
 
void MouseRptParser::OnMiddleButtonUp (MOUSEINFO *mi){
  mmb = 0;};
 
 
void MouseRptParser::OnMiddleButtonDown (MOUSEINFO *mi){
  mmb = 1;};
 
/*void MouseRptParser::OnWheelMove(MOUSEINFO *mi)  {
  scroll = mi->dZ;};*/
  
MouseRptParser                               Prs;

void setup() {
  delay(5000);
  Mouse.begin();
  
  Serial.begin(115200);
  Serial.setTimeout(1);  

  Usb.Init();
  HidMouse.SetReportParser(0, &Prs);
  
}

void loop() {
  dx = 0;
  dy = 0;

  j = 0;
  c = 0;
  e = 0;

  Usb.Task();
 
  //Clicking
  if (lmb == 0){
    Mouse.release(MOUSE_LEFT);}
    
  else if (lmb == 1){
    Mouse.press(MOUSE_LEFT);}
 
  if (rmb == 0){
    Mouse.release(MOUSE_RIGHT);}
    
  else if (rmb == 1){
    Mouse.press(MOUSE_RIGHT);}
 
  if (mmb == 0){
    Mouse.release(MOUSE_MIDDLE);}
    
  else if (mmb == 1){
    Mouse.press(MOUSE_MIDDLE);}
 
 
  //Moving the mouse with pcinput    
  if (Serial.available()){
    
    myString = Serial.readString();  
 
    for (int k = 0; k <= 1; k++){    
      String val = getValue(myString, ':', k);      
      arr[k] = val.toInt();
      }
 
      arr[0] += dx;
      arr[1] += dy;
 
    for (e = 0; e < (sizeof(arr) / sizeof(arr[0])); e++) {   
 
      if (arr[e] > 0){
        
        for (j = 0; j <= (arr[e] / 127); j++){         
          c = j * 127;         
                  
          if (int(arr[e]) > (c + 127)){            
            arrv[j] = 127;                                                               
            }
          
          else {
            arrv[j] = arr[e] - c;                             
            }             
          }                                       
        }
        
        else {
        
          for (j = 0; j <= (arr[e] / -127); j++){         
            c = j * -127;         
                  
            if (arr[e] < (c - 127)){
              arrv[j] = -127;                                           
              }
           
            else {
              arrv[j] = arr[e] - c;                     
              }                            
            }        
          } 
        
      for (int p = 0; p < j; p++){
 
            if (e == 0){
              //Serial.println("X: " + String(m));
              Mouse.move(arrv[p], 0);
              }
              
            else if (e == 1){
              //Serial.println("Y: " + String(m));
              Mouse.move(0, arrv[p]);
              }                       
            }
        
      memset(arrv, 0, sizeof(arrv));  
      }              
    }
 
  //Moving the mouse without pcinput
  else {
    Mouse.move(dx, dy);
    }
  

}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
 
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}