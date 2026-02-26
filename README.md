# 🌱 OpenValve - The 3D-Printable Irrigation Valve

[(Soon) Get OpenValve]() | [Build OpenValve](documents/Construction.md) | [Getting Started](documents/GettingStarted.md) | [Join Discord](https://discord.gg/JMEDK97xuT)

----

OpenValve is an open-source **irrigation and liquid-control valve**, built around a custom pinch-valve mechanism and optimized entirely for 3D printing. It waters plants based on **actual soil moisture**, works with **gravity-fed systems**, and runs on a 9V battery for **about 3 months**.


![Open Valve First Impressions](documents/pics/OpenValveTeaser.gif)

--- 

# ✨ Key Features

## 🚰 3D-Printable Pinch Valve


- works from **0 bar**, ideal for gravity-fed setups  
- requires **no minimum flow** and **no minimum pressure**  
- tolerates **imperfect or particle-rich water**  
- provides a straight **8 mm inner flow path** (Cv ≈ 2)  
- handles pressures up to **2 bar** (0–30 psi)  

![Open Valve First Impressions](documents/pics/PinchValve.gif)

## 🌧️ Sensor-Based Irrigation
OpenValve waters plants only when they need it - not on a schedule - by the help of a **soil moisture sensor**. 

### How it works

OpenValve regularly takes measurements from the connected soil moisture sensor.

When the soil gets dry (below a user-defined threshold):

1. The valve opens automatically
2. Water flows until measured soil moisture rises (+ user-defined extra time)
3. The valve closes automatically  


This cycle automatically adapts to plant demands and rainfall.  

No timers.  No schedules.  
Just plants getting exactly what they need.

![How Open Valve operates](documents/pics/HowItWorksGif.gif)

## 🔗 Universal Hose & Pipe Compatibility

OpenValve uses a standard **½″ BSPP thread on both ends**, making it easy to integrate into a wide range of irrigation hardware:

- garden hoses  
- quick-connect systems  
- compression fittings  
- micro-irrigation adapters  
- rainwater tank outlets  

Whether you’re setting up a balcony drip line or a small greenhouse system, OpenValve fits right in.

## 🛠️ Fully Repairable & Modifiable
Because every custom plastic part is 3D-printable and source files are open:
- print replacement parts  
- tweak the housing
- make custom mounts
- modify firmware
- integrate in your own projects

Besides a soil moisture sensor OpenValve can be used with **any other 0–3.3V analog sensor**, for example:

- potentiometers
- temperature sensors  
- distance sensors
- weight sensors
- custom DIY sensors  

--- 

# Applications
OpenValve was designed for **balconies, garden beds, small greenhouses, and other small-to-medium irrigation setups**. It has been deployed across multiple real-world locations. Both gravity-fed and faucet-fed (<= 2 bar) setups were tested through whole summer seasons.


![OpenValve ins multiple locations](documents/pics/applications.gif)


# 💡 User Interface

OpenValve’s user interface is intentionally minimal:

- **1 button**
- **2 LEDs**  (one blue, one bi-color: green/red → also orange when combined)

Because the device is designed to run autonomously, the UI focuses on simple, essential interactions:

- **Short and long button presses** for menu navigation and configuration  
- **LED patterns** to indicate status, watering activity, sensor readings, and possible errors

To make this easy to understand, every UI state is documented and can be explored interactively:

👉 **Interactive Manual:**  https://ui.open-valve.com/


This web-based “digital twin” mimics the behaviour of the real device - showing the same LED patterns and button interactions, along with explanations for each state.  
It’s the easiest way to learn how OpenValve thinks and operates.




---






# ⚠️ Prototype Status & Disclaimer

Right now OpenValve is a **prototype** intended for educational, experimental, and development use.

It is **not** a consumer product.

By using any files from this repository, you acknowledge that:
- No warranty or guarantee is provided  
- All assembly and use is at your own risk  
- You are responsible for ensuring safe operation  



---

# 🤝 Contributing

Contributions are welcome!  
If you’d like to help improve the firmware, CAD files, documentation, or electronics, feel free to open an issue or submit a pull request.

You can also [join the project discord](https://discord.gg/JMEDK97xuT) to discuss ideas, ask questions, etc...
