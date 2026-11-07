#ifndef NL_CONFIG_H
#define NL_CONFIG_H

/*
  EIVORIS SHADER CONFIGURATION
  The current material build still uses the Newb X Legacy RenderDragon
  framework. These values are consumed by that framework during compilation.
*/

/* Color response */
#define NL_TONEMAP_TYPE 4
#define NL_GAMMA 1.00
#define NL_EXPOSURE 1.10
#define NL_SATURATION 1.02

/* World lighting — balanced cinematic daylight */
#define NL_SUNLIGHT_INTENSITY   2.45
#define NL_TORCHLIGHT_INTENSITY 1.18
#define NL_SHADOW_INTENSITY     0.78
#define NL_MIN_LIGHTING_BOOST   0.32
//#define NL_BLINKING_TORCH
//#define NL_CLOUD_SHADOW

/* Ambient light for nether/end */
#define NL_NETHER_AMBIENT vec3(3.0,2.16,1.89)
#define NL_END_AMBIENT    vec3(1.98,1.25,2.3)

/* Sun/moon light color */
#define NL_DAWN_SUNLIGHT_COL   vec3(1.0,0.70,0.52)
#define NL_NOON_SUNLIGHT_COL   vec3(1.0,0.97,0.93)
#define NL_NIGHT_MOONLIGHT_COL vec3(0.10,0.15,0.25)

/* Torch colors */
#define NL_OVERWORLD_TORCH_COL  vec3(1.0,0.42,0.15)
#define NL_UNDERWATER_TORCH_COL vec3(0.32,0.68,1.0)
#define NL_NETHER_TORCH_COL     vec3(1.0,0.38,0.12)
#define NL_END_TORCH_COL        vec3(0.78,0.62,1.0)

/* Fog */
#define NL_FOG 0.88
#define NL_MIST_DENSITY 0.12
#define NL_RAIN_MIST_OPACITY 0.08
#define NL_CLOUDY_FOG 0.08

/* Sky */
#define NL_SKY_VOID_FACTOR     0.5
#define NL_SKY_VOID_DARKNESS   0.3
#define NL_SKY_RAIN_MIX_FACTOR 0.9

#define NL_DAWN_ZENITH_COL   vec3(0.16,0.30,0.52)
#define NL_DAWN_HORIZON_COL  vec3(1.25,0.48,0.32)
#define NL_DAWN_EDGE_COL     vec3(1.25,0.72,0.52)
#define NL_DAY_ZENITH_COL    vec3(0.34,0.59,0.90)
#define NL_DAY_HORIZON_COL   vec3(0.88,0.94,1.00)
#define NL_DAY_EDGE_COL      vec3(0.98,0.99,1.00)
#define NL_NIGHT_ZENITH_COL  vec3(0.008,0.048,0.08)
#define NL_NIGHT_HORIZON_COL vec3(0.02,0.06,0.1)
#define NL_NIGHT_EDGE_COL    vec3(0.04,0.08,0.1)
#define NL_RAIN_ZENITH_COL   vec3(0.34,0.40,0.48)
#define NL_RAIN_HORIZON_COL  vec3(0.53,0.59,0.66)
#define NL_END_ZENITH_COL    vec3(0.08,0.001,0.1)
#define NL_END_HORIZON_COL   vec3(0.6,0.02,0.6)

/* Water surface and tint */
#define NL_WATER_TRANSPARENCY 0.38
#define NL_WATER_BUMP 0.16
#define NL_WATER_WAVE_SPEED 1.02
#define NL_WATER_TEX_OPACITY 0.04
#define NL_WATER_WAVE
#define NL_WATER_TINT vec3(0.68,0.86,0.96)

/* Underwater */
#define NL_UNDERWATER_BRIGHTNESS 0.95
#define NL_CAUSTIC_INTENSITY 3.15
#define NL_UNDERWATER_WAVE 0.14
#define NL_UNDERWATER_STREAKS 1.0
#define NL_UNDERWATER_TINT vec3(0.78,0.93,1.0)

/* Keep vanilla cloud rendering while EIVORIS water is developed */
#define NL_CLOUD_TYPE 0
#define NL_CLOUD0_THICKNESS 2.1
#define NL_CLOUD0_RAIN_THICKNESS 4.0
#define NL_CLOUD0_OPACITY 0.9

/* Other framework defaults retained for compatibility */
#define NL_RAINBOW
#define NL_RAINBOW_CLEAR 0.0
#define NL_RAINBOW_RAIN 0.4
/* EIVORIS custom bloom disabled; retain no glow pass. */
#define NL_GLOW_TEX 0.0
#undef NL_GLOW_SHIMMER
#undef NL_GLOW_SHIMMER_SPEED
#define NL_PLANTS_WAVE 0.05
#define NL_LANTERN_WAVE 0.16
#define NL_WAVE_SPEED 2.8
#define NL_WAVE_RANGE 13.0
#define NL_CLOUD1_SCALE vec2(0.016, 0.022)
#define NL_CLOUD1_DEPTH 1.3
#define NL_CLOUD1_SPEED 0.04
#define NL_CLOUD1_DENSITY 0.54
#define NL_CLOUD1_OPACITY 0.9
#define NL_CLOUD2_THICKNESS 2.1
#define NL_CLOUD2_RAIN_THICKNESS 2.5
#define NL_CLOUD2_STEPS 5
#define NL_CLOUD2_SCALE vec2(0.033, 0.033)
#define NL_CLOUD2_SHAPE vec2(0.5, 0.4)
#define NL_CLOUD2_DENSITY 25.0
#define NL_CLOUD2_VELOCITY 0.8
#define NL_CLOUD2_LAYER2_OFFSET 143.0
#define NL_CLOUD2_LAYER2_THICKNESS 2.5
#define NL_CLOUD2_LAYER2_RAIN_THICKNESS 3.0
#define NL_CLOUD2_LAYER2_STEPS 3
#define NL_CLOUD2_LAYER2_SCALE vec2(0.03, 0.03)
#define NL_CLOUD2_LAYER2_SHAPE vec2(0.5, 0.4)
#define NL_CLOUD2_LAYER2_DENSITY 25.0
#define NL_CLOUD2_LAYER2_VELOCITY 0.8
#define NL_CLOUD3_SCALE vec2(0.03, 0.03)
#define NL_CLOUD3_SPEED 0.006
#define NL_CLOUD3_SHADOW 0.82
#define NL_CLOUD3_SHADOW_OFFSET 0.3
#define NL_AURORA 1.2
#define NL_AURORA_VELOCITY 0.03
#define NL_AURORA_SCALE 0.04
#define NL_AURORA_WIDTH 0.18
#define NL_AURORA_COL1 vec3(0.1,1.0,0.0)
#define NL_AURORA_COL2 vec3(0.1,0.0,1.0)
#define NL_SHOOTING_STAR 1.0
#define NL_SHOOTING_STAR_PERIOD 6.0
#define NL_SHOOTING_STAR_DELAY 64.0
#define NL_GALAXY_VIBRANCE 0.7
#define NL_GALAXY_SPEED 0.03
#define NL_GALAXY_DAY_VISIBILITY 0.0
#define NL_SUN_SIZE 1.0
#define NL_MOON_SIZE 1.0
#define NL_SUN_PATH_YAW 15.0
#define NL_MOON_PATH_YAW 17.0
#define NL_SUN_PATH_TILT 31.0
#define NL_MOON_PATH_TILT -28.0
#define NL_SUN_TILT 45.0
#define NL_MOON_TILT 45.0
/* God rays disabled: no custom volumetric fog pass. */
#undef NL_GODRAY
/* Rain does not alter ground materials: no wet sheen or puddles. */
#undef NL_GROUND_RAIN_WETNESS
#undef NL_GROUND_RAIN_PUDDLES
#define NL_ENTITY_BRIGHTNESS 0.65
#define NL_ENTITY_EDGE_HIGHLIGHT 0.41
#define NL_WEATHER_SPECK 0.6
#define NL_WEATHER_RAIN_SLANT 4.0
#define NL_WEATHER_PARTICLE_SIZE 1.0
#define NL_LAVA_NOISE
#define NL_LAVA_NOISE_SPEED 0.2

/* Subpack compatibility */
#ifdef LITE
  #define NO_WAVE
  #undef NL_GLOW_SHIMMER
  #undef NL_LAVA_NOISE
  #undef NL_WEATHER_SPECK
  #undef NL_SHOOTING_STAR
  #undef NL_UNDERWATER_STREAKS
  #undef NL_RAIN_MIST_OPACITY
  #undef NL_CLOUDY_FOG
  #undef NL_ENTITY_EDGE_HIGHLIGHT
#endif

#ifdef NO_WAVE_NO_FOG
  #define NO_WAVE
  #define NO_FOG
#endif

#ifdef NO_FOG
  #undef NL_FOG
#endif

#ifdef NO_WAVE
  #undef NL_PLANTS_WAVE
  #undef NL_LANTERN_WAVE
  #undef NL_UNDERWATER_WAVE
  #undef NL_WATER_WAVE
  #undef NL_RAIN_MIST_OPACITY
#endif

#ifdef CHUNK_ANIM
  #define NL_CHUNK_LOAD_ANIM 100.0
#endif

#ifdef ROUNDED_CLOUDS
  #undef NL_CLOUD_TYPE
  #define NL_CLOUD_TYPE 2
  #undef NL_CLOUD_SHADOW
#endif

#ifdef BOX_CLOUDS
  #undef NL_CLOUD_TYPE
  #define NL_CLOUD_TYPE 0
  #undef NL_CLOUD_SHADOW
#endif

#ifdef REALISTIC_CLOUDS
  #undef NL_CLOUD_TYPE
  #define NL_CLOUD_TYPE 3
  #undef NL_CLOUD_SHADOW
#endif

#endif
