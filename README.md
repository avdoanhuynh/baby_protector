# baby_protector

**Application**
To be used in baby room, in order to detect and give warning if the room temperature too low/high or when the baby is crying

**Function**
- Constant measurement of the room temperature.
- Detect noises (Ex: when the baby is crying)
- Different alarm through LEDs and Buzzer depending on occurring situations.
- Additional messages on the TFT-display.
- Consideration of the user's setting.

**Hardware**
- MySignal-Board
- Input: KY-038 Mikrofon Sound Sensor Modul, Body Temperature Sensor PRO for MySignals, Potentiometer, TFT Touchboard
- Output: LEDs, Buzzer, TFT Touchboard

**Different situations**
1. 
- Input: baby is quiet, room temperature between 18-28 degree Celcius
- Output: green LED, no Buzzer, On Display: "Everything is OK!"
2. 
- Input: room temperature too high (>28 degree Celscius) or too low (<18 degree Celcius)
- Output: red LED, alarm through Buzzer, On Display "Temp too high" or "Temp too low"
3. 
- Input: baby is crying
- Output: yellow LED, alarm through Buzzerm On Display: "Baby is crying"

**Start/Stop**
"Start" and "Stop" buttons on Touchscreen. Stop the alarm through pressing "Stop" button