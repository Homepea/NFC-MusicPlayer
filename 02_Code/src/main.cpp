#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"
#include "Ws2812B.h"

#define SS_PIN 7
#define RST_PIN 3

MFRC522 mfrc522(SS_PIN, RST_PIN);

#define FPSerial Serial1
u16_t uwSongNum = 0, uwSongBak = 0; // 歌曲编号

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
int tmp = 0;
void setup()
{
    USBSerial.begin();
    SPI.begin(6, 5, 4, 3);
    mfrc522.PCD_Init();
    USBSerial.printf("Ready to read NTAG213...");

    FPSerial.begin(9600, SERIAL_8N1, /*rx =*/8, /*tx =*/9);

    while (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true))
    { // Use serial to communicate with mp3.
        USBSerial.printf("Mp3 unable to begin:");
        USBSerial.printf("1.Please recheck the connection!");
        USBSerial.printf("2.Please insert the SD card!\r\n");
    }
    USBSerial.printf("DFPlayer Mini online.\r\n");

    myDFPlayer.volume(20); // Set volume value. From 0 to 30
    // myDFPlayer.playMp3Folder (1);    // Play the first mp3

    xTaskCreatePinnedToCore(
        Task_WS2812B, // 任务函数
        "LED Task",   // 任务名称
        4096,         // 堆栈大小
        NULL,         // 参数
        1,            // 优先级
        NULL,         // 任务句柄
        1             // 核心编号
    );
}

void loop()
{
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        delay(10);
        return;
    }

    // 显示UID
    USBSerial.printf("UID : ");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        USBSerial.printf("0x2%x ", mfrc522.uid.uidByte[i]); // mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    }
    USBSerial.println();

    // NTAG213专用读取流程（关键修复部分）
    byte pagesToRead = 24; // 从页4开始读取
    String tagData = "", numdat = "";
    bool success = true;

    for (byte page = 4; page <= pagesToRead; page++)
    { // 注意这里改为<=
        byte buffer[18];
        byte size = sizeof(buffer);

        // 使用正确的Ultralight读取方法
        MFRC522::StatusCode status = mfrc522.MIFARE_Read(page, buffer, &size);

        if (status != MFRC522::STATUS_OK)
        {
            // USBSerial.printf("Page 0x%2x read failed\r\n", page);
            success = false;
            break;
        }

        // 安全拷贝数据（每个页4字节）
        for (byte i = 0; i < 4; i++)
        {
            if (buffer[i] != 0)
            {
                tagData += (char)buffer[i];
            }
        }
    }

    // 安全解析数据
    if (success && tagData.length() > 0)
    {
        for (uint8_t i = 14; i < tagData.length() - 1; i++)
        {
            numdat += tagData[i];
        }
        uwSongNum = atoi(numdat.c_str());
        USBSerial.printf("Song num: %d\r\n", uwSongNum);
    }
    if (uwSongNum)
    {
        myDFPlayer.playMp3Folder (uwSongNum); // 播放歌曲
    }
    else
    {
        myDFPlayer.stop(); // 停止播放
    }
    mfrc522.PICC_HaltA();
}