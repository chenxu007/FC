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

BUILD_SPEC = PPC32e500v2diab_RTP
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
PROJECT_TYPE = RTP
DEFINES = 
DO_STRIP = 0
EXPAND_DBG = 0


#BuildSpec specific Build Macros
VX_CPU_FAMILY = ppc
CPU = PPC32
TOOL_FAMILY = diab
TOOL = e500v2diab
TOOL_PATH = 
CC_ARCH_SPEC = -tPPCE500V2EH:rtp
LIBPATH = 
LIBS = -lstlstd
VSB_DIR = $(WIND_BASE)/target/lib

IDE_INCLUDES = -I$(WIND_BASE)/target/h/wrn/coreip -I$(WIND_BASE)/target/h -IC:/WindRiver/workspace/fc_driver/include 

IDE_LIBRARIES = C:/WindRiver/workspace/fc_driver/PPC85XXe500v2diab/fc_driver/Debug/fc_driver.a 

IDE_DEFINES = -D_VX_CPU=_VX_$(CPU) -D_VX_TOOL_FAMILY=$(TOOL_FAMILY) -D_VX_TOOL=$(TOOL) -DVXWORKS_OPS 



#BuildTool flags
ifeq ($(DEBUG_MODE),1)
DEBUGFLAGS_C-Compiler = -g
DEBUGFLAGS_C++-Compiler = -g
DEBUGFLAGS_Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -g
else
DEBUGFLAGS_C-Compiler = -XO 
DEBUGFLAGS_C++-Compiler = -XO 
DEBUGFLAGS_Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -XO 
endif


#Project Targets
PROJECT_TARGETS = fc_driver_test/$(MODE_DIR)/fc_driver_test.vxe


#Rules

# fc_driver_test
ifeq ($(DEBUG_MODE),1)
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Linker = 
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
else
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -XO 
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -XO 
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Linker = 
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
fc_driver_test/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -XO 
endif
fc_driver_test/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/h/wrn/coreip -I$(WIND_BASE)/target/h -IC:/WindRiver/workspace/fc_driver/include 
fc_driver_test/$(MODE_DIR)/% : IDE_LIBRARIES = C:/WindRiver/workspace/fc_driver/PPC85XXe500v2diab/fc_driver/Debug/fc_driver.a 
fc_driver_test/$(MODE_DIR)/% : IDE_DEFINES = -D_VX_CPU=_VX_$(CPU) -D_VX_TOOL_FAMILY=$(TOOL_FAMILY) -D_VX_TOOL=$(TOOL) -DVXWORKS_OPS 
fc_driver_test/$(MODE_DIR)/% : PROJECT_TYPE = RTP
fc_driver_test/$(MODE_DIR)/% : DEFINES = 
fc_driver_test/$(MODE_DIR)/% : DO_STRIP = 0
fc_driver_test/$(MODE_DIR)/% : EXPAND_DBG = 0
fc_driver_test/$(MODE_DIR)/% : VX_CPU_FAMILY = ppc
fc_driver_test/$(MODE_DIR)/% : CPU = PPC32
fc_driver_test/$(MODE_DIR)/% : TOOL_FAMILY = diab
fc_driver_test/$(MODE_DIR)/% : TOOL = e500v2diab
fc_driver_test/$(MODE_DIR)/% : TOOL_PATH = 
fc_driver_test/$(MODE_DIR)/% : CC_ARCH_SPEC = -tPPCE500V2EH:rtp
fc_driver_test/$(MODE_DIR)/% : LIBPATH = 
fc_driver_test/$(MODE_DIR)/% : LIBS = -lstlstd
fc_driver_test/$(MODE_DIR)/% : VSB_DIR = $(WIND_BASE)/target/lib
fc_driver_test/$(MODE_DIR)/% : OBJ_DIR := fc_driver_test/$(MODE_DIR)

fc_driver_test/$(MODE_DIR)/Objects/fc_driver_test/HwaFcNewTest.o : $(PRJ_ROOT_DIR)/HwaFcNewTest.cpp
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dplus $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -Xansi -WDVSB_DIR=$(WIND_BASE)/target/lib -Xsystem-headers-warn -Xstsw-slow -Xforce-declarations -ei4167,4092,4144,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


OBJECTS_fc_driver_test = fc_driver_test/$(MODE_DIR)/Objects/fc_driver_test/HwaFcNewTest.o

fc_driver_test/$(MODE_DIR)/fc_driver_test.vxe : $(OBJECTS_fc_driver_test)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dplus $(DEBUGFLAGS_Linker) $(CC_ARCH_SPEC) -Xansi -WDVSB_DIR=$(WIND_BASE)/target/lib -Xsystem-headers-warn -Xstsw-slow -Xforce-declarations -ei4167,4092,4144,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457   -Xmake-dependency=0xd  -o "$@" $(OBJECTS_fc_driver_test) $(ADDED_OBJECTS) $(IDE_LIBRARIES) $(LIBPATH) $(LIBS) -L$(VSB_DIR)/usr/lib/ppc/PPC32/e500v2common $(ADDED_LIBPATH) $(ADDED_LIBS)   && if [ "$(DO_STRIP)" = "1" ]; then mv -f "$@" "$@.unstripped" && stripppc -g -o "$@" "$@.unstripped";fi && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

fc_driver_test/$(MODE_DIR)/fc_driver_test_compile_file : $(FILE) ;

_clean :: fc_driver_test/$(MODE_DIR)/fc_driver_test_clean

fc_driver_test/$(MODE_DIR)/fc_driver_test_clean : 
	$(TRACE_FLAG)if [ -d "fc_driver_test" ]; then cd "fc_driver_test"; rm -rf $(MODE_DIR); fi

DEP_FILES := fc_driver_test/$(MODE_DIR)/Objects/fc_driver_test/HwaFcNewTest.d
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
