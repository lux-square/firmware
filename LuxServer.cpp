#include "./LuxServer.h"

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
  dbUpdate = true;
  dbMessage = "...";
}

void LuxServer::setup(QueueHandle_t queueHandle)
{
  queue = queueHandle;
  WiFi.begin(ssid.c_str(), password.c_str());

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
  if (!Firebase.beginStream(*firebaseData, "/test/data"))
  {
    Serial.print("Firebase error: ");
    Serial.println(firebaseData->errorReason());
  }
  // TODO fetch message from firebase initially
  delay(10);
}

void LuxServer::loop()
{
  if (!Firebase.readStream(*firebaseData))
  {
    Serial.print("Firebase error: ");
    Serial.println(firebaseData->errorReason());
  }

  if (firebaseData->streamTimeout())
  {
    Serial.println("Stream timeout, resume streaming...");
    delay(10);
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
      dbMessage = firebaseData->boolData() == 1 ? "true" : "false";
    else if (firebaseData->dataType() == "string")
      dbMessage = firebaseData->stringData();
    else if (firebaseData->dataType() == "json")
      Serial.println(firebaseData->jsonData());
    dbUpdate = true;
  }
  produceQueue();
}

void LuxServer::produceQueue()
{
  uint16_t emptySpaces = uxQueueSpacesAvailable(queue);
  if (dbUpdate)
  {
    if (emptySpaces > dbMessage.length())
    {
      for (uint16_t i = 0; i < dbMessage.length(); i++)
      {
        xQueueSend(queue, static_cast<void *>(&dbMessage), 0);
      }
      dbUpdate = false;
      delay(100);
    }
    else
    {
      Serial.println("Too little space to send message in queue");
    }
  }
}
