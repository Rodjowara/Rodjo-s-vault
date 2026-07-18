#include "driver/gpio.h"

#define C1 32 
#define C2 27 
#define C3 26 
#define R1 12 
#define R2 14 
#define R3 25 
#define R4 33 

#define LONG_TIME     500000L
#define DOUBLE_TIME   200000L 

extern "C" typedef void (*ButtonEventHandler)(int key);

enum ButtonStatus{ IDLE, PRESSED, RELEASED, LONG_PRESS };

class CKeyPad{
    public:
        CKeyPad(gpio_num_t r1, gpio_num_t r2, gpio_num_t r3, gpio_num_t r4, gpio_num_t c1, gpio_num_t c2, gpio_num_t c3);
        void attachSingleClick(ButtonEventHandler method){singleClick = method;};
        void attachDoubleClick(ButtonEventHandler method){doubleClick = method;};
        void attachLongPress(ButtonEventHandler method){longPress = method;};

        void tick();
        void tick2();

    private:
        ButtonEventHandler singleClick = NULL;
        ButtonEventHandler doubleClick = NULL;
        ButtonEventHandler longPress = NULL;
        
        ButtonStatus m_buttonState = IDLE;
        bool currentState;
        int64_t startPress = 0;
        int m_lastKey = -1;
        bool m_waitingSecondPress = false;

        int64_t lastRelease = 0;
        const char *LogName = "CKeyPad";
};