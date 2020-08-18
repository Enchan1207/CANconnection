/*
 *  定期送信
*/
#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN0(10);     // Set CS to pin 10

byte data[8] = {0xAA, 0x55, 0x01, 0x10, 0xFF, 0x12, 0x34, 0x56};

// 初期化済み?
int isInitialized = -1; //-1: None 0: Complete 1: Failed

void setup(){
    Serial.begin(115200);
    Serial.println("MCP2515 Library Send Example...");
    
    // 初期化
    if(isInitialized < 0){
        Serial.print("Initialize...");
        isInitialized = initialize();
        Serial.println(isInitialized);
    }
    CAN0.setMode(MCP_NORMAL);
}

void loop(){
    // データを送りつける
    byte sndStat = CAN0.sendMsgBuf(0x100, 0, 8, data);
    Serial.println(sndStat);
    if(sndStat == CAN_OK){
        Serial.println("Message Sent Successfully!");
        blink(5);
    } else {
        Serial.println("Error Sending Message...");
    }
    delay(1000);
}

// LED点滅
void blink(int pin){
    for(int i = 0; i < 4; i++){
        digitalWrite(pin, (i % 2) == 0);
        delay(70);
    }
}

// MCPモジュール初期化(0: success)
int initialize(){
    return !(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK);
}