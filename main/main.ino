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
/* Private defines ---------------------------------------------------- */
#define ANALOG_MAX_SAMPLES      3

#define ANALOG_SENSOR_PIN       A0

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

/* Private function prototypes ---------------------------------------- */
void m_event_generate(float mean_value, float deviation);
void m_get_sample(void);

/* Function definitions ----------------------------------------------- */
void setup()
{
  Serial.begin(115200);
}

void loop()
{

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
