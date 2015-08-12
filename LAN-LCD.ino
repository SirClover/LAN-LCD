#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>

byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x09, 0x2D}; //MAC
byte ip[] = {192, 168, 1, 111}; //IP
EthernetServer servidor(80);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
String readString, mensaje;
char mensajeEscrito[32];
int indStart=0, indStop=0;

void setup()
{
  Ethernet.begin(mac, ip);
  servidor.begin();
  lcd.begin(16,2);
  lcd.clear();
  Serial.begin(9600);

  escribirLCD("ESPERANDO ...");
}

void loop()
{
  comprobarWeb();
}

void comprobarWeb()
{
  EthernetClient cliente = servidor.available();
  if (cliente)
  {
    while (cliente.connected())
    {
      if (cliente.available())
      {
        char c = cliente.read();
        if (readString.length() < 100)
        {
          readString += c;
        }

        if (c == '\n')
        {
          Serial.println(readString);

          cliente.println("HTTP/1.1 200 OK");
          cliente.println("Content-Type: text/html");
          cliente.println();
          cliente.println("<HTML>");
          cliente.println("<HEAD>");
          cliente.println("<TITLE>ONLINE LCD</TITLE>");
          cliente.println("</HEAD>");
          cliente.println("<BODY>");
          cliente.println("<H1>INTRODUZCA EL MENSAJE</H1>");
          cliente.println("<FORM ACTION='/' method=get>");
          cliente.println("<INPUT TYPE=TEXT NAME='MENSAJE' VALUE='' SIZE='25' MAXLENGTH='32'><BR>");
          cliente.println("<INPUT TYPE=SUBMIT NAME='SUBMIT' VALUE='ENVIAR'>");
          cliente.println("</FORM><BR>");
          cliente.println("</BODY>");
          cliente.println("</HTML>");

          delay(100);
          cliente.stop();

          Serial.println(readString);
          indStart = readString.indexOf("MENSAJE=");
          indStart += 8;
          indStop = readString.indexOf("&");
          Serial.println(readString.substring(indStart, indStop));
          mensaje = readString.substring(indStart, indStop);
          mensaje.replace("+", " ");
          Serial.println(mensaje);
          escribirLCD(mensaje);
          readString = "";
          mensaje = "";          
        }
      }
    }
  }
}

void escribirLCD(String mensajeLCD)
{
  if (mensajeLCD.indexOf("GET ") != 0 && mensajeLCD.indexOf("vicon.ico") != 0 && mensajeLCD.indexOf("TTP/") != 0)
  {
    lcd.clear();
    if(mensajeLCD.length()>15)
    {
      lcd.setCursor(0,0);
      lcd.print(mensajeLCD.substring(0,16));
      lcd.setCursor(0,1);
      lcd.print(mensajeLCD.substring(16,mensajeLCD.length()));
    }
    else
    {
      lcd.print(mensajeLCD);
    }
  }
}
