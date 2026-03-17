#pragma once

#include "bootstrap.hpp"
#include "IAction.hpp"
#include "app/effects/LED/NeoPixel/ILedEffect.hpp"

#include "app/service/LED/NeoPixelService.hpp"
#include "app/structs/LedPayloadBle.hpp"

#include "app/effects/LED/NeoPixel/TurnOnEffect.hpp"
#include "app/effects/LED/NeoPixel/TurnOffEffect.hpp"
#include "app/effects/LED/NeoPixel/SosEffects.hpp"


/*
    Protocol for LED control BLE:
    - The BLE characteristic will receive a payload of 5 bytes, structured as follows:
    Byte 0: Red intensity (0-255)
    Byte 1: Green intensity (0-255)
    Byte 2: Blue intensity (0-255)
    Byte 3: Brightness (0-255)
    Byte 4: Effects:
        0: Turn off LEDs
        1: TurnOnEffect (set all LEDs to the specified color)
        2: SosEffect (blink LEDs in SOS pattern)
*/

#define TOTAL_LED_EFFECTS 3 // Update this value based on the actual number of effects implemented

class ReadQueueLedBleAction : public IAction
{
    public:
        TaskService taskService;   
        NeoPixelService* ledService;
        LedPayloadBle payload;

        /* Effect instances */ 
        TurnOnEffect turnOnEffect;
        TurnOffEffect turnOffEffect;
        SosEffects sosEffect;

        ILedEffect* effectSelected = nullptr; 

        uint8_t lastR = 0, lastG = 0, lastB = 0, lastBrightness = 0; 
        uint8_t currentR = 0, currentG = 0, currentB = 0, currentBrightness = 0;

        ILedEffect* effectsLedMap[TOTAL_LED_EFFECTS]; // Map of effect instances

        ReadQueueLedBleAction(NeoPixelService* ledService): 
            taskService(TaskService()), 
            ledService(ledService), 
            turnOnEffect(ledService), 
            turnOffEffect(ledService),
            sosEffect(ledService) 
        {
            /* Initialize effect map */
            effectsLedMap[0] = &turnOffEffect; 
            effectsLedMap[1] = &turnOnEffect; 
            effectsLedMap[2] = &sosEffect; 
        }
       
        void task(void* pvParameters)
        {
            while (true)
            {
                if (xQueueReceive(QueuesConfig::instance().ledQueue, &payload, 0))
                {
                    Serial.println("Comando recebido da fila LED BLE:");
                    Serial.print("R: ");
                    Serial.print(payload.r);
                    Serial.print(" G: ");
                    Serial.print(payload.g);
                    Serial.print(" B: ");
                    Serial.print(payload.b);
                    Serial.print(" Brilho: ");
                    Serial.print(payload.brigthness);
                    Serial.print(" Efeito: ");
                    Serial.println(payload.effect);

                    currentR = payload.r;
                    currentG = payload.g;
                    currentB = payload.b;
                    currentBrightness = payload.brigthness;

                    if(payload.effect < TOTAL_LED_EFFECTS)
                    {
                        effectSelected = effectsLedMap[payload.effect];
                    }
                        
                }

                setLeds();
                setBrightness();
                setEffect();

                vTaskDelay(pdMS_TO_TICKS(50)); // wait before checking the queue again
                
            }
        }

        void run() override
        {
            taskService.create(
                IAction::entry,             // função da task
                "LED Task",                 // nome da task (debug)
                20480,                       // tamanho da stack
                this,                       // parâmetro opcional
                1,                          // prioridade
                nullptr,                    // handle da task (pode ser usado depois)
                0                           // core onde a task vai rodar (0 ou 1)
            );
        }

    private:
        void setLeds()
            {
                if(currentR != lastR || currentG != lastG || currentB != lastB) {
                    // Update the last received values
                    lastR = currentR;
                    lastG = currentG;
                    lastB = currentB;
                    
                    turnOnEffect.setColor(currentR, currentG, currentB); 
                    turnOnEffect.run();
                }
            }

        void setBrightness()
            {
                if(currentBrightness != lastBrightness) {
                    // Update the last received brightness
                    lastBrightness = currentBrightness;
                    
                    ledService->strip->setBrightness(currentBrightness);
                    ledService->strip->show();
                }
            }   

        void setEffect()
            {
                if(effectSelected != nullptr) {
                    effectSelected->run();
                }
            }

};