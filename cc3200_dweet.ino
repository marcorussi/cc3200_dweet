/*
* The MIT License (MIT)
*
* Copyright (c) 2015 Marco Russi
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

/* include WiFi library */
#include <WiFi.h>

/* delay between updates, in milliseconds */
#define REQUEST_INTERVAL    (2000L)

/* HTTP port number */
#define HTTP_PORT            80


/* CHANGE FOLLOWING VALUES TO YOUR NETWORK SETTINGS! */
/* network name also called SSID */
char ssid[] = "ssid";
/* network password */
char password[] = "password";

/* Wifi client library */
WiFiClient client;

/* server address */
char server[] = "dweet.io";

/* last time a get request is sent to the server, in milliseconds */
unsigned long lastConnectionTime = 0;        

/* string to store received data */
String received_data_string;


/* setup function */
void setup() {
  
  /* init LEDs pins */
  pinMode(RED_LED, OUTPUT); 
  pinMode(YELLOW_LED, OUTPUT); 
  pinMode(GREEN_LED, OUTPUT); 
  digitalWrite(RED_LED, LOW);   
  digitalWrite(YELLOW_LED, LOW);   
  digitalWrite(GREEN_LED, LOW);   
  
  /* init serial and wait for port to open */
  Serial.begin(115200);

  /* attempt to connect to Wifi network */
  Serial.print("Attempting to connect to Network named: ");
  /* print the network name (SSID) */
  Serial.println(ssid); 
  /* connect to WPA/WPA2 network. Change this line if using open or WEP network */
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    /* print dots while we wait to connect */
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  /* wait for an IP address */
  while (WiFi.localIP() == INADDR_NONE) {
    /* print dots while we wait for an ip address */
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  /* connected and IP address obtained. Print the WiFi status */
  printWifiStatus();
  
  /* request a permanent connection to the server */
  connection_request();
}


void loop() {
  /* read any received data (no data at the first run obviously) */
  /* and call related function to check data if necessary */
  if (true == read_received_data()) {
    /* check received data */
    check_received_data();
  }
  else
  {
    /* no data. Do nothing */
  }

  /* if REQUEST_INTERVAL milliseconds have passed since your last connection,
     then connect again and send data */
  if (millis() - lastConnectionTime > REQUEST_INTERVAL) {
    /* send a get request */
    get_request();
  }
  else {
    /* do nothing and wait */
  }
}


/* function to connect to the server */
void connection_request() {
  /* close any connection before send a new request.
     This will free the socket on the WiFi shield */
  client.stop();

  Serial.println("connecting...");
  /* if there's a successful connection */
  if (client.connect(server, HTTP_PORT)) {
    /* connection request succeed */
    Serial.println("connected!");
    /* turn green LED ON */
    digitalWrite(GREEN_LED, HIGH);
  }
  else {
    /* connection request failed */
    Serial.println("connection failed");
    /* turn red LED ON */
    digitalWrite(RED_LED, HIGH);
  }
}


/* function to get a dweet status from the server */
void get_request() {
  /* send get request */
  client.println("GET /get/latest/dweet/for/{test_cc3200} HTTP/1.1");
  client.println("Host: dweet.io");
  client.println("Connection: Keep-Alive");
  client.println();

  /* note the time that the connection was made */
  lastConnectionTime = millis();
}


/* function to read eventual received data */
boolean read_received_data() {
  boolean valid_data = false;
  /* if there is incoming data from the connection
     then read it */
  if( client.available() > 0 ) {
    Serial.println("Received data:");
    /* copy each character into the data string */
    do {
      char c = client.read();
      received_data_string += c;
    } while (client.available());
    /* print data for debugging */
    Serial.println(received_data_string);
    /* print 2 blank lines */
    Serial.println("\n\n");
    /* valid data received */
    valid_data = true;
  }
  else {
    /* do nothing */
  }
  
  return valid_data;
}


/* function to check received data */
void check_received_data()
{
  int string_index;
  int data_length;
  
  /* get data length */
  data_length = received_data_string.length();
  /* check if the message starts as expected */
  if (received_data_string.startsWith("HTTP/1.1 200 OK")) {
    
    /* find "this:" field */
    string_index = received_data_string.indexOf("this");
    /* if the previous field is whithn the data string */
    if (string_index < data_length) {
      /* check if get request has succeeded */
      if (received_data_string.startsWith("succeeded", string_index + 7)) {
        
        /* find "thing" field */
        string_index = received_data_string.indexOf("thing");
        /* if the previous field is whithn the data string */
        if (string_index < data_length) {
          /* check if the test led is ON */
          if (received_data_string.startsWith("test_cc3200", string_index + 9)) {
        
            /* find "led_test" field */
            string_index = received_data_string.indexOf("led_test");
            /* if the previous field is whithn the data string */
            if (string_index < data_length) {
              /* check if the test led is ON */
              if (received_data_string.startsWith("on", string_index + 11)) {
                /* turn the test led on */
                digitalWrite(YELLOW_LED, HIGH);
              }
              else if (received_data_string.startsWith("off", string_index + 11)) {
                /* turn the test led off */
                digitalWrite(YELLOW_LED, LOW);
              }
              else {
                /* invalid "led_test" field value. Do nothing */
              }
            } 
            else {
              /* unable lo find the "led_test" field. Do nothing */
            }
            
          }
          else {
            /* the "thing" field is not as expected. Do nothing */
          }
        }
        else {
          /* unable lo find the "thing" field. Do nothing */
        }
        
      }
      else {
        /* request has failed or an invalid field has been received. Do nothing */
      }
    } 
    else {
      /* unable lo find the "this:" field. Do nothing */
    }
    
  }
  else {
    /* invalid response. Do nothing */
  }
  
  /* clean received data string */
  received_data_string.remove(0);
}


/* function to print WiFi status */
void printWifiStatus() {
  /* print the SSID of the network you're attached to */
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  /* print your WiFi IP address */
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  /* print the received signal strength */
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}



