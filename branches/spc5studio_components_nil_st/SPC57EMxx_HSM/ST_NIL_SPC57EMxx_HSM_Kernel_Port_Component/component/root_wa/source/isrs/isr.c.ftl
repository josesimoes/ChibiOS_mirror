[#ftl]
[@pp.dropOutputFile /]
[#--
  -- Iterating through all the defined ISRs, a different file is generated
  -- for each one.
  --]
[#list conf.instance.isrs_configuration.isrs.isr_settings as settings]
  [#assign code = settings.isr_code.value[0] /]
  [#-- If the code block is empty then the file is not generated, only
       initializations.--]
  [#if code?trim != ""]
    [#assign identifier = settings.identifier.value[0]?trim /]
    [#assign vnum = settings.number.value[0]?trim /]
    [#assign name = settings.name_override.value[0]?trim /]
    [#if name == ""]
      [#assign name = "vector" + vnum /]
    [/#if]
    [@pp.changeOutputFile name="isr_" + identifier?lower_case + ".c" /]
/*
 * This file has been generated by the HSM RTOS Kernel Port Component.
 *
 * Do not edit this file manually, any change will be lost after next
 * code generation, edit the component configuration instead.
 */

#include "components.h"

/*
 * ISR parameters from the configuration.
 */
#define ISR_NAME            ${name}

${code}
  [/#if]
[/#list]