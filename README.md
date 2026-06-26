<div align="center">

# OpenValve

<a href="#availability">Get OpenValve</a> | <a href="documents/Construction.md">Build OpenValve</a> | <a href="https://docs.open-valve.com">Documentation</a> | <a href="https://discord.gg/JMEDK97xuT">Join Discord</a>

</div>








## Introduction

OpenValve is an **open-source, 3D-printable irrigation controller**. It waters plants based on **actual soil moisture**, works with **gravity-fed systems**, and runs on a **9V block battery for about 3 months**.

![OpenValve First Impressions](documents/pics/OpenValveTeaser.gif)

OpenValve was originally designed for irrigation. The soil moisture sensor and the firmware turn it into an irrigation controller.

Take those irrigation-specific parts away, and you are left with an **open-source pinch valve**. From there, the project is yours to shape: add your own sensor, write your own firmware, or connect it to another system and use it in a completely different way.

> [!NOTE]
> **OpenValve comes with one finished application.** What you build around the valve is up to you.


That is why it is called OpenValve.








## ✨ Key Features

### 🚰 3D-Printable Pinch Valve


- valve actuation works **down to 0 bar**, making it **perfect for gravity-fed setups**
- requires **no minimum flow** and **no minimum pressure**  
- tolerates **imperfect or particle-rich water**  
- provides a straight **8 mm inner flow path** (Cv ≈ 2)  
- handles pressures up to **2 bar** (0–30 psi)  

![Open Valve First Impressions](documents/pics/PinchValve.gif)

### 🌧️ Sensor-Based Irrigation

OpenValve does not water on a fixed timer. Instead, it uses a soil moisture sensor to decide when watering is needed.

The basic process is:

1. OpenValve regularly measures the soil moisture.
2. If the soil is drier than the threshold you set, the valve opens.
3. Water flows until the sensor detects that moisture is increasing.
4. The valve then stays open a little longer, based on the extra watering time you set.
5. The valve closes again.

This allows the watering frequency to adapt to the actual conditions around the plant instead of following a fixed schedule:

When the soil dries out faster, for example during hot weather or periods of increased plant demand, OpenValve waters more often. When the soil stays moist for longer, for example during cooler weather, after rainfall, or after a previous watering cycle, OpenValve waters less often.

For a more detailed explanation of the watering logic, see the ["How OpenValve works" section in the documentation.](https://docs.open-valve.com/category/how-openvalve-works) 



### 🔗 Standard Hose & Pipe Connections

OpenValve has a standard **½″ male BSPP thread on both ends**. This thread size is common in irrigation and plumbing systems in Europe, the UK, Australia, and many other regions.

With the right adapters, OpenValve can be connected to many different hose and pipe systems, for example:

* compression fittings
* hose connectors
* thread adapters, such as **½″ to ¾″**
* quick-connect fittings
* different hose sizes

This makes it flexible enough for setups ranging from a small balcony drip line to a greenhouse irrigation system.

For more details about installing OpenValve, see the [installation section in the documentation](https://docs.open-valve.com/installation/overview).


### Fully Repairable & Modifiable

OpenValve was designed as an irrigation controller, but the core of the project is a compact, battery-powered pinch valve.

For the irrigation use case, the valve is combined with a soil moisture sensor and firmware that decides when to water. With different firmware or sensors, the same valve platform can be adapted for other low-pressure fluid-control projects where low power consumption, fast actuation, and low internal flow resistance are useful.

Because OpenValve is a pinch valve, the fluid does not need to touch the valve mechanism itself. If a continuous hose is routed through the valve, the fluid only comes into contact with the inside of that hose. With a suitable certified hose material, this can also make the fluid path suitable for applications where material compatibility or food safety matters.

OpenValve is also designed to be repairable and modifiable:

* the 3D-printed parts can be reprinted or modified
* the silicone hose can be replaced when it wears out
* the firmware can be changed for different behaviours
* the electronics can be reused with other analog sensors
* the valve can be powered from a 9V battery or an external 5V supply

This makes OpenValve not only a practical irrigation controller, but also a starting point for custom low-power valve projects.



## Applications
OpenValve was designed for **balconies, garden beds, small greenhouses, and other small-to-medium irrigation setups**. It has been deployed in multiple real-world locations. Both gravity-fed and faucet-fed (`≤ 2 bar`) setups were tested over full summer seasons.


![OpenValve in multiple locations](documents/pics/applications.gif)


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



## Availability

### Can I get an OpenValve already?

**Not yet.**  
OpenValve is currently still in the prototype stage and is **not yet available as a finished product**.

### Why is it not available yet?

Turning OpenValve from a working prototype into a real product requires additional steps such as:

- certification / compliance
- small-batch production setup
- packaging and logistics
- support infrastructure

These steps come with significant cost and effort.

### I want one — what should I do?

If you’d like to get an OpenValve for yourself once it becomes available, you can join the waitlist here:

**[Join the OpenValve waitlist](https://open-valve.com/)**

### What is the waitlist for?

The waitlist helps me measure real demand and decide whether it makes sense to invest in the next steps needed to turn OpenValve into a product. If you join the waitlist, you’ll also be among the first to hear about updates on availability.


## 🤝 Contributing

Contributions are welcome!  
If you’d like to help improve the firmware, CAD files, documentation, or electronics, feel free to open an issue or submit a pull request.

You can also [join the project Discord](https://discord.gg/JMEDK97xuT) to discuss ideas, ask questions, or share feedback.
