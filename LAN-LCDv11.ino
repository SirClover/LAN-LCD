#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>

#define CHARACTERS 32
#define ROWS 10

////////////////////////////////////////////////////////////////////////////////
boolean test = false;
////////////////////////////////////////////////////////////////////////////////

byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x09, 0x2D}; //MAC
byte ip[] = {192, 168, 1, 111}; //IP
EthernetServer servidor(80);
EthernetClient cliente;

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
String readString, mensaje, mensajeWeb;
char mensajeEscrito[ROWS][CHARACTERS];
int indStart=0, indStop=0, boton = 3;



void setup()
{
  Ethernet.begin(mac, ip);
  servidor.begin();
  lcd.begin(16,2);
  lcd.clear();
  Serial.begin(9600);

  clearList();

  escribirLCD("ESPERANDO ...");
  
  pinMode(boton, INPUT);
}

void loop()
{
  comprobarWeb();
  comprobarBoton();
}

void comprobarWeb()
{
  cliente = servidor.available();
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
          printWeb();

          Serial.println(readString);
          indStart = readString.indexOf("MENSAJE=");
          indStart += 8;
          indStop = readString.indexOf("&");
          Serial.println(readString.substring(indStart, indStop));
          mensaje = readString.substring(indStart, indStop);
          mensaje.replace("+", " ");
          Serial.println(mensaje);
          if (test == true)
          {
            if (checkList() != -1)
            {
              if (checkList() == 0)
              {
                addRow(checkList());
                extractRow(0);
                escribirLCD(mensajeWeb);
              }
              else
              {
                addRow(checkList());
              }
              printWeb();
            }
          }
          else
          {
            escribirLCD(mensaje);
          }
          readString = "";
          mensaje = "";          
        }
      }
    }
  }
}

void comprobarBoton ()
{
	if (digitalRead(boton) == HIGH)
	{
		escribirLCD("HOLA");
		delay(50);
	}
}

void escribirLCD(String mensajeLCD)
{
	Serial.println(mensajeLCD);
	lcd.clear();
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

void extractRow (int index)
{
  for (int i = 0; i < CHARACTERS; i++)
  {
    mensajeWeb += mensajeEscrito[index][i];
  }
}

void addRow (int index)
{
  char charArray[32];

  mensaje.toCharArray(charArray, 32);
  for (int i = 0; i < CHARACTERS; i++)
  {
    mensajeEscrito[index][i] = charArray[i];
  }
}

void clearList ()
{
  for (int r = 0; r < ROWS; r++)
  {
    for (int c = 0; c < CHARACTERS; c++)
    {
      mensajeEscrito[r][c] = '.';      
    }
  }
  escribirLCD("Esperando ...");
}

int checkList ()
{
  int index = 0;
  boolean findHole = false;

  for (int r = 0; r < ROWS; r++)
  {
    if (mensajeEscrito[r][0] == '.')
    {
      findHole = true;
      index = r;
    }
  }

  if (findHole == true)
  {
    return index;
  }
  else
  {
    return -1;
  }
}

void printWeb()
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
  if (test == true)
  {
    for (int i = 0; i < ROWS; i++)
    {
      cliente.print("<UL>");
      cliente.print(i);
      cliente.print(". ");
      extractRow(0);
      cliente.print(mensajeWeb);
      cliente.print("</UL>");              
    }
  }          
  cliente.println("</BODY>");
  cliente.println("</HTML>");

  delay(1);
  cliente.stop();
}
