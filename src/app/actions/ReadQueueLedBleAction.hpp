#pragma once

#include "bootstrap.hpp"
#include "IAction.hpp"

#include "app/service/LED/NeoPixelService.hpp"
#include "app/structs/LedPayloadBle.hpp"

#include "app/effects/LED/NeoPixel/ColorWipeEffect.hpp"


/*
    Protocol for LED control via BLE:
    - The BLE characteristic will receive a payload of 5 bytes, structured as follows:
    Byte 0: Red intensity (0-255)
    Byte 1: Green intensity (0-255)
    Byte 2: Blue intensity (0-255)
    Byte 3: Brightness (0-255)
    Byte 4: Effects:
        0: No effect (static color)
        1: Breathing effect
        2: Flashing effect
        3: Rainbow effect
        4: Color Wipe effect
        5: Theater Chase effect
        6: Strobe effect

*/



class ReadQueueLedBleAction : public IAction
{
    public:
        TaskService taskService;   
        NeoPixelService* ledService;

        ReadQueueLedBleAction(NeoPixelService* ledService): taskService(TaskService()), ledService(ledService) {}
       
        void task(void* pvParameters)
        {
            Serial.println("Task de leitura da fila LED BLE iniciada.");

            LedPayloadBle payload;

            while (true)
            {
                if (xQueueReceive(QueuesConfig::instance().ledQueue, &payload, portMAX_DELAY))
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

                    switch (payload.effect)
                    {
                    case 0: // No effect (static color)
                        Serial.println("Aplicando cor estática...");
                        //ColorWipeEffect colorWipeEffect(&ledService, ledService.strip(payload.r, payload.g, payload.b), payload.brigthness);
                        //ledService.setPixelColor(0, payload.r, payload.g, payload.b);
                        ledService->strip->setPixelColor(0, payload.r, payload.g, payload.b);
                        ledService->strip->setBrightness(payload.brigthness);
                        ledService->strip->show();
                        //colorWipeEffect.run();
                        break;
                    
                    default:
                        break;
                    }
                }
                else
                {
                    Serial.println("Erro ao receber da fila.");
                }

                Serial.println("Aguardando próximo comando...");
                vTaskDelay(100 / portTICK_PERIOD_MS); // wait before checking the queue again
            }
        }

        void run() override
        {
            taskService.create(
                IAction::entry,             // função da task
                "LED Task",                 // nome da task (debug)
                2048,                       // tamanho da stack
                this,                       // parâmetro opcional
                1,                          // prioridade
                nullptr                     // handle da task (pode ser usado depois)
            );
        }
};