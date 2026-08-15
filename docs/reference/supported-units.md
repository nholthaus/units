# Supported units

*The catalog of built-in units, grouped by dimension — **47 dimensions**, **252 named units** (before metric prefixes). Generated from the headers by `docs/reference/gen_reference.py`; do not edit by hand.*

Each unit is available as a type (`meters`, `meters<double>`) and, where shown, a literal (`5.0_m`). Units marked **yes** under Prefixes also provide every SI metric prefix from femto to peta (e.g. `kilometers`/`_km`, `millimeters`/`_mm`). Include the umbrella header `<units.h>` for all of them, or a single `<units/DIMENSION.h>` for one dimension.

For units shared across dimensions (e.g. `pounds` of mass vs. force), qualify with the dimension namespace: `units::mass::pounds` vs `units::force::pounds`.

### acceleration

| Unit | Literal | Prefixes |
|------|---------|----------|
| `meters_per_second_squared` | `_mps2` |  |
| `feet_per_second_squared` | `_fps2` |  |
| `standard_gravity` | `_SG` |  |
| `gals` | `_Gal` |  |

### angle

| Unit | Literal | Prefixes |
|------|---------|----------|
| `radians` | `_rad` | yes |
| `degrees` | `_deg` |  |
| `arcminutes` | `_arcmin` |  |
| `arcseconds` | `_arcsec` |  |
| `milliarcseconds` | `_mas` |  |
| `turns` | `_tr` |  |
| `gradians` | `_gon` |  |
| `angular_mils` | `_amil` |  |
| `compass_points` | `_cpt` |  |

### angular velocity

| Unit | Literal | Prefixes |
|------|---------|----------|
| `radians_per_second` | `_rad_per_s` |  |
| `degrees_per_second` | `_deg_per_s` |  |
| `revolutions_per_minute` | `_rpm` |  |
| `revolutions_per_second` | `_rps` |  |
| `milliarcseconds_per_year` | `_mas_per_yr` |  |

### area

| Unit | Literal | Prefixes |
|------|---------|----------|
| `square_meters` | `_m2` |  |
| `square_feet` | `_ft2` |  |
| `square_inches` | `_in2` |  |
| `square_miles` | `_mi2` |  |
| `square_kilometers` | `_km2` |  |
| `hectares` | `_ha` |  |
| `acres` | `_acre` |  |
| `roods` | `_rood` |  |
| `square_rods` | `_rd2` |  |

### capacitance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `farads` | `_F` | yes |

### charge

| Unit | Literal | Prefixes |
|------|---------|----------|
| `coulombs` | `_C` | yes |
| `ampere_hours` | `_Ah` | yes |
| `abcoulombs` | `_abC` |  |
| `statcoulombs` | `_statC` |  |

### concentration

| Unit | Literal | Prefixes |
|------|---------|----------|
| `parts_per_million` | `_ppm` |  |
| `parts_per_billion` | `_ppb` |  |
| `parts_per_trillion` | `_ppt` |  |
| `percent` | `_pct` |  |

### conductance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `siemens` | `_S` | yes |

### current

| Unit | Literal | Prefixes |
|------|---------|----------|
| `amperes` | `_A` | yes |
| `abamperes` | `_abA` |  |
| `statamperes` | `_statA` |  |

### data

| Unit | Literal | Prefixes |
|------|---------|----------|
| `bytes` | `_B` |  |
| `exabytes` | `_EB` |  |
| `bits` | `_b` |  |
| `exabits` | `_Eb` |  |
| `nibbles` | `_nibble` |  |

### data transfer rate

| Unit | Literal | Prefixes |
|------|---------|----------|
| `bytes_per_second` | `_Bps` |  |
| `exabytes_per_second` | `_EBps` |  |
| `bits_per_second` | `_bps` |  |
| `exabits_per_second` | `_Ebps` |  |

### density

| Unit | Literal | Prefixes |
|------|---------|----------|
| `kilograms_per_cubic_meter` | `_kg_per_m3` |  |
| `grams_per_milliliter` | `_g_per_mL` |  |
| `kilograms_per_liter` | `_kg_per_L` |  |
| `ounces_per_cubic_foot` | `_oz_per_ft3` |  |
| `ounces_per_cubic_inch` | `_oz_per_in3` |  |
| `ounces_per_gallon` | `_oz_per_gal` |  |
| `pounds_per_cubic_foot` | `_lb_per_ft3` |  |
| `pounds_per_cubic_inch` | `_lb_per_in3` |  |
| `pounds_per_gallon` | `_lb_per_gal` |  |
| `slugs_per_cubic_foot` | `_slug_per_ft3` |  |

### energy

| Unit | Literal | Prefixes |
|------|---------|----------|
| `joules` | `_J` | yes |
| `calories` | `_cal` | yes |
| `kilowatt_hours` | `_kWh` |  |
| `watt_hours` | `_Wh` |  |
| `british_thermal_units` | `_BTU` |  |
| `british_thermal_units_iso` | `_BTU_iso` |  |
| `british_thermal_units_59` | `_BTU59` |  |
| `therms` | `_thm` |  |
| `foot_pounds` | `_ftlbf` |  |
| `ergs` | `_erg` |  |
| `calories_it` | `_cal_it` |  |
| `tons_of_tnt` | `_tTNT` |  |

### energy density

| Unit | Literal | Prefixes |
|------|---------|----------|
| `joules_per_meter_cubed` | `_J_per_m3` | yes |

### force

| Unit | Literal | Prefixes |
|------|---------|----------|
| `newtons` | `_N` | yes |
| `pounds` | `_lbf` |  |
| `dynes` | `_dyn` |  |
| `kiloponds` | `_kp` |  |
| `poundals` | `_pdl` |  |
| `kips` | `_kip` |  |
| `ounces_force` | `_ozf` |  |
| `grams_force` | `_gf` |  |
| `short_tons_force` | `_tonf` |  |
| `long_tons_force` | `_ltonf` |  |
| `sthenes` | `_sn` |  |

### frequency

| Unit | Literal | Prefixes |
|------|---------|----------|
| `hertz` | `_Hz` | yes |

### illuminance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `lux` | `_lx` | yes |
| `footcandles` | `_fc` |  |
| `lumens_per_square_inch` | `_lm_per_in2` |  |
| `phots` | `_ph` |  |

### impedance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `ohms` | `_Ohm` | yes |

### inductance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `henries` | `_H` | yes |

### irradiance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_meter_squared` | `_W_per_m2` | yes |

### jerk

| Unit | Literal | Prefixes |
|------|---------|----------|
| `meters_per_second_cubed` | `_mps3` | yes |
| `feet_per_second_cubed` | `_fps3` |  |

### length

| Unit | Literal | Prefixes |
|------|---------|----------|
| `meters` | `_m` | yes |
| `feet` | `_ft` |  |
| `inches` | `_in` |  |
| `mils` | `_mil` |  |
| `miles` | `_mi` |  |
| `nautical_miles` | `_nmi` |  |
| `astronomical_units` | `_au` |  |
| `lightyears` | `_ly` |  |
| `parsecs` | `_pc` |  |
| `angstroms` | `_angstrom` |  |
| `cubits` | `_cbt` |  |
| `fathoms` | `_ftm` |  |
| `chains` | `_ch` |  |
| `furlongs` | `_fur` |  |
| `hands` | `_hand` |  |
| `leagues` | `_lea` |  |
| `nautical_leagues` | `_nl` |  |
| `yards` | `_yd` |  |
| `rods` | `_rod` |  |
| `links` | `_li` |  |
| `barleycorns` | `_bc` |  |
| `nails` | `_nail` |  |
| `spans` | `_span` |  |
| `picas` | `_pica` |  |
| `points` | `_pnt` |  |

### luminance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `candelas_per_square_meter` | `_cd_per_m2` | yes |
| `stilbs` | `_sb` |  |
| `apostilbs` | `_asb` |  |
| `brils` | `_bril` |  |
| `skots` | `_sk` |  |
| `lamberts` | `_la` |  |
| `millilamberts` | `_mla` |  |
| `foot_lamberts` | `_ftL` |  |

### luminous flux

| Unit | Literal | Prefixes |
|------|---------|----------|
| `lumens` | `_lm` | yes |

### luminous intensity

| Unit | Literal | Prefixes |
|------|---------|----------|
| `candelas` | `_cd` | yes |

### magnetic field strength

| Unit | Literal | Prefixes |
|------|---------|----------|
| `teslas` | `_Te` | yes |
| `gauss` | `_G` |  |

### magnetic flux

| Unit | Literal | Prefixes |
|------|---------|----------|
| `webers` | `_Wb` | yes |
| `maxwells` | `_Mx` |  |

### mass

| Unit | Literal | Prefixes |
|------|---------|----------|
| `grams` | `_g` | yes |
| `tonnes` | `_t` |  |
| `pounds` | `_lb` |  |
| `long_tons` | `_ln_conversion_factor` |  |
| `short_tons` | `_sh_conversion_factor` |  |
| `stone` | `_st` |  |
| `ounces` | `_oz` |  |
| `carats` | `_ct` |  |
| `slugs` | `_slug` |  |
| `grains` | `_gr` |  |
| `avoirdupois_drams` | `_dr_av` |  |
| `pennyweights` | `_dwt` |  |
| `troy_ounces` | `_ozt` |  |
| `troy_pounds` | `_lbt` |  |
| `hundredweights` | `_cwt` |  |
| `short_hundredweights` | `_sh_cwt` |  |

### power

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts` | `_W` | yes |
| `horsepower` | `_hp` |  |
| `metric_horsepower` | `_hpM` |  |
| `electrical_horsepower` | `_hpE` |  |
| `tons_of_refrigeration` | `_TR` |  |

### pressure

| Unit | Literal | Prefixes |
|------|---------|----------|
| `pascals` | `_Pa` | yes |
| `bars` | `_bar` |  |
| `millibars` | `_mbar` |  |
| `atmospheres` | `_atm` |  |
| `pounds_per_square_inch` | `_psi` |  |
| `torrs` | `_torr` |  |
| `millimeters_of_mercury` | `_mmHg` |  |
| `inches_of_mercury` | `_inHg` |  |
| `technical_atmospheres` | `_at` |  |
| `pounds_per_square_foot` | `_psf` |  |
| `kips_per_square_inch` | `_ksi` |  |
| `baryes` | `_Ba` |  |
| `piezes` | `_pz` |  |
| `centimeters_of_water` | `_cmH2O` |  |
| `millimeters_of_water` | `_mmH2O` |  |
| `inches_of_water` | `_inH2O` |  |

### radiance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_steradian_per_meter_squared` | `_W_per_srm2` | yes |

### radiant intensity

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_steradian` | `_W_per_sr` | yes |

### radiation

| Unit | Literal | Prefixes |
|------|---------|----------|
| `becquerels` | `_Bq` | yes |
| `grays` | `_Gy` | yes |
| `sieverts` | `_Sv` | yes |
| `curies` | `_Ci` |  |
| `rutherfords` | `_rd` |  |
| `radiation_absorbed_dose` | `_rads` |  |
| `roentgens_equivalent_man` | `_rem` |  |

### solid angle

| Unit | Literal | Prefixes |
|------|---------|----------|
| `steradians` | `_sr` | yes |
| `degrees_squared` | `_deg2` |  |
| `spats` | `_sp` |  |

### spectral flux

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_meter` | `_W_per_m` | yes |

### spectral intensity

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_steradian_per_meter` | `_W_per_srm` | yes |

### spectral irradiance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_meter_cubed` | `_W_per_m3` | yes |

### spectral radiance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_steradian_per_meter_cubed` | `_W_per_srm3` | yes |

### substance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `mols` | `_mol` | yes |
| `pound_moles` | `_lbmol` |  |

### substance concentration

| Unit | Literal | Prefixes |
|------|---------|----------|
| `molars` | `_M` | yes |

### substance mass

| Unit | Literal | Prefixes |
|------|---------|----------|
| `grams_per_mole` | `_g_per_mol` | yes |

### temperature

| Unit | Literal | Prefixes |
|------|---------|----------|
| `kelvin` | `_K` |  |
| `celsius` | `_degC` |  |
| `fahrenheit` | `_degF` |  |
| `reaumur` | `_Re` |  |
| `rankine` | `_Ra` |  |

### time

| Unit | Literal | Prefixes |
|------|---------|----------|
| `seconds` | `_s` | yes |
| `minutes` | `_min` |  |
| `hours` | `_hr` |  |
| `days` | `_d` |  |
| `weeks` | `_wk` |  |
| `years` | `_yr` |  |
| `julian_years` | `_a_j` |  |
| `gregorian_years` | `_a_g` |  |
| `fortnights` | `_fn` |  |
| `decades` | `_dec` |  |
| `centuries` | `_cent` |  |
| `millennia` | `_kyr` |  |

### torque

| Unit | Literal | Prefixes |
|------|---------|----------|
| `newton_meters` | `_Nm` |  |
| `foot_pounds` | `_ftlb` |  |
| `foot_poundals` | `_ftpdl` |  |
| `inch_pounds` | `_inlb` |  |
| `meter_kilograms` | `_mkgf` |  |

### velocity

| Unit | Literal | Prefixes |
|------|---------|----------|
| `meters_per_second` | `_mps` |  |
| `feet_per_second` | `_fps` |  |
| `miles_per_hour` | `_mph` |  |
| `kilometers_per_hour` | `_kph` |  |
| `knots` | `_kts` |  |
| `feet_per_minute` | `_fpm` |  |
| `meters_per_minute` | `_mpm` |  |
| `inches_per_second` | `_ips` |  |
| `kilometers_per_second` | `_kmps` |  |

### voltage

| Unit | Literal | Prefixes |
|------|---------|----------|
| `volts` | `_V` | yes |
| `statvolts` | `_statV` |  |
| `abvolts` | `_abV` |  |

### volume

| Unit | Literal | Prefixes |
|------|---------|----------|
| `cubic_meters` | `_m3` |  |
| `cubic_millimeters` | `_mm3` |  |
| `cubic_kilometers` | `_km3` |  |
| `liters` | `_L` | yes |
| `cubic_inches` | `_in3` |  |
| `cubic_feet` | `_ft3` |  |
| `cubic_yards` | `_yd3` |  |
| `cubic_miles` | `_mi3` |  |
| `gallons` | `_gal` |  |
| `quarts` | `_qt` |  |
| `pints` | `_pt` |  |
| `cups` | `_c` |  |
| `fluid_ounces` | `_fl_oz` |  |
| `barrels` | `_bl` |  |
| `bushels` | `_bu` |  |
| `cords` | `_cord` |  |
| `cubic_fathoms` | `_fm3` |  |
| `tablespoons` | `_tbsp` |  |
| `teaspoons` | `_tsp` |  |
| `pinches` | `_pinch` |  |
| `dashes` | `_dash` |  |
| `drops` | `_drop` |  |
| `fifths` | `_fifth` |  |
| `drams` | `_dr` |  |
| `gills` | `_gi` |  |
| `pecks` | `_pk` |  |
| `sacks` | `_sck` |  |
| `shots` | `_shts` |  |
| `strikes` | `_strk` |  |
