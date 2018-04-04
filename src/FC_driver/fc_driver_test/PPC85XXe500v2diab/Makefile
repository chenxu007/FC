# Wind River Workbench generated Makefile.
# Do not edit!!!
#
# The file ".wrmakefile" is the template used by the Wind River Workbench to
# generate the makefiles of this project. Add user-specific build targets and
# make rules only(!) in this project's ".wrmakefile" file. These will then be
# automatically dumped into the makefiles.

WIND_HOME := $(subst \,/,$(WIND_HOME))
WIND_BASE := $(subst \,/,$(WIND_BASE))
WIND_USR := $(subst \,/,$(WIND_USR))
WRVX_COMPBASE := $(subst \,/,$(WRVX_COMPBASE))

all : pre_build main_all post_build

_clean ::
	@echo "make: removing targets and objects of `pwd`"

TRACE=0
TRACEON=$(TRACE:0=@)
TRACE_FLAG=$(TRACEON:1=)

MAKEFILE := Makefile

FLEXIBLE_BUILD := 1

BUILD_SPEC = PPC85XXe500v2diab
DEBUG_MODE = 1
ifeq ($(DEBUG_MODE),1)
MODE_DIR := Debug
else
MODE_DIR := NonDebug
endif
OBJ_DIR := .
WS_ROOT_DIR := C:/WindRiver/workspace
PRJ_ROOT_DIR := $(WS_ROOT_DIR)/fc_driver_test



#Global Build Macros
PROJECT_TYPE = DKM
DEFINES = 
EXPAND_DBG = 0


#BuildSpec specific Build Macros
VX_CPU_FAMILY = ppc
CPU = PPC85XX
TOOL_FAMILY = diab
TOOL = e500v2diab
TOOL_PATH = 
CC_ARCH_SPEC = -tPPCE500V2FH:vxworks69
VSB_DIR = $(WIND_BASE)/target/lib
VSB_CONFIG_FILE = $(VSB_DIR)/h/config/vsbConfig.h
LIBPATH = 
LIBS = 

IDE_INCLUDES = -I$(WIND_BASE)/target/h/wrn/coreip -IC:/WindRiver/vxworks-6.9/target/h/hwif/util -IC:/WindRiver/vxworks-6.9/target/h/hwif/vxbus -I$(WIND_BASE)/target/h -IC:/WindRiver/workspace/fc_driver/include 

IDE_LIBRARIES = C:/WindRiver/workspace/fc_driver/PPC85XXe500v2diab/fc_driver/Debug/fc_driver.a 

IDE_DEFINES = -DCPU=_VX_$(CPU) -DVXWORKS_TEST -DVXWORKS_OPS -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL -D_VSB_CONFIG_FILE=\"$(VSB_DIR)/h/config/vsbConfig.h\"   



#BuildTool flags
ifeq ($(DEBUG_MODE),1)
DEBUGFLAGS_C-Compiler = -g
DEBUGFLAGS_C++-Compiler = -g
DEBUGFLAGS_Linker = -g
DEBUGFLAGS_Partial-Image-Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -g
else
DEBUGFLAGS_C-Compiler = -XO -Xsize-opt
DEBUGFLAGS_C++-Compiler = -XO -Xsize-opt
DEBUGFLAGS_Linker = -XO -Xsize-opt
DEBUGFLAGS_Partial-Image-Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -XO -Xsize-opt
endif


#Project Targets
PROJECT_TARGETS = fc_driver_test/$(MODE_DIR)/fc_driver_test.out \
	 fc_driver_test_partialImage/$(MODE_DIR)/fc_driver_test_partialImage.o


#Rules

# fc_driver_test
ifeq ($(DEBUG_MODE),1)
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -g
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
else
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -XO -Xsize-opt
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -XO -Xsize-opt
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -XO -Xsize-opt
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -XO -Xsize-opt
endif
fc_driver_test/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/h/wrn/coreip -IC:/WindRiver/vxworks-6.9/target/h/hwif/util -IC:/WindRiver/vxworks-6.9/target/h/hwif/vxbus -I$(WIND_BASE)/target/h -IC:/WindRiver/workspace/fc_driver/include 
fc_driver_test/$(MODE_DIR)/% : IDE_LIBRARIES = C:/WindRiver/workspace/fc_driver/PPC85XXe500v2diab/fc_driver/Debug/fc_driver.a 
fc_driver_test/$(MODE_DIR)/% : IDE_DEFINES = -DCPU=_VX_$(CPU) -DVXWORKS_TEST -DVXWORKS_OPS -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL -D_VSB_CONFIG_FILE=\"$(VSB_DIR)/h/config/vsbConfig.h\"   
fc_driver_test/$(MODE_DIR)/% : PROJECT_TYPE = DKM
fc_driver_test/$(MODE_DIR)/% : DEFINES = 
fc_driver_test/$(MODE_DIR)/% : EXPAND_DBG = 0
fc_driver_test/$(MODE_DIR)/% : VX_CPU_FAMILY = ppc
fc_driver_test/$(MODE_DIR)/% : CPU = PPC85XX
fc_driver_test/$(MODE_DIR)/% : TOOL_FAMILY = diab
fc_driver_test/$(MODE_DIR)/% : TOOL = e500v2diab
fc_driver_test/$(MODE_DIR)/% : TOOL_PATH = 
fc_driver_test/$(MODE_DIR)/% : CC_ARCH_SPEC = -tPPCE500V2FH:vxworks69
fc_driver_test/$(MODE_DIR)/% : VSB_DIR = $(WIND_BASE)/target/lib
fc_driver_test/$(MODE_DIR)/% : VSB_CONFIG_FILE = $(VSB_DIR)/h/config/vsbConfig.h
fc_driver_test/$(MODE_DIR)/% : LIBPATH = 
fc_driver_test/$(MODE_DIR)/% : LIBS = 
fc_driver_test/$(MODE_DIR)/% : OBJ_DIR := fc_driver_test/$(MODE_DIR)

OBJECTS_fc_driver_test = fc_driver_test_partialImage/$(MODE_DIR)/fc_driver_test_partialImage.o

fc_driver_test/$(MODE_DIR)/fc_driver_test.out : $(OBJECTS_fc_driver_test)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@";rm -f "$@";ddump -Ng $(OBJECTS_fc_driver_test) | tclsh $(WIND_BASE)/host/resource/hutils/tcl/munch.tcl -c ppc -tags $(VSB_DIR)/tags/ppc/PPC85XX/e500v2common/dkm.tags > $(OBJ_DIR)/ctdt.c; $(TOOL_PATH)dcc $(DEBUGFLAGS_Linker) $(CC_ARCH_SPEC) -Xdollar-in-ident -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828 -ei1522,4092,4111,4144,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations  $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES)  $(IDE_DEFINES) $(DEFINES) -o $(OBJ_DIR)/ctdt.o -c $(OBJ_DIR)/ctdt.c; $(TOOL_PATH)dld -tPPCE500V2FH:vxworks69 -X -r5 -r4 -o "$@" $(OBJ_DIR)/ctdt.o $(OBJECTS_fc_driver_test) $(IDE_LIBRARIES) $(LIBPATH) $(LIBS) $(ADDED_LIBPATH) $(ADDED_LIBS) && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

fc_driver_test/$(MODE_DIR)/fc_driver_test_compile_file : $(FILE) ;

_clean :: fc_driver_test/$(MODE_DIR)/fc_driver_test_clean

fc_driver_test/$(MODE_DIR)/fc_driver_test_clean : 
	$(TRACE_FLAG)if [ -d "fc_driver_test" ]; then cd "fc_driver_test"; rm -rf $(MODE_DIR); fi


# fc_driver_test_partialImage
ifeq ($(DEBUG_MODE),1)
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -g
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
else
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -XO -Xsize-opt
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -XO -Xsize-opt
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -XO -Xsize-opt
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
fc_driver_test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -XO -Xsize-opt
endif
fc_driver_test_partialImage/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/h/wrn/coreip -IC:/WindRiver/vxworks-6.9/target/h/hwif/util -IC:/WindRiver/vxworks-6.9/target/h/hwif/vxbus -I$(WIND_BASE)/target/h -IC:/WindRiver/workspace/fc_driver/include 
fc_driver_test_partialImage/$(MODE_DIR)/% : IDE_LIBRARIES = C:/WindRiver/workspace/fc_driver/PPC85XXe500v2diab/fc_driver/Debug/fc_driver.a 
fc_driver_test_partialImage/$(MODE_DIR)/% : IDE_DEFINES = -DCPU=_VX_$(CPU) -DVXWORKS_TEST -DVXWORKS_OPS -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL -D_VSB_CONFIG_FILE=\"$(VSB_DIR)/h/config/vsbConfig.h\"   
fc_driver_test_partialImage/$(MODE_DIR)/% : PROJECT_TYPE = DKM
fc_driver_test_partialImage/$(MODE_DIR)/% : DEFINES = 
fc_driver_test_partialImage/$(MODE_DIR)/% : EXPAND_DBG = 0
fc_driver_test_partialImage/$(MODE_DIR)/% : VX_CPU_FAMILY = ppc
fc_driver_test_partialImage/$(MODE_DIR)/% : CPU = PPC85XX
fc_driver_test_partialImage/$(MODE_DIR)/% : TOOL_FAMILY = diab
fc_driver_test_partialImage/$(MODE_DIR)/% : TOOL = e500v2diab
fc_driver_test_partialImage/$(MODE_DIR)/% : TOOL_PATH = 
fc_driver_test_partialImage/$(MODE_DIR)/% : CC_ARCH_SPEC = -tPPCE500V2FH:vxworks69
fc_driver_test_partialImage/$(MODE_DIR)/% : VSB_DIR = $(WIND_BASE)/target/lib
fc_driver_test_partialImage/$(MODE_DIR)/% : VSB_CONFIG_FILE = $(VSB_DIR)/h/config/vsbConfig.h
fc_driver_test_partialImage/$(MODE_DIR)/% : LIBPATH = 
fc_driver_test_partialImage/$(MODE_DIR)/% : LIBS = 
fc_driver_test_partialImage/$(MODE_DIR)/% : OBJ_DIR := fc_driver_test_partialImage/$(MODE_DIR)

fc_driver_test_partialImage/$(MODE_DIR)/Objects/fc_driver_test/HwaFcSelfTest.o : $(PRJ_ROOT_DIR)/HwaFcSelfTest.cpp
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only -Xsystem-headers-warn -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828 -ei1522,4092,4111,4144,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


OBJECTS_fc_driver_test_partialImage = fc_driver_test_partialImage/$(MODE_DIR)/Objects/fc_driver_test/HwaFcSelfTest.o

fc_driver_test_partialImage/$(MODE_DIR)/fc_driver_test_partialImage.o : $(OBJECTS_fc_driver_test_partialImage)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dld -tPPCE500V2FH:vxworks69 -X -r5  -o "$@" $(OBJECTS_fc_driver_test_partialImage) $(ADDED_OBJECTS) $(IDE_LIBRARIES) $(LIBPATH) $(LIBS) $(ADDED_LIBPATH) $(ADDED_LIBS) && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

fc_driver_test_partialImage/$(MODE_DIR)/fc_driver_test_partialImage_compile_file : $(FILE) ;

_clean :: fc_driver_test_partialImage/$(MODE_DIR)/fc_driver_test_partialImage_clean

fc_driver_test_partialImage/$(MODE_DIR)/fc_driver_test_partialImage_clean : 
	$(TRACE_FLAG)if [ -d "fc_driver_test_partialImage" ]; then cd "fc_driver_test_partialImage"; rm -rf $(MODE_DIR); fi

DEP_FILES := fc_driver_test_partialImage/$(MODE_DIR)/Objects/fc_driver_test/HwaFcSelfTest.d
-include $(DEP_FILES)


WIND_SCOPETOOLS_BASE := $(subst \,/,$(WIND_SCOPETOOLS_BASE))

clean_scopetools :
	$(TRACE_FLAG)rm -rf $(PRJ_ROOT_DIR)/.coveragescope/db

CLEAN_STEP := clean_scopetools


-include $(PRJ_ROOT_DIR)/*.makefile

-include *.makefile

JOBS?=1
ifeq ($(JOBS),1)
main_all : external_build  $(PROJECT_TARGETS)
	@echo "make: built targets of `pwd`"
else
main_all : external_build 
	@$(MAKE) -f $(MAKEFILE) --jobs $(JOBS) $(MFLAGS) $(PROJECT_TARGETS);\
	echo "make: built targets of `pwd`"
endif 

# entry point for extending the build
external_build ::
	@echo ""

# main entry point for pre processing prior to the build
pre_build :: $(PRE_BUILD_STEP) generate_sources
	@echo ""

# entry point for generating sources prior to the build
generate_sources ::
	@echo ""

# main entry point for post processing after the build
post_build :: $(POST_BUILD_STEP) deploy_output
	@echo ""

# entry point for deploying output after the build
deploy_output ::
	@echo ""

clean :: external_clean $(CLEAN_STEP) _clean

# entry point for extending the build clean
external_clean ::
	@echo ""
