#include "app/effects/Sound/ISound.hpp"
#include "app/service/position/accelerometer/MT6580/Mt6580Service.hpp"
#include "app/service/fileSystem/IFileSystemConfig.hpp"

class SoundMoveRoboAction: public IAction
{
public:

    SoundMoveRoboAction(Mt6580Service* mt6580Service, ISound* sound): 
        taskService(TaskService()),
        sound(sound),
        mt6580Service(mt6580Service){}

    void task(void* pvParameters)
    {
        while (true)
        {          
            mt6580Service->detectStep(
                mt6580Service->calcMov(),
                *sound
            );

            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }

    void run() override
    {
        taskService.create(
            IAction::entry,
            "Sound Robot Move Task",
            4096,   
            this,
            1,
            nullptr,
            0
        );
    }

private:
    Mt6580Service* mt6580Service;
    ISound* sound;
    TaskService taskService; 
};