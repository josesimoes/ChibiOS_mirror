[#ftl]
[@pp.dropOutputFile /]
[@pp.changeOutputFile name="component.mak" /]
# List of library files exported by the component.
LIB_C_SRC       +=

LIB_CPP_SRC     +=

LIB_ASM_SRC     +=

LIB_INCLUDES    +=

APP_C_SRC       += ${global.local_component_path}/cfg/adc_lld_cfg.c

APP_CPP_SRC     +=

APP_ASM_SRC     +=

APP_INCLUDES    += ${global.local_component_path}/cfg