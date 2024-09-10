# Real latching relay module controlled by ESP32 and how to minimize its electrical current in deepsleep mode
**The advantage of a bistable relay is that it remains in its state when its power supply is no longer provided.** When you search for a bistable relay module, you will often notice that the relay component itself is not bistable. In most cases, it's the modules that are bistable. If you remove the power from these modules, their relay returns to its default state.

So far, I have only found one module that includes a true bistable relay controlled with two coils: the **DFRobot Magnetic Latching Relay Module.**

![Alt text](/Images/DFRobot_Magnetic_latching_relay_module_V1.jpg "a title")

If the power to this relay is cut, the relay stays in its position. This module can operate entirely on 3.3V, meaning both its power supply and the signals controlling the coils. At 3.3V, the static power consumption of the module is approximately 840µA, which is low, but still significant for the system I want to create. 

Indeed, having an **ESP32 T18_3.0 module** powered by a 3.6V rechargeable 18650 Li-Ion battery (blue lines are pin used for the project):

![Alt text](/Images/TTGO_T18_V3.png)
![Alt text](/Images/TTGO_T18_V3_back_Side.png)

I want this module to fully power the relay module. However, if I continuously power the relay module using a **Samsung INR18650-35E 3500mAh battery,** I would only have about 5 months of autonomy in full deepsleep mode. On the other hand, if I control the relay module's power via a **PNP transistor,** the autonomy would be about 2 years and 5 months, theoretically, excluding the battery's self-discharge effect. 

If you refer to the first graph on this excellent site: [link](https://lygte-info.dk/review/batteries2012/Samsung%20INR18650-35E%203500mAh%20%28Pink%29%20UK.html), the top red curve shows the discharge curve of this battery model at a continuous current of 200mA. We can observe that when the battery voltage drops to 3.0V, the battery will have delivered approximately 3.3Ah at that point.

Since my coil control signals driven by the ESP32 kit and the module's power supply share a common ground, I chose to use a **PNP transistor.** I selected the **S8550,** which is in a TO-92 package and available in my stock.

To use this transistor as a switch, I need to saturate it during operation. The gain at these voltages and currents is about 120. The relay module requires 137mA at 3.3V to switch the relay. I'll consider it needs 150mA.


$Ib = Ic/\beta = 0.15A/120 = 0.00125A$ $(1.25mA)$

I multiply this value by 3 for a good safety margin: $1.25mAx4 = 3.75mA$ $(0.00375A)$

$Rb = (Vin - Vbe)/Ib = (3.3V - 0.9V)/0.00357A = 2.4V/0.00375A = 640Ω$ Therefore, I select a standard 680Ω resistor


**Here is my electrical diagram:**
![Alt text](/Images/Scheme_ESP32_DFRobot_Relay.png)

In my test software, I initialize **GPIO27, GPIO26, and GPIO25** as **OUTPUT.** To power the relay module, I need to set **GPIO27 to 0V (LOW).** To turn it off, I set it to **3.3V (HIGH).** Therefore, by default, I set **GPIO27 to HIGH** at the start of my ESP32 and in deepsleep mode.

After testing, I did not notice any difference in battery consumption, with or without the circuit, **in deepsleep mode: approximately 155µA**. This consumption is far from what the ESP32 promises, but it is an **ESP32-WROVER-B module** containing 8MB of PSRAM and 4MB of FLASH. The circuit also includes several components (voltage divider bridges, buck-boost, LDO) leading to such consumption. There are other, more recent ESP32 kits that consume much less than 155µA in deepsleep mode, but the **T_18 module** provides me with sufficient autonomy in deepsleep mode.

I developed a test source code that does the following: set the 3 GPIOs to **OUTPUT mode,** set **GPIO27 to HIGH** to turn off the relay module, and set **GPIO26 and GPIO25 to LOW.** Then, I switch the relay to **A-COM mode,** then to **B-COM mode,** back to **A-COM mode,** and then to **B-COM mode** again. 

Next, I initialize **GPIO27** to keep its **HIGH state** in deepsleep mode. Then I put the **ESP32 module into deepsleep mode for 10 seconds.** Afterward, the module will restart its sequence indefinitely, approximately every 13 seconds.

To switch the relay to **A-COM mode,** I must do the following:
1. Power the relay module by setting **GPIO27 to LOW**
2. Wait about 10ms for the relay module to be properly powered
3. Create a 3.3V pulse of approximately 5ms on **GPIO26**
4. Wait about 10ms to ensure the switch has toggled
5. Turn off the relay module

The manufacturer indicates that the two signals controlling the coils should not be **HIGH simultaneously.** For safety, I set **GPIO26 and GPIO25** to **LOW** and then wait 5ms before powering the relay module.
