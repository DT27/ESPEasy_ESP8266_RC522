#include "src/Helpers/_CPlugin_Helper.h"
#ifdef USES_C010

// #######################################################################################################
// ########################### Controller Plugin 010: Generic UDP ########################################
// #######################################################################################################

# define CPLUGIN_010
# define CPLUGIN_ID_010         10
# define CPLUGIN_NAME_010       "Generic UDP"

bool CPlugin_010(CPlugin::Function function, struct EventStruct *event, String& string)
{
  bool success = false;

  switch (function)
  {
    case CPlugin::Function::CPLUGIN_PROTOCOL_ADD:
    {
      ProtocolStruct& proto = getProtocolStruct(event->idx); //      = CPLUGIN_ID_010;
      proto.usesMQTT     = false;
      proto.usesTemplate = true;
      proto.usesAccount  = false;
      proto.usesPassword = false;
      proto.defaultPort  = 514;
      proto.usesID       = false;
      break;
    }

    case CPlugin::Function::CPLUGIN_GET_DEVICENAME:
    {
      string = F(CPLUGIN_NAME_010);
      break;
    }

    case CPlugin::Function::CPLUGIN_PROTOCOL_TEMPLATE:
    {
      event->String1 = String();
      event->String2 = F("%sysname%_%tskname%_%valname%=%value%");
      break;
    }

    case CPlugin::Function::CPLUGIN_INIT:
    {
      success = init_c010_delay_queue(event->ControllerIndex);
      break;
    }

    case CPlugin::Function::CPLUGIN_EXIT:
    {
      exit_c010_delay_queue();
      break;
    }

    case CPlugin::Function::CPLUGIN_PROTOCOL_SEND:
    {
      if (C010_DelayHandler == nullptr) {
        break;
      }
      if (C010_DelayHandler->queueFull(event->ControllerIndex)) {
        break;
      }

      const uint8_t valueCount = getValueCountForTask(event->TaskIndex);

      if (valueCount == 0) {
        break;
      }

      //LoadTaskSettings(event->TaskIndex); // FIXME TD-er: This can probably be removed

      std::unique_ptr<C010_queue_element> element(new (std::nothrow) C010_queue_element(event, valueCount));


      {
        String pubname;
        {
          MakeControllerSettings(ControllerSettings); //-V522

          if (!AllocatedControllerSettings()) {
            break;
          }
          LoadControllerSettings(event->ControllerIndex, *ControllerSettings);
          pubname = ControllerSettings->Publish;
        }
        parseControllerVariables(pubname, event, false);
        const bool contains_valname = pubname.indexOf(F("%valname%")) != -1;

        for (uint8_t x = 0; x < valueCount; x++)
        {
          bool   isvalid;
          const String formattedValue = formatUserVar(event, x, isvalid);

          if (isvalid) {
            String txt;
            txt = pubname;
            if (contains_valname) {
              parseSingleControllerVariable(txt, event, x, false);
            }
            txt.replace(F("%value%"), formattedValue);
            move_special(element->txt[x], std::move(txt));
#ifndef BUILD_NO_DEBUG
            if (loglevelActiveFor(LOG_LEVEL_DEBUG_MORE))
              addLog(LOG_LEVEL_DEBUG_MORE, element->txt[x]);
#endif
          }
        }
      }

      success = C010_DelayHandler->addToQueue(std::move(element));
      Scheduler.scheduleNextDelayQueue(SchedulerIntervalTimer_e::TIMER_C010_DELAY_QUEUE, C010_DelayHandler->getNextScheduleTime());
      break;
    }

    case CPlugin::Function::CPLUGIN_FLUSH:
    {
      process_c010_delay_queue();
      delay(0);
      break;
    }

    default:
      break;
  }
  return success;
}

// ********************************************************************************
// Generic UDP message
// ********************************************************************************

// Uncrustify may change this into multi line, which will result in failed builds
// *INDENT-OFF*
bool do_process_c010_delay_queue(cpluginID_t cpluginID, const Queue_element_base& element_base, ControllerSettingsStruct& ControllerSettings) {
  const C010_queue_element& element = static_cast<const C010_queue_element&>(element_base);
// *INDENT-ON*
  while (element.txt[element.valuesSent].isEmpty()) {
    // A non valid value, which we are not going to send.
    // Increase sent counter until a valid value is found.
    if (element.checkDone(true)) {
      return true;
    }
  }
  WiFiUDP C010_portUDP;

  if (!beginWiFiUDP_randomPort(C010_portUDP)) { return false; }

  if (!try_connect_host(cpluginID, C010_portUDP, ControllerSettings)) {
    return false;
  }

  C010_portUDP.write(
    reinterpret_cast<const uint8_t *>(element.txt[element.valuesSent].c_str()),
    element.txt[element.valuesSent].length());
  bool reply = C010_portUDP.endPacket();

  C010_portUDP.stop();

  if (ControllerSettings.MustCheckReply) {
    return element.checkDone(reply);
  }
  return element.checkDone(true);
}

#endif // ifdef USES_C010
