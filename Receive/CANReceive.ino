/*
 * 受信待機
*/
#include <mcp_can.h>
#include <SPI.h>

// 
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

// シリアルバッファ
char msgString[128];

// ピン定義
#define CAN0_INT 2
#define CAN0_CS 10
MCP_CAN CAN0(CAN0_CS);

// 初期化済み?
int isInitialized = -1; //-1: None 0: Complete 1: Failed

void setup(){
    Serial.begin(115200);
    Serial.println("MCP2515 Library Receive Example...");
    pinMode(CAN0_INT, INPUT);
    pinMode(3, INPUT_PULLUP);

    // 初期化
    if(isInitialized < 0){
        Serial.print("Initialize...");
        isInitialized = initialize();
        Serial.println(isInitialized);
    }
    CAN0.setMode(MCP_NORMAL);
}

void loop(){
    if(!digitalRead(CAN0_INT) && !isInitialized){
        CAN0.readMsgBuf(&rxId, &len, rxBuf);
        
        if((rxId & 0x80000000) == 0x80000000){
            sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
        }else{
            sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
        }
        Serial.print(msgString);

        if((rxId & 0x40000000) == 0x40000000){
            sprintf(msgString, " REMOTE REQUEST FRAME");
            Serial.print(msgString);
        }else{
            for(byte i = 0; i<len; i++){
                sprintf(msgString, " 0x%.2X", rxBuf[i]);
                Serial.print(msgString);
            }
        }

        Serial.println();
    }
}

// MCPモジュール初期化(0: success)
int initialize(){
    return !(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK);
}