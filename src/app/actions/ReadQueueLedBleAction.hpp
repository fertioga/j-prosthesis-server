#pragma once

#include "bootstrap.hpp"

class ReadQueueLedBleAction
{
    public:
        TaskService taskService;   

        ReadQueueLedBleAction(): taskService(TaskService()) {}
       
        static void task(void* pvParameters)
        {
            Serial.println("Task de leitura da fila LED BLE iniciada.");

            uint8_t brightness;

            while (true)
            {
                if (xQueueReceive(QueuesConfig::instance().ledQueue, &brightness, portMAX_DELAY))
                {
                    Serial.print("Valor recebido na task: ");
                    Serial.println(brightness);
                    ledcWrite(0, brightness);
                }
                else
                {
                    Serial.println("Erro ao receber da fila.");
                }

                Serial.println("Aguardando próximo comando...");
                delay(100); // Pequena pausa para evitar saturação do log
            }
        }

        void run()
        {
            taskService.create(
                ReadQueueLedBleAction::task,// função da task
                "LED Task",                 // nome da task (debug)
                2048,                       // tamanho da stack
                QueuesConfig::instance().ledQueue,         // parâmetro opcional
                1,                          // prioridade
                nullptr                     // handle da task (pode ser usado depois)
            );
        }
};