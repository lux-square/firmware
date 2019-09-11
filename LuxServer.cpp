#include "./LuxServer.h"

//Global function that handle stream data
void streamCallback(StreamData data)
{
  Serial.println("STREAM CALLBACK");
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
    Serial.println("Stream Callback");
  Serial.println(data.jsonData());
}

//Global function that notify when stream connection lost
//The library will resume the stream connection automatically
void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    //Stream timeout occurred
  }
}

LuxServer::LuxServer()
{
  // server = new WiFiServer(80, 4);
  firebaseData = new FirebaseData();
  strcpy(ssid, "Chateau RoRo");
  strcpy(password, "rosquared");
  strcpy(jsonData, "{\"text\":\"...\"}");
  dbUpdate = true;
}

void LuxServer::setup(QueueHandle_t queueHandle)
{
  queue = queueHandle;
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print('.');
  }
  Serial.println();
  Serial.println("Connected to the Wifi network with ip: ");
  Serial.print(WiFi.localIP());
  Serial.println();

  Firebase.begin(firebase_db, firebase_db_secret);
  Firebase.setStreamCallback(*firebaseData, streamCallback, streamTimeoutCallback);
  if (!Firebase.beginStream(*firebaseData, "/data"))
  {
    Serial.print("Firebase error: ");
    Serial.println(firebaseData->errorReason());
  }
  //init and get the time
  configTime(0, 0, ntpServer);
  printUTCTime();
  delay(10);
}

void LuxServer::loop()
{
  if (!Firebase.readStream(*firebaseData))
  {
    Serial.print("Firebase error: ");
    Serial.println(firebaseData->errorReason());
  }

  if (firebaseData->streamAvailable())
  {
    Serial.print("stream available of type: ");
    Serial.println(firebaseData->dataType());
    if (firebaseData->dataType() == "json" || firebaseData->dataType() == "string")
    {
      strcpy(jsonData, firebaseData->jsonData().c_str());
    }

    dbUpdate = true;
  }
  produceQueue();
}

void LuxServer::produceQueue()
{
  uint16_t emptySpaces = uxQueueSpacesAvailable(queue);
  if (dbUpdate)
  {
    if (emptySpaces > strlen(jsonData))
    {
      for (uint16_t i = 0; i < strlen(jsonData) + 1; i++)
      {
        xQueueSend(queue, static_cast<void *>(&jsonData[i]), 0);
      }
      Serial.print("Server: ");
      Serial.println(jsonData);
      dbUpdate = false;
      delay(100);
    }
  }
}
