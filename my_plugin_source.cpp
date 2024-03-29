#include <stdio.h>
#include <string.h>
#include <bsd/string.h>
#include <math.h>

#include "public/render_engine_ui.h"
#include "public/telemetry_info.h"
#include "public/settings_info.h"

#include "../mavlink/common/mavlink.h"

PLUGIN_VAR RenderEngineUI* g_pEngine = NULL;
PLUGIN_VAR const char* g_szPluginName = "RubyContorol";
PLUGIN_VAR const char* g_szUID = "33eb61d-984f6c6-646fc-43da";

static struct {
    uint8_t pos;
    uint8_t event;
    int16_t encoder;
    uint16_t length;
    int16_t stop;
    uint32_t error;
} side[2];

#ifdef __cplusplus
extern "C" {
#endif

void init(void* pEngine)
{
  g_pEngine = (RenderEngineUI*)pEngine;
}

char* getName()
{
  return (char*)g_szPluginName;
}

char* getUID()
{
  return (char*)g_szUID;
}

int getVersion()
{
   return 1;
}

int requestTelemetryStreams()
{
    return 1;
}

float getDefaultWidth()
{
   if ( NULL == g_pEngine )
      return 0.24;
   u32 fontId = g_pEngine->getFontIdRegular();
   float text_width = g_pEngine->textWidth(fontId, "W")*24;
   return text_width;
}

float getDefaultHeight()
{
   if ( NULL == g_pEngine )
      return 0.24;

   u32 fontId = g_pEngine->getFontIdRegular();
   float text_height = g_pEngine->textHeight(fontId);
   return text_height*5;
}

static void set_l_pos(uint32_t value)
{
    side[0].pos = value;
}

static void set_r_pos(uint32_t value)
{
    side[1].pos = value;
}

static void set_l_enc(uint32_t value)
{
    side[0].encoder = value;
}

static void set_r_enc(uint32_t value)
{
    side[1].encoder = value;
}

static void set_l_err(uint32_t value)
{
    side[0].error = value;
}

static void set_r_err(uint32_t value)
{
    side[1].error = value;
}

static void set_l_len(uint32_t value)
{
    side[0].length = value;
}

static void set_r_len(uint32_t value)
{
    side[1].length = value;
}

static void set_l_stop(uint32_t value)
{
    side[0].stop = value;
}

static void set_r_stop(uint32_t value)
{
    side[1].stop = value;
}

static void set_l_evt(uint32_t value)
{
    side[0].event = value;
}

static void set_r_evt(uint32_t value)
{
    side[1].event = value;
}

static struct {
    const char *name;
    void (*callback)(uint32_t value);
} values_int[] = {
    { "l_pos", &set_l_pos},
    { "r_pos", &set_r_pos},
    { "l_enc", &set_l_enc},
    { "r_enc", &set_r_enc},
    { "l_err", &set_l_err},
    { "r_err", &set_r_err},
    { "l_len", &set_l_len},
    { "r_len", &set_r_len},
    { "l_stop", &set_l_stop},
    { "r_stop", &set_r_stop},
    { "l_evt", &set_l_evt},
    { "r_evt", &set_r_evt},
    { NULL, NULL },
};

static void processNamedValueInt(mavlink_message_t *msgMav)
{
    mavlink_named_value_int_t named_value_int;
    mavlink_msg_named_value_int_decode(msgMav, &named_value_int);
    for (uint32_t n=0; values_int[n].name!=NULL; ++n) {
        if (!strcmp(values_int[n].name, named_value_int.name)) {
            if (values_int[n].callback)
                values_int[n].callback(named_value_int.value);
            break;
        }
    }
}

void onTelemetryStreamData(u8* pData, int nDataLength, int nTelemetryType)
{
    mavlink_status_t statusMav;
    mavlink_message_t msgMav;

    for (int n=0;n<nDataLength; ++n) {
        if (mavlink_parse_char(0, (char)*(pData+n), &msgMav, &statusMav))
        {
            switch (msgMav.msgid)
            {
                case MAVLINK_MSG_ID_NAMED_VALUE_INT:
                    processNamedValueInt(&msgMav);
                    break;
            }
        }
    }
}

void render(vehicle_and_telemetry_info_t* pTelemetryInfo, plugin_settings_info_t2* pCurrentSettings, float xPos, float yPos, float fWidth, float fHeight)
{
  if ( NULL == g_pEngine)
  return;

  char szBuff[64];
  u32 fontId = g_pEngine->getFontIdRegular();
  float height_text = g_pEngine->textHeight(fontId);

  g_pEngine->setColors(g_pEngine->getColorOSDText());
  snprintf(szBuff, sizeof(szBuff), "P: %u %u", side[0].pos, side[1].pos);
  g_pEngine->drawText(xPos, yPos+height_text, fontId, szBuff);

  snprintf(szBuff, sizeof(szBuff), "L: %u %u", side[0].length, side[1].length);
  g_pEngine->drawText(xPos, yPos+height_text*2, fontId, szBuff);

  snprintf(szBuff, sizeof(szBuff), "A: %d %d", side[0].encoder, side[1].encoder);
  g_pEngine->drawText(xPos, yPos+height_text*3, fontId, szBuff);

  const char *event_l="UNK", *event_r="UNK";
  switch (side[0].event) {
    case 0: event_l = "IDLE"; break;
    case 1: event_l = "CALIBRATE"; break;
    case 2: event_l = "CALIBRATE"; break;
    case 3: event_l = "UP"; break;
    case 4: event_l = "UP_END"; break;
    case 5: event_l = "DOWN"; break;
    case 6: event_l = "DOWN_END"; break;
    case 7: 
    case 9: event_l = "LEFT"; break;
    case 8: 
    case 10: event_l = "LEFT_END"; break;
  }
  switch (side[1].event) {
    case 0: event_r = "IDLE"; break;
    case 1: event_r = "CALIBRATE"; break;
    case 2: event_r = "CALIBRATE"; break;
    case 3: event_r = "UP"; break;
    case 4: event_r = "UP_END"; break;
    case 5: event_r = "DOWN"; break;
    case 6: event_r = "DOWN_END"; break;
    case 7:
    case 9: event_r = "RIGHT"; break;
    case 8:
    case 10: event_r = "RIGHT_END"; break;
  }

  strcpy(szBuff, "E: ");
  g_pEngine->drawText(xPos, yPos+height_text*4, fontId, szBuff);
  float offset = g_pEngine->textWidth(fontId, szBuff);

  g_pEngine->setColors((side[0].error)?g_pEngine->getColorOSDWarning():g_pEngine->getColorOSDText());
  g_pEngine->drawText(xPos+offset, yPos+height_text*4, fontId, event_l);

  offset += g_pEngine->textWidth(fontId, event_l);
  offset += g_pEngine->textWidth(fontId, " ");

  g_pEngine->setColors((side[1].error)?g_pEngine->getColorOSDWarning():g_pEngine->getColorOSDText());
  g_pEngine->drawText(xPos+offset, yPos+height_text*4, fontId, event_r);
}

#ifdef __cplusplus
}
#endif