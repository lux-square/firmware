#include "./LuxServer.h"
#include "./secrets.h"

//Global function that handle stream data
void streamCallback(StreamData data)
{

  //Print out all information

  Serial.println("Stream Data...");
  Serial.println(data.streamPath());
  Serial.println(data.dataPath());
  Serial.println(data.dataType());

  //Print out value
  //Stream data can be many types which can be determined from function dataType

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
    Serial.println(data.jsonData());
}

//Global function that notify when stream connection lost
//The library will resume the stream connection automatically
void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    //Stream timeout occurred
    Serial.println("Stream timeout, resume streaming...");
  }
}

LuxServer::LuxServer()
{
  // server = new WiFiServer(80, 4);
  firebaseData = new FirebaseData();
  ssid = "Chateau RoRo";
  password = "rosquared";
}

void LuxServer::setup()
{
  /////////////WIFI SETUP////////////////
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to the Wifi network");
  Serial.println(WiFi.localIP());
  // TODO: send localIP to firebase for connection
  Serial.println();

  Firebase.begin(firebase_db, firebase_db_secret);
  Firebase.reconnectWiFi(true);
  Firebase.setStreamCallback(*firebaseData, streamCallback, streamTimeoutCallback);
  if (!Firebase.beginStream(*firebaseData, "/test/data"))
  {
    //Could not begin stream connection, then print out the error detail
    Serial.println(firebaseData->errorReason());
  }
  // server->begin();
  delay(10);
}

void LuxServer::loop()
{
  if (!Firebase.readStream(*firebaseData))
  {
    Serial.println(firebaseData->errorReason());
  }

  if (firebaseData->streamTimeout())
  {
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }

  if (firebaseData->streamAvailable())
  {

    if (firebaseData->dataType() == "int")
      Serial.println(firebaseData->intData());
    else if (firebaseData->dataType() == "float")
      Serial.println(firebaseData->floatData(), 5);
    else if (firebaseData->dataType() == "double")
      printf("%.9lf\n", firebaseData->doubleData());
    else if (firebaseData->dataType() == "boolean")
      Serial.println(firebaseData->boolData() == 1 ? "true" : "false");
    else if (firebaseData->dataType() == "string")
      Serial.println(firebaseData->stringData());
    else if (firebaseData->dataType() == "json")
      Serial.println(firebaseData->jsonData());
  }
}

// void LuxServer::loop()
// {
//   WiFiClient client = server->available(); // Listen for incoming clients

//   if (client)
//   {                                // If a new client connects,
//     Serial.println("New Client."); // print a message out in the serial port
//     String currentLine = "";       // make a String to hold incoming data from the client
//     while (client.connected())
//     {
//       if (client.available())
//       {                         // if there's bytes to read from the client,
//         char c = client.read(); // read a byte, then
//         Serial.write(c);        // print it out the serial monitor
//         header += c;
//         if (c == '\n')
//         { // if the byte is a newline character
//           // if the current line is blank, you got two newline characters in a row.
//           // that's the end of the client HTTP request, so send a response:
//           if (currentLine.length() == 0)
//           {
//             client.print(html_header);

//             // turns the GPIOs on and off
//             if (header.indexOf("GET /26/on") >= 0)
//             {
//               Serial.println("GPIO 26 on");
//               LED1 = "on";
//               digitalWrite(LED1_PIN, HIGH);
//             }
//             else if (header.indexOf("GET /26/off") >= 0)
//             {
//               Serial.println("GPIO 26 off");
//               LED1 = "off";
//               digitalWrite(LED1_PIN, LOW);
//             }
//             else if (header.indexOf("GET /27/on") >= 0)
//             {
//               Serial.println("GPIO 27 on");
//               LED2 = "on";
//               digitalWrite(LED2_PIN, HIGH);
//             }
//             else if (header.indexOf("GET /27/off") >= 0)
//             {
//               Serial.println("GPIO 27 off");
//               LED2 = "off";
//               digitalWrite(LED2_PIN, LOW);
//             }

//             client.print(html_body);
//             // Display current state, and ON/OFF buttons for GPIO 26
//             client.println("<p>GPIO 26 - State " + LED1 + "</p>");

//             // If the LED1 is off, it displays the ON button
//             if (LED1 == "off")
//             {
//               client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
//             }
//             else
//             {
//               client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
//             }

//             // Display current state, and ON/OFF buttons for GPIO 27
//             client.println("<p>GPIO 27 - State " + LED2 + "</p>");
//             // If the LED2 is off, it displays the ON button
//             if (LED2 == "off")
//             {
//               client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
//             }
//             else
//             {
//               client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
//             }
//             client.println("</body></html>");

//             // The HTTP response ends with another blank line
//             client.println();
//             // Break out of the while loop
//             break;
//           }
//           else
//           { // if you got a newline, then clear currentLine
//             currentLine = "";
//           }
//         }
//         else if (c != '\r')
//         {                   // if you got anything else but a carriage return character,
//           currentLine += c; // add it to the end of the currentLine
//         }
//       }
//     }
//     // Clear the header variable
//     header = "";
//     // Close the connection
//     client.stop();
//     Serial.println("Client disconnected.");
//     Serial.println("");
//   }
// }
