#
# OMNeT++/OMNEST Makefile for ECSNeT
#
# This file was generated with the command:
#  opp_makemake -f --deep -KINET_PROJ=/home/gayashan/projects/research/omnetpp/inet -DINET_IMPORT -I$$\(INET_PROJ\)/src -L$$\(INET_PROJ\)/out/$$\(CONFIGNAME\)/src -lINET
#

# Name of target to be created (-o option)
TARGET = ECSNeT$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(QTENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)
#USERIF_LIBS = $(QTENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = -I$(INET_PROJ)/src

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = $(LDFLAG_LIBPATH)$(INET_PROJ)/out/$(CONFIGNAME)/src  -lINET

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/builder/ECSBuilder.o \
    $O/builder/TaskBuilder.o \
    $O/common/SimulationController.o \
    $O/cpu/CPUCore.o \
    $O/cpu/scheduling/ICpuCoreScheduler.o \
    $O/cpu/scheduling/RoundRobinCpuCoreScheduler.o \
    $O/global/GlobalStreamingSupervisor.o \
    $O/model/operator/productivity/FixedProductivityDistribution.o \
    $O/model/operator/productivity/IOperatorProductivityDistribution.o \
    $O/model/operator/selectivity/FixedSelectivityDistribution.o \
    $O/model/operator/selectivity/IOperatorSelectivityDistribution.o \
    $O/model/source/eventrate/FixedSourceEventRateDistribution.o \
    $O/model/source/eventrate/ISourceEventRateDistribution.o \
    $O/model/source/eventrate/NormallyDistributedSourceEventRate.o \
    $O/model/source/eventrate/UniformSourceEventRateDistribution.o \
    $O/model/source/msgsize/FixedMessageSizeDistribution.o \
    $O/model/source/msgsize/IMessageSizeDistribution.o \
    $O/power/consumer/CPUPowerConsumer.o \
    $O/power/consumer/NetworkPowerConsumer.o \
    $O/power/storage/IdealNodeEnergyStorage.o \
    $O/stask/Acker.o \
    $O/stask/ISTask.o \
    $O/stask/StreamingOperator.o \
    $O/stask/StreamingSink.o \
    $O/stask/StreamingSource.o \
    $O/stask/StreamingSupervisor.o \
    $O/msg/Ack_m.o \
    $O/msg/StreamingMessage_m.o

# Message files
MSGFILES = \
    msg/Ack.msg \
    msg/StreamingMessage.msg

# SM files
SMFILES =

# Other makefile variables (-K)
INET_PROJ=/home/gayashan/projects/research/omnetpp/inet

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = $(OPPMAIN_LIB) $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)
ifneq ($(TOOLCHAIN_NAME),clangc2)
LIBS += -Wl,-rpath,$(abspath $(INET_PROJ)/out/$(CONFIGNAME)/src)
endif

COPTS = $(CFLAGS) $(IMPORT_DEFINES) -DINET_IMPORT $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET) $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE) | msgheaders smheaders
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

# include all dependencies
-include $(OBJS:%.o=%.d)
