#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>  // Incluimos la librería  SoftwareSerial
SoftwareSerial BT(6, 5);     // Definimos los pines TX y RX del Arduino conectados al Bluetooth
/*Using Hardware SPI of Arduino */
/*MOSI (11), MISO (12) and SCK (13) are fixed */
/*You can configure SS and RST Pins*/
#define SS_PIN 10 /* Slave Select Pin */
#define RST_PIN 7 /* Reset Pin */

/* Create an instance of MFRC522 */
MFRC522 mfrc522(SS_PIN, RST_PIN);
/* Create an instance of MIFARE_Key */
MFRC522::MIFARE_Key key;

/* Set the block to which we want to write data */
/* Be aware of Sector Trailer Blocks */
int blockNum = 2;
/* Create an array of 16 Bytes and fill it with data */
/* This is the actual data which is going to be written into the card */
byte blockData[16] = { "Electronics-Hub-" };

/* Create another array to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];

MFRC522::StatusCode status;

void setup() {
  BT.begin(9600);  // Inicializamos el puerto serie BT (Para Modo AT 2)
  /* Initialize serial communications with the PC */
  Serial.begin(9600);
  /* Initialize SPI bus */
  SPI.begin();
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  Serial.println("Scan a MIFARE 1K Tag to write data...");
}
//EL SCRIPT NO PERMITE ESCRITURA y la LECTURA ES DEL UUID de la tarjeta
void loop() {

  // if (BT.available())  // Si llega un dato por el puerto BT se envía al monitor serial
  // {
  //   Serial.write(BT.read());
  // }

  if (Serial.available())  // Si llega un dato por el monitor serial se envía al puerto BT
  {
    BT.write(Serial.read());
  }
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if (mfrc522.PICC_IsNewCardPresent()) {

    /* Select one of the cards */
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("\n");
      Serial.println("**Card Detected**");
      /* Print UID of the Card */
      Serial.print(F("Card UID:"));
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      }
      String uidString = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        uidString += String(mfrc522.uid.uidByte[i], HEX);
      }
      for (byte i = 0; i < uidString.length(); i++) {
        BT.write(uidString[i]);
      }
      BT.write('\n');
      //delay(1000);
      // Serial.print("\n");
      /* Print type of card (for example, MIFARE 1K) */
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      // Serial.println(mfrc522.PICC_GetTypeName(piccType));
      /* Read data from the same block */
      // Serial.print("\n");
      // Serial.println("Reading from Data Block...");
      //mfrc522.PICC_HaltA();
      ReadDataFromBlock(blockNum, readBlockData);
      /* If you want to print the full memory dump, uncomment the next line */
      // //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

      /* Print the data read from block */
      Serial.print("\n");
      Serial.print("Data in Block:");
      Serial.print(blockNum);
      Serial.print(" --> ");
      for (int j = 0; j < 16; j++) {
        Serial.write(readBlockData[j]);
        //BT.write(char(readBlockData[j]));
      }
      Serial.print("\n");
    }else{
      Serial.print("no read");
    }
  }
  //  /* Call 'WriteDataToBlock' function, which will write data to the block */
  //  Serial.print("\n");
  //  Serial.println("Writing to Data Block...");
  //  WriteDataToBlock(blockNum, blockData);
}

// void WriteDataToBlock(int blockNum, byte blockData[]) {
//   /* Authenticating the desired data block for write access using Key A */
//   status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
//   if (status != MFRC522::STATUS_OK) {
//     Serial.print("Authentication failed for Write: ");
//     Serial.println(mfrc522.GetStatusCodeName(status));
//     return;
//   } else {
//     Serial.println("Authentication success");
//   }


//   /* Write data to the block */
//   status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
//   if (status != MFRC522::STATUS_OK) {
//     Serial.print("Writing to Block failed: ");
//     Serial.println(mfrc522.GetStatusCodeName(status));
//     return;
//   } else {
//     Serial.println("Data was written into Block successfully");
//   }
// }

void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
  /* Authenticating the desired data block for Read access using Key A */
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Read: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Block was read successfully");
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
