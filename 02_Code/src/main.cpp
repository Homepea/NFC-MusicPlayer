#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"
#include "Ws2812B.h"
#include <Preferences.h>

Preferences preferences;

#define SS_PIN 7
#define RST_PIN 3

MFRC522 mfrc522(SS_PIN, RST_PIN);

#define FPSerial Serial1
u16_t uwSongNum = 0, uwSongBak = 0; // 歌曲编号

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
int tmp = 0, gboVolUpFlag = 0, gboVolDownFlag = 0;

unsigned long startTime;
bool functionExecuted = false;

int gubVol = 20, VOL_CHANGE_INT = 4; // 音量

void Task_NFC(void *User_Task_NFC)
{
    while (1)
    {
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
        {
            delay(10);
        }
        else
        {
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
                tmp = atoi(numdat.c_str());
            }
            if ('+' == tagData[14]) // 音量增加
            {
                if (gubVol + VOL_CHANGE_INT < 30)
                {
                    gubVol += VOL_CHANGE_INT;
                    myDFPlayer.volume(gubVol);
                    USBSerial.printf("Vol: %d\r\n", gubVol);
                    preferences.putInt("volume", gubVol);
                    gboVolUpFlag = 1;
                    gboVolDownFlag = 0;
                }
                else
                {
                    gubVol = 30;
                    myDFPlayer.volume(gubVol);
                    USBSerial.printf("Vol: %d\r\n", gubVol);
                    preferences.putInt("volume", gubVol);
                    gboVolDownFlag = 0;
                    gboVolUpFlag = 2;
                }
            }
            else if ('-' == tagData[14]) // 音量减小
            {
                if (gubVol > VOL_CHANGE_INT)
                {
                    gubVol -= VOL_CHANGE_INT;
                    gboVolUpFlag = 0;
                    gboVolDownFlag = 1;
                    myDFPlayer.volume(gubVol);
                    USBSerial.printf("Vol: %d\r\n", gubVol);
                    preferences.putInt("volume", gubVol);
                }
                else
                {
                    gubVol = 2;
                    myDFPlayer.volume(gubVol);
                    USBSerial.printf("Vol: %d\r\n", gubVol);
                    preferences.putInt("volume", gubVol);
                    gboVolUpFlag = 0;
                    gboVolDownFlag = 2;
                }
            }
            else
            {
                uwSongNum = tmp;
                if (uwSongNum)
                {
                    uwSongNum = tmp;
                    myDFPlayer.playMp3Folder(uwSongNum); // 播放歌曲
                    USBSerial.printf("Song num: %d\r\n", uwSongNum);
                }
                else
                {
                    myDFPlayer.stop(); // 停止播放
                    USBSerial.printf("Song stop!\r\n");
                }
            }
            mfrc522.PICC_HaltA();
        }
        delay(10);
    }
}

void setup()
{
    startTime = millis();
    delay(100);
    pinMode(0, OUTPUT);

    USBSerial.begin();
    USBSerial.printf("System init...\r\n");
    preferences.begin("my-app", false); // 打开或创建命名空间 "my-app"

    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    digitalWrite(2, HIGH);  // 打开LED
    digitalWrite(3, HIGH);  // 打开LED
    digitalWrite(4, HIGH);  // 打开LED
    digitalWrite(5, HIGH);  // 打开LED
    digitalWrite(6, HIGH);  // 打开LED
    digitalWrite(7, HIGH);  // 打开LED
    digitalWrite(8, HIGH);  // 打开LED
    digitalWrite(9, HIGH);  // 打开LED
    digitalWrite(10, HIGH); // 打开LED
    delay(5);
    digitalWrite(8, LOW); // 打开LED
    digitalWrite(9, LOW); // 打开LED
    delay(5);
    digitalWrite(8, HIGH); // 打开LED
    digitalWrite(9, HIGH); // 打开LED
    delay(50);

    SPI.begin(6, 5, 4, 3);
    mfrc522.PCD_Init();
    USBSerial.printf("Ready to read NTAG213...");

    FPSerial.begin(9600, SERIAL_8N1, /*rx =*/8, /*tx =*/9);
    delay(50);

    while (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true))
    { // Use serial to communicate with mp3.
        USBSerial.printf("Mp3 unable to begin:");
        USBSerial.printf("1.Please recheck the connection!");
        USBSerial.printf("2.Please insert the SD card!\r\n");
    }
    USBSerial.printf("DFPlayer Mini online.\r\n");

    delay(100);

    gubVol = preferences.getInt("volume", 20);
    gubVol = gubVol > 30 ? 30 : gubVol;
    myDFPlayer.volume(gubVol);
    USBSerial.printf("Vol: %d\r\n", gubVol);

    // myDFPlayer.volume(20); // Set volume value. From 0 to 30
    // myDFPlayer.playMp3Folder(1);    // Play the first mp3

    xTaskCreatePinnedToCore(
        Task_WS2812B, // 任务函数
        "LED Task",   // 任务名称
        4096,         // 堆栈大小
        NULL,         // 参数
        1,            // 优先级
        NULL,         // 任务句柄
        0             // 核心编号
    );

    xTaskCreatePinnedToCore(
        Task_NFC,  // 任务函数
        "LED NFC", // 任务名称
        4096,      // 堆栈大小
        NULL,      // 参数
        2,         // 优先级
        NULL,      // 任务句柄
        0          // 核心编号
    );
}

void loop()
{
    // printf("NFC music player!\r\n");
    digitalWrite(0, LOW); // 关闭LED
    delay(1000);
    digitalWrite(0, HIGH); // 打开LED
    delay(1000);
}
