/**
 * @file       main.c
 * @copyright  Copyright (C) 2020 HydraTech. All rights reserved.
 * @license    This project is released under the HydraTech License.
 * @version    1.0.0
 * @date       2021-12-19
 * @author     Thuan Le
 * @brief      Main file for Prototype IR project
 * @note       None
 * @example    None
 */

/* Includes ----------------------------------------------------------- */
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

/* Private defines ---------------------------------------------------- */
#define ANALOG_MAX_SAMPLES      3

#define ANALOG_SENSOR_PIN       A0

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLRT7nwmg1"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "7lr8FV9YRhXPl2rF4iSZHjX5cxKjJcXI"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static float m_sensor_value[ANALOG_MAX_SAMPLES];

static float m_mean_sensor_sample = 0.0;
static float m_standard_deviation = 0.0;
static float m_s2_sum             = 0.0;
static bool  m_event_4_trigger    = false;
static bool  m_event_4_drift_cnt  = 0;

char auth[] = BLYNK_AUTH_TOKEN;
// Your WiFi credentials.
char ssid[] = "Le Thinh";
char pass[] = "lethinh123";

BlynkTimer timer;

/* Private function prototypes ---------------------------------------- */
void m_event_generate(float mean_value, float deviation);
void m_get_sample(void);

/* Function definitions ----------------------------------------------- */
// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  Serial.println("Event 2");
}

void m_get_sample(void)
{
  // Calculate m_mean_sensor_sample
  for (int i = 0; i < ANALOG_MAX_SAMPLES; i++)
  {
    m_sensor_value[i] = analogRead(ANALOG_SENSOR_PIN);

    m_mean_sensor_sample += m_sensor_value[i];
    
    // Sum of squared input values
    m_s2_sum += m_sensor_value[i] * m_sensor_value[i]; 

    delay(50);
  }

  m_mean_sensor_sample /= ANALOG_MAX_SAMPLES;

  // Square-root of the mean of the squared deviations
  m_standard_deviation = sqrt((m_s2_sum / ANALOG_MAX_SAMPLES) - (m_mean_sensor_sample * m_mean_sensor_sample));

  m_event_generate(m_mean_sensor_sample, m_standard_deviation);

  Serial.print(m_mean_sensor_sample);

  Serial.print("\t");

  Serial.println(m_standard_deviation);
}

void m_event_generate(float mean_value, float deviation)
{
  if ((mean_value < 300) && (deviation > 80))
  {
    Serial.println("Event 2");
  }
  else if ((40 < deviation) && (deviation < 80) && (mean_value < 300))
  {
    Serial.println("Event 3");
  }
  else if ((40 < deviation) && (300 < mean_value) && (mean_value < 360))
  {
    Serial.println("Event 1");
  }
  else if ((40 > deviation) && (mean_value < 360) && (!m_event_4_trigger))
  {
    m_event_4_trigger = true;
  }
  else if (m_event_4_trigger)
  {
    if (mean_value < 360)
    {
      m_event_4_drift_cnt++;
    }
    else
    {
      m_event_4_trigger = false;
      m_event_4_drift_cnt = 0;
    }

    if (m_event_4_drift_cnt >= (2000 / 50))
    {
      Serial.println("Event 4");
      m_event_4_drift_cnt = 0;
      m_event_4_trigger = false;
    }
  }
}

/* End of file -------------------------------------------------------- */
