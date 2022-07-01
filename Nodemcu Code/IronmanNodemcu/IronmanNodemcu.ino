/*
  This code was created on 25.6.2022
  By: Marwa Abdullah , Ahmed Hassan
  Last Updated : 1.7.2022
  **Notes**:
   1-Please Down grade esp8266 version to 2.3.0 in boardmanager .
   2-ArduinoJson version 5.13.5
   3-Firebase Library from https://github.com/FirebaseExtended/firebase-arduino and do not forget to update database link fingerprint in FirebaseHttpClient.h
   4-Please Download Voice Recognition module from https://github.com/elechouse/VoiceRecognitionV3
*/

#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

/********************Firebase Configurations***************************/
#define FIREBASE_HOST "ironman-ef172-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "YbzQWwQva2mMzlG8asPDgJygbpCfv2QkrNlk7RZR"
/********************WIFI Configurations*******************************/
#define WIFI_SSID "ITI_Students"
#define WIFI_PASSWORD "ITI@2022"
/**************************** VR module *******************************/
/**
  Connection
  nodemcu    VoiceRecognitionModule
   D2   ------->     TX
   D3   ------->     RX
*/
VR myVR(4, 0);   // 2:RX 3:TX, you can choose your favourite pins.

uint8_t records[7]; // save record of VR module
uint8_t buf[64];

#define onRecord1    (14)
#define onRecord2    (15)
#define onRecord3    (16)
#define offRecord1   (17)
#define offRecord2   (18)
#define offRecord3   (19)

/******************** Boatloader MACROS ******************************/
#define SESSION_CONTROL         0x10U
#define DOWN_REQ                0x34U
#define DATA_REQ                0x36U
#define CRC_REQ                 0x31U
#define TRANS_EXIT              0x37U

#define POS_RES                 0x10U
#define NEG_RES                 0x7FU
/********************Global Variables Declaration*********************/
String Command;
int Res = 0; // Response for UART communication
int DataCounter = 0;
int CurrDataBuffSize = 0;
int ret;
byte DataDone = 0;
byte tot_chunks = 0;
/************************ Applicationn Buffers ************************/
byte AppInfo[12];
byte AppData[1024];
byte StringToInt[3];
byte CRC[4];
/********************Functions ProtoTypes***************************/
void getAppInfo();
int getAppData();
int Str_to_byte(char* str);
void printSignature(uint8_t *buf, int len);
void printVR(uint8_t *buf);
/****************************Setup*********************************/
void setup() {

  Serial.setTimeout(2000);

  myVR.begin(9600); // serial connection with nodemcu code
  Serial.begin(9600);
  /************************ connect to wifi.*************************/
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  /************************ connect to Firebase.*************************/
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  if (myVR.clear() == 0) {
    Serial.println("Recognizer cleared.");
  } else {
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while (1);
  }
  /*********************** load recorded commands.*************************/
  if (myVR.load((uint8_t)onRecord1) >= 0 && myVR.load((uint8_t)onRecord2) >= 0  && myVR.load((uint8_t)onRecord3)  >= 0 ) {
    Serial.println("onRecord loaded");
  }
    if (myVR.load((uint8_t)offRecord1) >= 0 && myVR.load((uint8_t)offRecord2) >= 0  && myVR.load((uint8_t)offRecord3)  >= 0 ) {
    Serial.println("offRecord loaded");
  }
}
/***************************Main Loop*******************************/
void loop() {

  /********************** start checking for Flashing Sequence ******************/
  if (Firebase.getString("/AppStatus") == "Flash") {
    delay(5000);
    Serial.write(SESSION_CONTROL);
    DataCounter = 0;
    CurrDataBuffSize = 0;
    DataDone = 0;
    Firebase.setString("/AppStatus", "Busy");

  }
  else if (Firebase.getString("/AppStatus") == "Busy") {
    if (Serial.available() > 0)
    {
      Res = Serial.read();
      if (Res == (POS_RES + SESSION_CONTROL))
      {
        getAppInfo();
        Res = 0;
        delay(3000);
        Serial.write(DOWN_REQ);
        delay(3000);
        Serial.write(AppInfo, 12);
      }
      if (Res == (POS_RES + DOWN_REQ))
      {
        Res = (POS_RES + DATA_REQ);
      }
      if (Res == (POS_RES + DATA_REQ))
      {
        Res = 0;
        if (!DataDone) {
          delay(5000);
          Serial.write(DATA_REQ);
          CurrDataBuffSize = getAppData();
          delay(3000);
          Serial.write(AppData, CurrDataBuffSize);
        }
        else {
          delay(2000);
          Serial.write(CRC_REQ);
          delay(2000);
          Serial.write(CRC, 4);
        }
      }
      if (Res == (POS_RES + CRC_REQ))
      {
        delay(2000);
        Firebase.setString("/AppStatus", "Done");
        Serial.write(TRANS_EXIT);
      }
      /******************************Negative Res***********************************/
      if ( Res == NEG_RES) {
        tot_chunks = (byte)Firebase.getInt("/blockSize/BackUp_totChuncks");
        Firebase.setInt("/blockSize/totalChuncks", tot_chunks);
        Firebase.setString("/AppStatus", "Error");
      }
    }
  }
  /****************************** Helmet Voice Control ***********************************/



  else if (Firebase.getString("/AppStatus") == "Done") {


  /****************************** VR Voice Control ***********************************/

    ret = myVR.recognize(buf, 50);
    if (ret > 0) {
      if (buf[1] == onRecord1 || buf[1] == onRecord2 || buf[1] == onRecord3) {
        Firebase.setString("/IronManHelmet/AppOrder", "\"open\"");
        delay(100);
      }
      else if (buf[1] == offRecord1 || buf[1] == offRecord2 || buf[1] == offRecord3) {
        Firebase.setString("/IronManHelmet/AppOrder", "\"close\"");
        delay(100);
      }

    }

  /****************************** Voice Control send command to STM***********************************/

    Command  = Firebase.getString("/IronManHelmet/AppOrder");
    if (Command == "\"open\"") {
      delay(500);
      Serial.write("O");
      Firebase.setString("/IronManHelmet/AppOrder", "idle");
      Firebase.setString("/IronManHelmet/Status", "o");
    }

    else if (Command == "\"close\"") {
      delay(500);
      Serial.write("C");
      Firebase.setString("/IronManHelmet/AppOrder", "idle");
      Firebase.setString("/IronManHelmet/Status", "c");
    }
    else
    {
      Command  = Firebase.getString("/IronManHelmet/AppOrder");
    }
  }

}
int Str_to_byte(char* str)
{
  int res = 0;
  for (int i = 0; str[i] != '\0'; ++i) {
    if (str[i] == 0) {
      continue;
    }
    else {
      res = res * 10 + str[i] - '0';
    }
  }
  return res;
}
int getAppData() {
  int blockSize = 0 ;
  byte counter = 0;
  int appCount = 0;
  String  dataRec ;
  int str_len;
  byte tot_chunks_left = (byte)Firebase.getInt("/blockSize/totalChuncks") ;
  byte chuncks = 4;
  if (tot_chunks_left < 4) {
    chuncks = tot_chunks_left  ;
    Firebase.setInt("/blockSize/totalChuncks", 0) ;
    tot_chunks_left = 0;
  }
  else {
    Firebase.setInt("/blockSize/totalChuncks", tot_chunks_left - 4) ;
  }

  for (int k = 0; k < chuncks; k++) {
    dataRec =  Firebase.getString("/dataCh/data/" + String(DataCounter));
    str_len = dataRec.length() + 1;
    char char_array[str_len];
    dataRec.toCharArray(char_array, str_len);
    blockSize = Firebase.getInt("/blockSize/sizes/" + String(DataCounter));
    for (int j = 0; j < 3; j++) {
      StringToInt[j] = 0;
    }
    for (int i = 0 ; i <= blockSize; i++) {
      if (char_array[i] != '-') {
        StringToInt[counter] = char_array[i];
        counter++;
      }
      else {
        AppData[appCount] = Str_to_byte((char*)StringToInt);
        //        Serial.println(AppData[appCount]);
        appCount++;
        counter = 0;
        for (int j = 0; j < 3; j++) {
          StringToInt[j] = 0;
        }

      }
    }
    DataCounter++;
  }
  if (tot_chunks_left == 0)
    DataDone = 1;

  return appCount;
}
void getAppInfo() {
  String dataRec =  Firebase.getString("/AppInfo/0");
  byte appInfoCount = 0;
  byte counter = 0 ;
  byte  str_len = dataRec.length() + 1;
  char char_array[str_len];
  byte CRC_Count = 0;
  dataRec.toCharArray(char_array, str_len);
  for (int j = 0; j < 3; j++) {
    StringToInt[j] = 0;
  }
  for (int i = 0 ; i <= str_len; i++) {
    if (char_array[i] != '-') {
      StringToInt[counter] = char_array[i];
      counter++;
    }
    else {
      if (appInfoCount < 12) {
        AppInfo[appInfoCount] = Str_to_byte((char*)StringToInt);
      }
      else {

        CRC[CRC_Count] = Str_to_byte((char*)StringToInt);
        CRC_Count++;

      }
      appInfoCount++;
      counter = 0;
      for (int j = 0; j < 3; j++) {
        StringToInt[j] = 0;
      }
    }
  }



}
