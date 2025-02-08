#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <DS1302.h>
#include <EEPROM.h>
#define REQ_BUF_SZ   120

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 222);
EthernetServer server(80);
File webFile;
char HTTP_req[REQ_BUF_SZ] = {0};
char req_index = 0;
char saatdk[8] = {0};
char idpw[12]={0};
boolean Modlar[1] = {0};
boolean Giris=0;
boolean LED_state[9] = {0};
boolean Sensorler[4] = {0};
boolean Hirsiz = 0;
boolean f=0;
int sakat[15]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
boolean perde[10]= {0};
String veriler[2];
String saatdkler[32]={"","","","","","","","","","","","","","","","","",""};

Time day;
Time t;
Time sn;
DS1302 rtc(19,20,21);

void setup()
{  
  t=rtc.getTime();
//  day=t.getDOWStr();
  pinMode(10, OUTPUT); 
  digitalWrite(10, HIGH);
  
  Serial.begin(9600);
  
  Serial.println("SD kart araniyor...");
  if (!SD.begin(4)) {
    Serial.println("SD kart bulunamadı");
    return;
  }
  Serial.println("SD kart bulundu");
  // check for index.htm file
  if (!SD.exists("index.htm")) {
    Serial.println("index.htm bulunamadı");
  }
  Serial.println("index.htm bulundu");
  saatdkler[0]=String(char(EEPROM.read(0)))+String(char(EEPROM.read(1))); saatdkler[1]=String(char(EEPROM.read(2)))+String(char(EEPROM.read(3))); //Zamanlı çıkış 1 başlangıç saat dakikası 
  saatdkler[2]=String(char(EEPROM.read(4)))+String(char(EEPROM.read(5))); saatdkler[3]=String(char(EEPROM.read(6)))+String(char(EEPROM.read(7))); //Zamanlı çıkış 1 bitiş saat dakikası
  saatdkler[4]=String(char(EEPROM.read(20)))+String(char(EEPROM.read(21))); saatdkler[5]=String(char(EEPROM.read(22)))+String(char(EEPROM.read(23))); //Zamanlı çıkış 2 başlangıç saat dakikası 
  saatdkler[6]=String(char(EEPROM.read(24)))+String(char(EEPROM.read(25))); saatdkler[7]=String(char(EEPROM.read(26)))+String(char(EEPROM.read(27))); //Zamanlı çıkış 2 bitiş saat dakikası
  saatdkler[8]=String(char(EEPROM.read(28)))+String(char(EEPROM.read(29))); saatdkler[9]=String(char(EEPROM.read(30)))+String(char(EEPROM.read(31))); //Zamanlı çıkış 3 başlangıç saat dakikası 
  saatdkler[10]=String(char(EEPROM.read(32)))+String(char(EEPROM.read(33))); saatdkler[11]=String(char(EEPROM.read(34)))+String(char(EEPROM.read(35))); //Zamanlı çıkış 3 bitiş saat dakikası
  saatdkler[12]=String(char(EEPROM.read(36)))+String(char(EEPROM.read(37))); saatdkler[13]=String(char(EEPROM.read(38)))+String(char(EEPROM.read(39))); //Zamanlı çıkış 4 başlangıç saat dakikası 
  saatdkler[14]=String(char(EEPROM.read(40)))+String(char(EEPROM.read(41))); saatdkler[15]=String(char(EEPROM.read(42)))+String(char(EEPROM.read(43))); //Zamanlı çıkış 4 bitiş saat dakikası
  pinMode(22, OUTPUT); pinMode(23, OUTPUT); pinMode(24, OUTPUT); pinMode(25, OUTPUT); pinMode(26, OUTPUT);
  pinMode(27, OUTPUT); pinMode(28, OUTPUT); pinMode(29, OUTPUT); pinMode(30, OUTPUT);
  pinMode(31, OUTPUT); pinMode(32, OUTPUT); pinMode(33, OUTPUT);  pinMode(35, OUTPUT);
  pinMode(36,OUTPUT); pinMode(37,OUTPUT); pinMode(38,OUTPUT); pinMode(39,OUTPUT); 
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop()
{
  ZamanliCikislar();
  PerdeAc(); 
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // limit the size of the stored received HTTP request
        // buffer first part of HTTP request in HTTP_req array (string)
        // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c;          // save HTTP request character
          req_index++;
        }
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          // remainder of header follows below, depending on if
          // web page or XML page is requested
          // Ajax request - send XML file
          if (StrContains(HTTP_req, "ajax_inputs")) {
            // send rest of HTTP header
            client.println("Content-Type: text/html");
            client.println("Connection: keep-alive");
            client.println();
            Mod();
            SetLEDs();
            EthernetClient client = server.available();
            if(client)
            {
                client.println("<script>GetArduinoIO()</script>");  
            }
            //XML_response(client);
          }
          else {  // web page request
            // send rest of HTTP header
            client.println("Content-Type: text/html");
            client.println("Connection: keep-alive");
            client.println();
            // send web page
            if (StrContains(HTTP_req, "GET / ")) 
            {
              webFile = SD.open("giris.htm");        // open web page file
              if (webFile) 
              {
               while (webFile.available()) 
                {
                client.write(webFile.read()); // send web page to client
                
                }
              webFile.close();
              }
            }
            else if (StrContains(HTTP_req, "GET /odalar.htm"))
            {
              webFile = SD.open("odalar.htm");        // open web page file
              if (webFile) 
              {
               while (webFile.available()) 
                {
                client.write(webFile.read()); // send web page to client
               
                }
              webFile.close();
              }
            }
            else if (StrContains(HTTP_req, "GET /index.htm"))
            {
              webFile = SD.open("index.htm");        // open web page file
              if (webFile) 
              {
               while (webFile.available()) 
                {
                client.write(webFile.read()); // send web page to client
               
                }
              webFile.close();
              }
            }
            else if (StrContains(HTTP_req, "GET /cihazlar.htm"))
            {
              webFile = SD.open("cihazlar.htm");        // open web page file
              if (webFile) 
              {
               while (webFile.available()) 
                {
                client.write(webFile.read()); // send web page to client
               
                }
              webFile.close();
              }
            }
            else if (StrContains(HTTP_req, "GET /baglanti.htm"))
            {
              webFile = SD.open("baglanti.htm");        // open web page file
              if (webFile) 
              {
               while (webFile.available()) 
                {
                client.write(webFile.read()); // send web page to client
               
                }
              webFile.close();
              }
            }
            else if (StrContains(HTTP_req, "GET /paneller.htm "))
            {
              webFile = SD.open("paneller.htm");        // open web page file
              if (webFile) 
              {
               while (webFile.available()) 
                {
                client.write(webFile.read()); // send web page to client
               
                }
              webFile.close();
              }
            }
            else if (StrContains(HTTP_req, "GET /ayarlar.htm "))
            {
              webFile = SD.open("ayarlar.htm");        // open web page file
              if (webFile) 
              {
               while (webFile.available()) 
                {
                client.write(webFile.read()); // send web page to client
               
                }
              webFile.close();
              }
            }
            else if (StrContains(HTTP_req, "GET /sayfa.htm "))
            {
              webFile = SD.open("sayfa.htm");        // open web page file
              if (webFile) 
              {
               while (webFile.available()) 
                {
                client.write(webFile.read()); // send web page to client
               
                }
              webFile.close();
              }
            }
          }
          // display received HTTP request on serial port
         // Serial.print(HTTP_req);
          // reset buffer index and all buffer elements to 0
          Serial.print(HTTP_req);
          req_index = 0;
          StrClear(HTTP_req, REQ_BUF_SZ);
          break;
        }
        // every line of text received from the client ends with \r\n
        if (c == '\n') {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;
        }
      } // end if (client.available())
    } // end while (client.connected())
    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  } // end if (client)
}

// checks if received HTTP request is switching on/off LEDs
// also saves the state of the LEDs
void SetLEDs(void)
{
   if (StrContains(HTTP_req, "Mod=1")) {
      Modlar[0] = 1;
    }
    else if (StrContains(HTTP_req, "Mod=0")) {
    Modlar[0] = 0;
    }
  // LED 1 (pin 6)
  if (StrContains(HTTP_req, "Led1=1")) {
    LED_state[0] = 1;  // save LED state
    digitalWrite(22, HIGH);
    }
  else if (StrContains(HTTP_req, "Led1=0")) {
    LED_state[0] = 0;  // save LED state
    digitalWrite(22, LOW);
  }
  // LED 2 (pin 7)
  if (StrContains(HTTP_req, "Led2=1")) {
    LED_state[1] = 1;
    // save LED state
    digitalWrite(23,HIGH);
  }
  else if (StrContains(HTTP_req, "Led2=0")) {
    LED_state[1] = 0;
    digitalWrite(23,LOW);
  }
  if (StrContains(HTTP_req, "Led3=1")) {
    LED_state[2] = 1;  // save LED state
    digitalWrite(24, HIGH);
  }
  else if (StrContains(HTTP_req, "Led3=0")) {
    LED_state[2] = 0;  // save LED state
    digitalWrite(24, LOW);
  }
  if (StrContains(HTTP_req, "Led4=1")) {
    LED_state[3] = 1;  // save LED state
    digitalWrite(25, HIGH);
  }
  else if (StrContains(HTTP_req, "Led4=0")) {
    LED_state[3] = 0;  // save LED state
    digitalWrite(25, LOW);
  }
  // LED 4 (pin 9)
  if (StrContains(HTTP_req, "Led5=1")) {
    LED_state[4] = 1;  // save LED state
    digitalWrite(26, HIGH);
  }
  else if (StrContains(HTTP_req, "Led5=0")) {
    LED_state[4] = 0;  // save LED state
    digitalWrite(26, LOW);
  }
  if (StrContains(HTTP_req, "Led6=1")) {
    LED_state[5] = 1;  // save LED state
    digitalWrite(27, HIGH);
  }
  else if (StrContains(HTTP_req, "Led6=0")) {
    LED_state[5] = 0;  // save LED state
    digitalWrite(27, LOW);
  }
  if (StrContains(HTTP_req, "Led7=1")) {
    LED_state[6] = 1;  // save LED state
    digitalWrite(28, HIGH);
    
  }
  else if (StrContains(HTTP_req, "Led7=0")) {
    LED_state[6] = 0;  // save LED state
    digitalWrite(28, LOW);
    
  }
  if (StrContains(HTTP_req, "Led8=1")) {
    LED_state[7] = 1;  // save LED state
    //digitalWrite(31, HIGH);
    
  }
  else if (StrContains(HTTP_req, "Led8=0")) {
    LED_state[7] = 0;  // save LED state
    //digitalWrite(31, LOW);
    perde[1]=0;
  }
  if (StrContains(HTTP_req, "Led9=1")) {
    LED_state[8] = 1;  // save LED state
    perde[0]=1;
    t=rtc.getTime();
    sn.sec=t.sec; 
    delay(1000);
  }
  else if (StrContains(HTTP_req, "Led9=0")) {
    LED_state[8] = 0;  // save LED state
    perde[0]=0;
  }
  if (StrContains(HTTP_req, "Led10=1")) {
    LED_state[9] = 1;  // save LED state
    perde[1]=1;
    t=rtc.getTime();
    sn.sec=t.sec;
    delay(1000);
  }
  else if (StrContains(HTTP_req, "Led10=0")) {
    LED_state[9] = 0;  // save LED state
    perde[0]=0;
  }
  if (StrContains(HTTP_req, "Led11=1")) {
    LED_state[10] = 1;  // save LED state
    digitalWrite(31, HIGH);
    
  }
  else if (StrContains(HTTP_req, "Led11=0")) {
    LED_state[10] = 0;  // save LED state
    digitalWrite(31, LOW);
    
  }
  if (StrContains(HTTP_req, "Led12=1")) {
    LED_state[11] = 1;  // save LED state
    digitalWrite(32, HIGH);
    
  }
  else if (StrContains(HTTP_req, "Led12=0")) {
    LED_state[11] = 0;  // save LED state
    digitalWrite(32, LOW);
    
  }
  if (StrContains(HTTP_req, "y"))
  {
    idpw[0]=HTTP_req[16]; idpw[1]=HTTP_req[17]; idpw[2]=HTTP_req[18]; idpw[3]=HTTP_req[19];
    idpw[4]=HTTP_req[20]; idpw[5]=HTTP_req[21]; idpw[6]=HTTP_req[22]; idpw[7]=HTTP_req[23];
    idpw[8]=HTTP_req[24]; idpw[9]=HTTP_req[25]; idpw[10]=HTTP_req[26]; idpw[11]=HTTP_req[27];
    veriler[0]=String(idpw[0])+String(idpw[1])+String(idpw[2])+String(idpw[3])+String(idpw[4])+String(idpw[5]);
    veriler[1]=String(idpw[6])+String(idpw[7])+String(idpw[8])+String(idpw[9])+String(idpw[10])+String(idpw[11]);
    Serial.println(veriler[0]); Serial.println(veriler[0]);
    if(veriler[0]==
    (
      String(char(EEPROM.read(8)))+
      String(char(EEPROM.read(9)))+
      String(char(EEPROM.read(10)))+
      String(char(EEPROM.read(11)))+
      String(char(EEPROM.read(12)))+
      String(char(EEPROM.read(13))))
      &&
        veriler[1]==
    (
      String(char(EEPROM.read(14)))+
      String(char(EEPROM.read(15)))+
      String(char(EEPROM.read(16)))+
      String(char(EEPROM.read(17)))+
      String(char(EEPROM.read(18)))+
      String(char(EEPROM.read(19)))))
    {
      Giris=true; Serial.println("Giriş başarılı");
    }
  }
  
  if (StrContains(HTTP_req, "z"))
  {
    if(StrContains(HTTP_req,"z1"))
    {
      saatdk[0]=HTTP_req[16];
      saatdk[1]=HTTP_req[17];
      saatdk[2]=HTTP_req[18];
      saatdk[3]=HTTP_req[19];
      saatdk[4]=HTTP_req[20];
      saatdk[5]=HTTP_req[21];
      saatdk[6]=HTTP_req[22];
      saatdk[7]=HTTP_req[23];
  
      for(int i=0;i<8;i++)
      {
        EEPROM.write(i,saatdk[i]);  
      }
      saatdkler[0]=String(char(EEPROM.read(0)))+String(char(EEPROM.read(1))); saatdkler[1]=String(char(EEPROM.read(2)))+String(char(EEPROM.read(3))); //Zamanlı çıkış 1 başlangıç saat dakikası 
      saatdkler[2]=String(char(EEPROM.read(4)))+String(char(EEPROM.read(5))); saatdkler[3]=String(char(EEPROM.read(6)))+String(char(EEPROM.read(7))); //Zamanlı çıkış 1 bitiş saat dakikası
    }
    if(StrContains(HTTP_req,"z2"))
    {
      saatdk[0]=HTTP_req[16];
      saatdk[1]=HTTP_req[17];
      saatdk[2]=HTTP_req[18];
      saatdk[3]=HTTP_req[19];
      saatdk[4]=HTTP_req[20];
      saatdk[5]=HTTP_req[21];
      saatdk[6]=HTTP_req[22];
      saatdk[7]=HTTP_req[23];
  
      for(int i=20;i<28;i++)
      {
        EEPROM.write(i,saatdk[i-20]);  
      }
      saatdkler[4]=String(char(EEPROM.read(20)))+String(char(EEPROM.read(21))); saatdkler[5]=String(char(EEPROM.read(22)))+String(char(EEPROM.read(23))); //Zamanlı çıkış 2 başlangıç saat dakikası 
      saatdkler[6]=String(char(EEPROM.read(24)))+String(char(EEPROM.read(25))); saatdkler[7]=String(char(EEPROM.read(26)))+String(char(EEPROM.read(27))); //Zamanlı çıkış 2 bitiş saat dakikası
    }
    if(StrContains(HTTP_req,"z3"))
    {
      saatdk[0]=HTTP_req[16];
      saatdk[1]=HTTP_req[17];
      saatdk[2]=HTTP_req[18];
      saatdk[3]=HTTP_req[19];
      saatdk[4]=HTTP_req[20];
      saatdk[5]=HTTP_req[21];
      saatdk[6]=HTTP_req[22];
      saatdk[7]=HTTP_req[23];
  
      for(int i=28;i<36;i++)
      {
        EEPROM.write(i,saatdk[i-28]);  
      }
      saatdkler[8]=String(char(EEPROM.read(28)))+String(char(EEPROM.read(29))); saatdkler[9]=String(char(EEPROM.read(30)))+String(char(EEPROM.read(31))); //Zamanlı çıkış 3 başlangıç saat dakikası 
      saatdkler[10]=String(char(EEPROM.read(32)))+String(char(EEPROM.read(33))); saatdkler[11]=String(char(EEPROM.read(34)))+String(char(EEPROM.read(35))); //Zamanlı çıkış 3 bitiş saat dakikası
    }
    if(StrContains(HTTP_req,"z4"))
    {
      saatdk[0]=HTTP_req[16];
      saatdk[1]=HTTP_req[17];
      saatdk[2]=HTTP_req[18];
      saatdk[3]=HTTP_req[19];
      saatdk[4]=HTTP_req[20];
      saatdk[5]=HTTP_req[21];
      saatdk[6]=HTTP_req[22];
      saatdk[7]=HTTP_req[23];
  
      for(int i=36;i<44;i++)
      {
        EEPROM.write(i,saatdk[i-36]);  
      }
      saatdkler[12]=String(char(EEPROM.read(36)))+String(char(EEPROM.read(37))); saatdkler[13]=String(char(EEPROM.read(38)))+String(char(EEPROM.read(39))); //Zamanlı çıkış 4 başlangıç saat dakikası 
      saatdkler[14]=String(char(EEPROM.read(40)))+String(char(EEPROM.read(41))); saatdkler[15]=String(char(EEPROM.read(42)))+String(char(EEPROM.read(43))); //Zamanlı çıkış 4 bitiş saat dakikası
    }
  }
}
void PerdeAc()
{      
  t=rtc.getTime();
  switch(perde[0])
  {
    case 0: digitalWrite(29,LOW); break;
    case 1: 
    if(sn.sec!=t.sec)
      {
          digitalWrite(29,HIGH);
      }
      else
      {
          perde[0]=0;
          digitalWrite(29,LOW);
      }
    break;
  } 
  switch(perde[1])
  {
    case 0: digitalWrite(30,LOW); break;
    case 1: 
    if(sn.sec!=t.sec)
      {
          digitalWrite(30,HIGH);
      }
      else
      {
          perde[1]=0;
          digitalWrite(30,LOW);
      }
    break;
  }
}
 void XML_response(EthernetClient cl)
{
  cl.print("<?xml version = \"1.0\" ?>");
  cl.print("<inputs>");
  cl.print("<LED>");
  if (LED_state[0]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");
  // LED4
  cl.print("<LED>");
  if (LED_state[1]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");

  cl.print("<LED>");
  if (LED_state[2]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");

  cl.print("<LED>");
  if (LED_state[3]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");

  cl.print("<LED>");
  if (LED_state[4]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");

  cl.print("<LED>");
  if (LED_state[5]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");

  cl.print("<LED>");
  if (LED_state[6]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");

  cl.print("<LED>");
  if (LED_state[7]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");

  cl.print("<LED>");
  if (LED_state[8]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");

  cl.print("<LED>");
  if (LED_state[9]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");
  cl.print("<LED>");
  if (LED_state[10]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");
  cl.print("<LED>");
  if (LED_state[11]) {
    cl.print("on");
  }
  else {
    cl.print("off");
  }
  cl.println("</LED>");
  
  t=rtc.getTime();
  cl.print("<zaman>");
  cl.print(t.hour+":");
  cl.print(t.min);
  cl.println("</zaman>");
  cl.print("<zaman0>");
  cl.print(saatdkler[0]);
  cl.println("</zaman0>");
  cl.print("<zaman1>");
  cl.print(saatdkler[1]);
  cl.println("</zaman1>");
  cl.print("<zaman2>");
  cl.print(saatdkler[2]);
  cl.println("</zaman2>");
  cl.print("<zaman3>");
  cl.print(saatdkler[3]);
  cl.println("</zaman3>");
  cl.print("<zaman4>");
  cl.print(saatdkler[4]);
  cl.println("</zaman4>");
  cl.print("<zaman5>");
  cl.print(saatdkler[5]);
  cl.println("</zaman5>");
  cl.print("<zaman6>");
  cl.print(saatdkler[6]);
  cl.println("</zaman6>");
  cl.print("<zaman7>");
  cl.print(saatdkler[7]);
  cl.println("</zaman7>");
  cl.print("<zaman8>");
  cl.print(saatdkler[8]);
  cl.println("</zaman8>");
  cl.print("<zaman9>");
  cl.print(saatdkler[9]);
  cl.println("</zaman9>");
  cl.print("<zaman10>");
  cl.print(saatdkler[10]);
  cl.println("</zaman10>");
  cl.print("<zaman11>");
  cl.print(saatdkler[11]);
  cl.println("</zaman11>");
  cl.print("<zaman12>");
  cl.print(saatdkler[12]);
  cl.println("</zaman12>");
  cl.print("<zaman13>");
  cl.print(saatdkler[13]);
  cl.println("</zaman13>");
  cl.print("<zaman14>");
  cl.print(saatdkler[14]);
  cl.println("</zaman14>");
  cl.print("<zaman15>");
  cl.print(saatdkler[15]);
  cl.println("</zaman15>");
  cl.print("<MOD>");
  if (Hirsiz) {
    cl.print("hirsiz");
  }
  else {
    cl.print("temiz");
  }
  cl.println("</MOD>");
  if(Giris)
  {
    cl.print("<giris>");
    cl.print("1");
    cl.println("</giris>");
  }
  else
  {
    cl.print("<giris>");
    cl.print("0");
    cl.println("</giris>");
  }
  cl.println("</inputs>");
}

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
  for (int i = 0; i < length; i++) {
    str[i] = 0;
  }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
  char found = 0;
  char index = 0;
  char len;

  len = strlen(str);

  if (strlen(sfind) > len) {
    return 0;
  }
  while (index < len) {
    if (str[index] == sfind[found]) {
      found++;
      if (strlen(sfind) == found) {
        return 1;
      }
    }
    else {
      found = 0;
    }
    index++;
  }

  return 0;
}
void Mod()
{
  if (Modlar[0])
  {
    int deger = 0;
    if (deger == 1)
    {
      Hirsiz = 1;
    }
    else
    {
      Hirsiz = 0;
    }
  }
  else
  {
    Hirsiz = 0;
  }
}

void ZamanliCikislar()
{
  t=rtc.getTime();   
  if(sakat[0]!=0&&sakat[2]!=0)
  {
    if(saatdkler[0].toInt()>12&&saatdkler[2].toInt()<12) //Zamanlı çıkış 1
    {
      if(t.hour<12)
      {
        if(t.hour>saatdkler[2].toInt()||(t.hour>=saatdkler[2].toInt()&&t.min>=saatdkler[3].toInt()))
        {
          if(sakat[2]==1){digitalWrite(27,LOW); LED_state[5]=0; sakat[2]=0; sakat[0]=1;} 
        }  
        else
        {
          if(sakat[0]==1){digitalWrite(27,HIGH); LED_state[5]=1; sakat[0]=0; sakat[2]=1;} 
        }
      }
      else
      {
        if(t.hour<saatdkler[0].toInt()||(t.hour<=saatdkler[0].toInt()&&t.min<=saatdkler[1].toInt()))
        {
          if(sakat[2]==1){digitalWrite(27,LOW); LED_state[5]=0; sakat[2]=0; sakat[0]=1;}
        }  
        else
        {
          if(sakat[0]==1){digitalWrite(27,HIGH); LED_state[5]=1; sakat[0]=0; sakat[2]=1;} 
        }
      }  
    }
    else
    {
      if(t.hour>saatdkler[0].toInt()||(t.hour>=saatdkler[0].toInt()&&t.min>=saatdkler[1].toInt()))
      {    
        if(t.hour>saatdkler[2].toInt()||(t.hour>=saatdkler[2].toInt()&&t.min>=saatdkler[3].toInt()))
        { 
          if(sakat[2]==1){digitalWrite(27,LOW); LED_state[5]=0; sakat[2]=0; sakat[0]=1;}
        }
        else
        { 
          if(sakat[0]==1){digitalWrite(27,HIGH); LED_state[5]=1; sakat[0]=0; sakat[2]=1;} 
        }
      }  
    }
  }
    if(saatdkler[4].toInt()>12&&saatdkler[6].toInt()<12) //Zamanlı Çıkış 2
  {
      if(t.hour<12)
      {
        if(t.hour>saatdkler[6].toInt()||(t.hour>=saatdkler[6].toInt()&&t.min>=saatdkler[7].toInt()))
        {
       if(sakat[6]==1){  digitalWrite(28,LOW); LED_state[6]=0;  sakat[6]=0; sakat[4]=1;} 
        }  
        else
        {
        if(sakat[4]==1)  {digitalWrite(28,HIGH); LED_state[6]=1; sakat[4]=0; sakat[6]=1;} 
        }
      }
      else
      {
        if(t.hour<saatdkler[4].toInt()||(t.hour<=saatdkler[4].toInt()&&t.min<=saatdkler[5].toInt()))
        {
        if(sakat[6]==1){  digitalWrite(28,LOW); LED_state[6]=0;  sakat[6]=0; sakat[4]=1;}  
        }  
        else
        {
         if(sakat[4]==1)  {digitalWrite(28,HIGH); LED_state[6]=1; sakat[4]=0; sakat[6]=1;} 
        }
      }  
    }
    else
    {
      if(t.hour>saatdkler[4].toInt()||(t.hour>=saatdkler[4].toInt()&&t.min>=saatdkler[5].toInt()))
      {    
        if(t.hour>saatdkler[6].toInt()||(t.hour>=saatdkler[6].toInt()&&t.min>=saatdkler[7].toInt()))
        { 
           if(sakat[6]==1){  digitalWrite(28,LOW); LED_state[6]=0;  sakat[6]=0; sakat[4]=1;;}   
        }
        else
        { 
           if(sakat[4]==1)  {digitalWrite(28,HIGH); LED_state[6]=1; sakat[4]=0; sakat[6]=1;}
        }
      }  
    }
  
  if(sakat[8]!=0&&sakat[10]!=0)
  {
    if(saatdkler[8].toInt()>12&&saatdkler[10].toInt()<12) //Zamanlı çıkış 3
    {
      if(t.hour<12)
      {
        if(t.hour>saatdkler[10].toInt()||(t.hour>=saatdkler[10].toInt()&&t.min>=saatdkler[11].toInt()))
        {
          perde[0]=1;  
        }  
        else
        {
          perde[1]=1; 
        }
      }
      else
      {
        if(t.hour<saatdkler[8].toInt()||(t.hour<=saatdkler[8].toInt()&&t.min<=saatdkler[9].toInt()))
        {
          perde[0]=1;  
        }  
        else
        {
          perde[1]=1; 
        }
      }  
    }
    else
    {
      if(t.hour>saatdkler[8].toInt()||(t.hour>=saatdkler[8].toInt()&&t.min>=saatdkler[9].toInt()))
      {    
        if(t.hour>saatdkler[10].toInt()||(t.hour>=saatdkler[10].toInt()&&t.min>=saatdkler[11].toInt()))
        { 
          perde[0]=1;  
        }
        else
        { 
          perde[1]=1; 
        }
      }  
    }
  }
  if(sakat[12]!=0&&sakat[14]!=0)
  {
    if(saatdkler[12].toInt()>12&&saatdkler[14].toInt()<12) //Zamanlı çıkış 4
    {
      if(t.hour<12)
      {
        if(t.hour>saatdkler[14].toInt()||(t.hour>=saatdkler[14].toInt()&&t.min>=saatdkler[15].toInt()))
        {
          if(sakat[14]==1){digitalWrite(26,LOW);  LED_state[4]=0; sakat[14]=0; sakat[12]=1;} 
        }  
        else
        {
          if(sakat[12]==1){digitalWrite(26,HIGH); LED_state[4]=1; sakat[12]=0; sakat[14]=1;} 
        }
      }
      else
      {
        if(t.hour<saatdkler[12].toInt()||(t.hour<=saatdkler[12].toInt()&&t.min<=saatdkler[13].toInt()))
        {
          if(sakat[14]==1){digitalWrite(26,LOW);  LED_state[4]=0; sakat[14]=0; sakat[12]=1;} 
        }  
        else
        {
          if(sakat[12]==1){digitalWrite(26,HIGH); LED_state[4]=1; sakat[12]=0; sakat[14]=1;} 
        }
      }  
    }
    else
    {
      if(t.hour>saatdkler[12].toInt()||(t.hour>=saatdkler[12].toInt()&&t.min>=saatdkler[13].toInt()))
      {    
        if(t.hour>saatdkler[14].toInt()||(t.hour>=saatdkler[14].toInt()&&t.min>=saatdkler[15].toInt()))
        { 
          if(sakat[14]==1){digitalWrite(26,LOW);  LED_state[4]=0; sakat[14]=0; sakat[12]=1;} 
        }
        else
        { 
          if(sakat[12]==1){digitalWrite(26,HIGH); LED_state[4]=1; sakat[12]=0; sakat[14]=1;}   
        }
      }  
    }
  }
}