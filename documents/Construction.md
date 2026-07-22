## 🔧 How to Build OpenValve

### Assembly Guide

The [Assembly Guide Video](https://youtu.be/af_efyuOWUs) guides you through the build with detailed, step-by-step assembly instructions.

### To build your own OpenValve, you need:

- [The OpenValve Assembly Kit and a soil moisture sensor](https://lectronz.com/products/openvalve-assembly-kit-v1-0)
- An enclosed 3D printer capable of printing ASA without warping*
- ASA filament*
- 1 × 9 V alkaline battery
- Basic soldering equipment
- Basic hand tools, such as Allen keys and pliers


*Optionally, all missing 3D-printed parts can be purchased with the Assembly Kit. In that case, no 3D printer or filament is required to build OpenValve.

### Building OpenValve from Scratch

As an alternative to using the Assembly Kit, you can source all components yourself and build OpenValve completely from scratch. All required resources are available in this repository.

The hardware files—including the [PCB](./../hardware/pcb/), [3D models](./../hardware/3D-models/), and [BOM](./../hardware/)—as well as the [firmware](./../software/) are all open source.

> [!WARNING]
> If you are considering self-sourcing OpenValve, please read this carefully. This is not intended as an advertisement for the Assembly Kit, but as practical advice based on real and frustrating experiences.
>
> The 3D-printed parts included with the Assembly Kit require very tight tolerances and very high print quality to work properly. Printing these parts yourself is likely to result in components that fall outside the required specifications, even when they look correct. These small deviations often cause poor fits, creating a substantial risk of water entering the housing and damaging the electronics.
>
> For this reason, the critical 3D-printed valve components that require precise manufacturing are included in the Assembly Kit by default.
