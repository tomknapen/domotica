
#include <EasyButton.h>
#include <Arduino.h>

class DomoButton
{
  public:
    DomoButton(uint8_t pin, uint32_t debounce_time = 35, bool pullup_enable = true, bool invert = true) : _button(new EasyButton(pin, debounce_time, pullup_enable, invert)) {}
    ~DomoButton(){}

  private:
    EasyButton _button;
};
