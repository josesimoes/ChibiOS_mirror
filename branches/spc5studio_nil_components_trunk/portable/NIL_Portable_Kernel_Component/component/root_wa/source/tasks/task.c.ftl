[#ftl]
[#--
  -- Iterating through all the defined tasks, a different file is generated
  -- for each one.
  --]
[#list conf.instance.tasks_configuration.tasks.task_settings as settings]
  [#assign name = settings.readable_name.value[0]?trim /]
  [#assign sname = settings.symbolic_name.value[0]?trim /]
  [@pp.dropOutputFile /]
  [@pp.changeOutputFile name="task_" + sname?lower_case + ".c" /]
/*
 * This file has been generated by SPC5Studio NIL RTOS Kernel Component.
 *
 * Do not edit this file manually, any change will be lost after next
 * code generation, edit the component configuration instead.
 */

#include "components.h"

#define TASK_NAME           ${sname}
#define TASK_WA_NAME        ${sname}_wa
#define TASK_STACK_SIZE     ${settings.stack_size.value[0]?trim}

${settings.task_code.value[0]}
[/#list]
