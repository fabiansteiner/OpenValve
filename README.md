<div align="center">

# OpenValve

<a href="https://open-valve.com/">Get OpenValve</a> | <a href="documents/Construction.md">Build OpenValve</a> | <a href="https://docs.open-valve.com">Documentation</a> | <a href="https://discord.gg/JMEDK97xuT">Join Discord</a>

</div>

## Introduction

OpenValve is an **open-source, 3D-printable irrigation controller**. It waters plants based on **actual soil moisture**, works with **gravity-fed systems**, and runs on a **9V block battery for about 3 months**.

![OpenValve First Impressions](documents/pics/OpenValveTeaser.gif)

OpenValve was built for irrigation. But underneath the soil moisture sensor and irrigation firmware is an open-source pinch valve that you can build other things around.


> [!NOTE]
> **Built for irrigation. Open to other ideas.**
> Add your own sensor, write your own firmware, or connect OpenValve to another system. [Explore OpenValve as a valve platform →](documents/OV-as-platform.md)


## Applications
OpenValve was designed for **balconies, garden beds, small greenhouses, and other small-to-medium irrigation setups**. It has been deployed in multiple real-world locations. Both gravity-fed and faucet-fed (`≤ 4 bar`) setups were successfully tested over full summer seasons.


![OpenValve in multiple locations](documents/pics/applications.gif)


## ✨ Key Features

### 🚰 3D-Printable Pinch Valve


- bistable (no power needed to hold the open/closed position)
- reliably actuates **down to 0 bar**, making it **perfect for gravity-fed setups**
- requires **no minimum flow**
- tolerates **imperfect or particle-rich water**  
- **8 mm inner flow path** (Cv ≈ 2)  
- Max. pressure: **4 bar** (60 psi)  

![Open Valve First Impressions](documents/pics/PinchValve.gif)

### 🌧️ Sensor-Based Irrigation

OpenValve does not water on a fixed timer. Instead, it uses a **soil moisture sensor** to decide when watering is needed.

#### How it works:

1. OpenValve regularly measures the soil moisture.
2. If the soil is drier than the threshold you set, the valve opens.
3. Water flows until the sensor detects that moisture is increasing.
4. The valve then either closes immedetiately or stays open for longer, based on the extra watering time you set.
5. OpenValve stops watering and returns to the regular soil moisture measurement.

This allows the watering frequency to adapt to the actual conditions around the plant instead of following a fixed schedule:

When the soil dries out faster, for example during hot weather or periods of increased plant demand, OpenValve waters more often. When the soil stays moist for longer, for example during cooler weather, after rainfall, or after a previous watering cycle, OpenValve waters less often.

For a more detailed explanation of the watering logic, see the ["How OpenValve works" section in the documentation.](https://docs.open-valve.com/category/how-openvalve-works) 

![How OpenValve waters](documents/pics/HowItWorksGif.gif)




### 🔗 Standard Hose & Pipe Connections

OpenValve has a standard **½″ male BSPP thread on both ends**. This thread size is common in irrigation and plumbing systems in Europe, the UK, Australia, and many other regions.

Any fitting, adapter, or component with a compatible **½″ female BSPP thread** can be connected directly to OpenValve, for example:

* compression fittings
* hose connectors
* thread adapters, such as ½″ to ¾″
* quick-connect fittings
* adapters for different hose sizes

This makes OpenValve flexible enough for setups ranging from a small balcony drip line to a greenhouse irrigation system.

For more details about installing OpenValve, see the [installation section in the documentation](https://docs.open-valve.com/installation/overview).


### 🛠️ Fully Repairable & Modifiable

OpenValve is designed to be repaired, modified, and reused.

* the 3D-printed parts can be reprinted or modified
* the silicone hose (pinch valve sleeve) can be replaced when it wears out
* the firmware can be changed for different behaviours
* the hardware and firmware allow easy integration of different sensors
* the valve can alternatively be powered from an external 5 V, 1 A supply

OpenValve was designed for irrigation, but can also be used for other projects. [Explore OpenValve as a valve platform →](documents/OV-as-platform.md)



## 💡 User Interface

OpenValve’s user interface (UI) is intentionally minimal:

- **1 button**
- **2 LEDs**: one blue LED and one bi-color LED that can show green, red, or orange

Because the device is designed to run autonomously, the UI focuses on simple, essential interactions:

- **Short and long button presses** for menu navigation and configuration  
- **LED patterns** to indicate status, watering activity, sensor readings, and possible errors

To make this easy to understand, every UI state is documented and can be explored interactively:

👉 **Interactive Manual:**  https://ui.open-valve.com/


This web-based “digital twin” mimics the behaviour of the real device - showing the same LED patterns and button interactions, along with explanations for each state.  

For a complete explanation of the button actions, LED blink patterns, and UI states, see the [User Interface documentation](https://docs.open-valve.com/category/user-interface).


## 🤝 Contributing

Contributions are welcome!  
If you’d like to help improve the firmware, CAD files, documentation, or electronics, feel free to open an issue or submit a pull request.

You can also [join the project Discord](https://discord.gg/JMEDK97xuT) to discuss ideas, ask questions, or share feedback.
