/*
 * CANデバイス
 * 1. 初期化+ループバックモード 
*/

#include <mcp_can.h>
#include <SPI.h>

// 送信インターバル
unsigned long prevTX = 0;
const unsigned int invlTX = 1000;

// データ
byte data[] = {0xAA, 0x55, 0x01, 0x10, 0xFF, 0x12, 0x34, 0x56};

// 受信データ周り
long unsigned int rxId;
unsigned char len;
unsigned char rxBuf[8];

// シリアル出力バッファ
char msgString[128];

// ピン定義
#define CAN0_INT 2
MCP_CAN CAN0(10);

// 初期化済み?
int isInitialized = -1; //-1: None 0: Complete 1: Failed

// MCPモジュール初期化(0: success)
int initialize(){
    return !(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK);
}

//
void setup(){
    Serial.begin(115200);
    Serial.println("MCP2515 Library Loopback Example...");
    pinMode(CAN0_INT, INPUT);
    pinMode(3, INPUT_PULLUP);
}

void loop(){
    // D3押下時初期化
    if(!digitalRead(3) && isInitialized != 0){
        Serial.println("Initialize...");
        isInitialized = initialize();
    }

    if (isInitialized == 0){
        // データ受信時
        if (!digitalRead(CAN0_INT)){
            CAN0.readMsgBuf(&rxId, &len, rxBuf); // データ受信

            // IDで処理を分ける
            if ((rxId & 0x80000000) == 0x80000000){
                sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
            }else{
                sprintf(msgString, "Standard ID: 0x%.3lX  DLC: %1d  Data:", rxId, len);
            }
            Serial.print(msgString);

            if ((rxId & 0x40000000) == 0x40000000){
                sprintf(msgString, " REMOTE REQUEST FRAME");
                Serial.print(msgString);
            }else{
                for (byte i = 0; i < len; i++){
                    sprintf(msgString, " 0x%.2X", rxBuf[i]);
                    Serial.print(msgString);
                }
            }

            Serial.println();
        }

        // 定期的にデータを送りつける
        if (millis() - prevTX >= invlTX){
            prevTX = millis();
            byte sndStat = CAN0.sendMsgBuf(0x100, 8, data);

            if (sndStat == CAN_OK){
                Serial.println("Message Sent Successfully!");
            }else{
                Serial.println("Error Sending Message...");
            }
        }
    }
}
