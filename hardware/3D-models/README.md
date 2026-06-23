# 🧵 3D Models

All custom plastic parts of OpenValve are **designed and optimized for 3D printing**. No generated support material is required.

This document contains information about the parts and how to produce them yourself.

> [!WARNING]
> Functionality and long-term reliability can only be achieved when all components are printed using the specified materials and slicer parameters.

## ASA Parts

### Tested Filaments

* Fiberlogy ASA
* Prusament ASA

> [!CAUTION]
> Do not use dark filament colors if you intend to use OpenValve outdoors in direct sunlight. Dark colors heat up to higher temperatures under the same conditions and may deform over time.

### Global Slicer Settings

Use these settings for **all ASA parts**, unless stated otherwise in the part-specific section.

| Setting                   | Value                  |
| ------------------------- | ---------------------- |
| Nozzle size               | 0.4 mm                 |
| Layer height              | 0.2 mm                 |
| Perimeters                | 4                      |
| Infill                    | 100%                   |
| Infill pattern            | Rectilinear            |
| Supports                  | None                   |
| Fuzzy skin thickness      | 0.2 mm — paint-on only |
| Fuzzy skin point distance | 0.6 mm — paint-on only |

### Print-Yourself ASA Parts

Some ASA parts can either be purchased with the assembly kit or printed by the user:

* `Housing_Up`
* `Housing_Down`
* `Nut_scaled` ×2
* `Guide`
* `MotorMount`
* `Slider`

Ready-to-print profiles for these parts are available on [Printables](https://www.printables.com/model/1564060-openvalve-missing-parts) and [MakerWorld](https://makerworld.com/de/models/2277535-openvalve-missing-parts#profileId-2483396).

Detailed print instructions for these parts can also be found there.

### ASA Parts Included in the Assembly Kit

The following ASA parts are included in the assembly kit by default. They are more difficult to produce reliably, but the settings below are provided for users who still want to print them themselves.

### Part-Specific Slicer Settings

#### Fitting (2 pieces)

The fittings require post-processing by impregnating them with **Dichtol AM Hydro** to handle water pressure. For this reason, they are included in the assembly kit.

| Setting               | Value                |
| --------------------- | -------------------- |
| Scarf joint placement | Everywhere           |
| Enable ironing        | Yes                  |
| Ironing type          | Topmost surface only |

#### Battery Lid

The battery lid has an embedded QR code that needs 2 different ASA filaments, a smooth print sheet and very specific slicer settings if printed on a single-extruder system. For that reason it is included in the assembly kit by default.

With a multi-material system, the battery lid should be easy to print. 

If your printer does not have a multi-material system, a dual-extruder system can be simulated to allow a mid-layer material change by tweaking a few slicer settings (PrusaSlicer):

| Path                                                      | Setting |
| --------------------------------------------------------- | ------- |
| `Printers → General → Extruders`                          | `2`     |
| `Printers → General → Custom G-code → Tool change G-code` | `M600`  |
| `Print Settings → Multiple Extruders → Wipe Tower`        | Off     |

##### Steps

1. Select two materials (both ASA).

   * Material 1 → QR code (preferably black)
   * Material 2 → rest of the battery lid
2. Assign extruders:

   * Battery lid body → **Material 2**
   * QR code bodies → **Material 1**
3. Slice the model.
4. Inspect the first layer preview.
5. Print if correct

> [!NOTE]
> G-code compatibility warnings can be ignored.

## PC Parts

All PC parts are included in the assembly kit by default.

They are included because they must stay within very narrow tolerances and require highly repeatable printing results. They also require special assembly tools and precise post-processing.

In addition, polycarbonate is not a commonly used 3D-printing material and may not be readily available to every user.

### Tested Filament

* Prusament PC Blend

### Global Slicer Settings

Use these settings for **all PC parts**, unless stated otherwise in the part-specific section.

| Setting        | Value                                    |
| -------------- | ---------------------------------------- |
| Layer height   | 0.15 mm                                  |
| Perimeters     | 4                                        |
| Infill         | 100%                                     |
| Infill pattern | Rectilinear                              |
| Cooling        | Disable the fan for the first layer only |

### Part-Specific Slicer Settings

#### Link2 (2 pieces)

#### Shell (2 pieces)


## Other Parts


### Button

**Tested filament**

* Recreus Filaflex SEBS Black

#### Part-Specific Slicer Settings

| Setting            | Value       |
| ------------------ | ----------- |
| Nozzle temperature | 260 °C      |
| Layer height       | 0.2 mm      |
| Perimeters         | 4           |
| Infill             | 100%        |
| Infill pattern     | Rectilinear |

### Knob (2 pieces)

**Tested filament**

* Fiberlogy Easy PET-G Pure Transparent

#### Part-Specific Slicer Settings

| Setting      | Value  |
| ------------ | ------ |
| Layer height | 0.2 mm |
| Perimeters   | 5      |
