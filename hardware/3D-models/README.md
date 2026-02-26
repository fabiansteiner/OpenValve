# 🧵 3D-models

All custom plastic parts of OpenValve are **designed and optimized for 3D-printing**. No generated support material required. In this document you wil find detailed information about the different parts and how to produce them yourself.


### Print Instructions

Functionality and long-term reliability is achieved only when all components are printed using the specified materials and slicer parameters.

## ASA parts

**Tested filaments:**
- Fiberlogy ASA
- Prusament ASA

!!! Do not use dark filament colors if you intend to use OpenValve outdoors and under direct sunlight. !!!

**Global Print Settings for ALL ASA parts**
- **Layer height:** 0.2 mm
- **Perimeters:** 4
- **Infill:** 100%
- **Infill pattern:** Rectilinear
- **Supports:** none
- **Fuzzy skin thickness:** 0.2 mm (only paint-on)
- **Fuzzy skin point distance:** 0.6 mm (only paint-on)

### Print-yourself ASA parts

Some ASA parts are not included in the assembly kit and must be printed by the user:

- Housing_Up
- Housing_Down
- 2x Nut_scaled
- Guide
- MotorMount

**Ready-to-print profiles for these parts are available on [Printables](https://www.printables.com/model/1564060-openvalve-missing-parts) and [Makerworld](https://makerworld.com/de/models/2277535-openvalve-missing-parts#profileId-2483396).**

### Included ASA parts in assembly kit

The following parts (fittings and a battery lid) are included in the assembly kit by default and do not have to be printed by the user:


### Fitting (2 pieces)
- Scarf joint placement: **Everywhere**
- Enable ironing: **Yes**
- Ironing type: **Topmost surface only**

The fittings require **post-processing (impregnation with Dichtol AM Hydro)** to handle water pressure. For that reason they are included in the kit.

### BatteryLid

The battery lid has an embedded qr code that links to a manual. The part must be printed on a smooth print bed, otherwise the qr code will not be detected. With a multi material system the lid should be easy to print.

If your printer does not have a multi material system, a dual-extruder system can be simulated to allow a mid-layer material change by the tweeking a few slicer settings:

#### Required PrusaSlicer Settings
- **Printers → General → Extruders:** 2
- **Printers → General → Custom G-code → Tool change G-code:** `M600`
- **Print Settings → Multiple Extruders → Wipe Tower:** off

#### Steps
1. Select two materials (both ASA).  
   - Material 1 → QR code (preferably black)  
   - Material 2 → rest of the battery lid  
2. Assign extruders:  
   - Battery lid body → **Material 2**
   - QR code bodies → **Material 1**
3. Slice the model.
4. Inspect the first layer preview.
5. Print if correct (G‑code compatibility warnings can be ignored).



## PC Parts

All required PC parts are included in the assemby kit.

**Tested filament:**
- Prusament PC Blend

### Global Print Settings
- **Layer height:** 0.15 mm
- **Perimeters:** 4
- **Infill:** 100%
- **Infill pattern:** Rectilinear
- **Cooling:** Disable fan for only the **first layer**


### Link2 (2 pieces)
*(No special settings documented, but two pieces)*

### Shell (2 pieces)
*(No special settings documented, but two pieces)*


## Other Parts

### Button
**Tested Filaments:** 
- Recreus Filaflex SEBS Black

**Print Settings**
- Layer height: 0.2 mm
- Perimeters: 4
- Infill: 100%
- Infill pattern: Rectilinear

### Knob (2 pieces)
**Tested Filaments:** 
- Fiberlogy Easy PET-G Pure Transparent

**Print Settings**
- Layer height: 0.2 mm
- Perimeters: 5

## Helper Tools

For the assembly of OpenValve some helper tools are necessary.