ifeq "$(SCOE_BASE)" ""
force:: ;
% : force
	@ echo ERROR: need a definition for SCOE_BASE
	@ exit 1
endif

ifeq "$(PLATFORM)" ""
force:: ;
% : force
	@ echo ERROR: need a definition for PLATFORM
	@ exit 1
endif
include $(SCOE_BASE)/bin/commonDefinitions.mak
include $(SCOE_BASE)/platforms/$(PLATFORM)/platformDefinitions.mak

OUTPUTDIR = A-Course-Bank
OUTPUTDIR_DEBUG := $(OUTPUTDIR)_Debug

.PHONY: clean release debug

debug: OUTPUTDIR:=$(OUTPUTDIR_DEBUG)

release debug:
	-if not exist $(OUTPUTDIR)	$(MKDIR) $(OUTPUTDIR)
	@echo ***********************************************************
	$(CP) GraderExecutables\grader_main1 \
	     $(OUTPUTDIR)\grader_main
	$(MAKE) -C Applications/Student $@
	$(CP) Applications\Student\droneController_main \
	     $(OUTPUTDIR)\droneController_main
	@echo ***********************************************************
	$(ADECONFIGGEN) ConfigData/Module.xml \
	                /o=$(OUTPUTDIR)/PartitionConfig.cfg
                    
grader_main1:
	-if not exist $(OUTPUTDIR)	$(MKDIR) $(OUTPUTDIR)
	@echo ***********************************************************
	$(CP) GraderExecutables\$@ \
	     $(OUTPUTDIR)\grader_main
	$(MAKE) -C Applications/Student release
	$(CP) Applications\Student\droneController_main \
	     $(OUTPUTDIR)\droneController_main
	@echo ***********************************************************
	$(ADECONFIGGEN) ConfigData/Module.xml \
	                /o=$(OUTPUTDIR)/PartitionConfig.cfg

grader_main2:
	-if not exist $(OUTPUTDIR)	$(MKDIR) $(OUTPUTDIR)
	@echo ***********************************************************
	$(CP) GraderExecutables\$@ \
	     $(OUTPUTDIR)\grader_main
	$(MAKE) -C Applications/Student release
	$(CP) Applications\Student\droneController_main \
	     $(OUTPUTDIR)\droneController_main
	@echo ***********************************************************
	$(ADECONFIGGEN) ConfigData/Module.xml \
	                /o=$(OUTPUTDIR)/PartitionConfig.cfg

grader_main3:
	-if not exist $(OUTPUTDIR)	$(MKDIR) $(OUTPUTDIR)
	@echo ***********************************************************
	$(CP) GraderExecutables\$@ \
	     $(OUTPUTDIR)\grader_main
	$(MAKE) -C Applications/Student release
	$(CP) Applications\Student\droneController_main \
	     $(OUTPUTDIR)\droneController_main
	@echo ***********************************************************
	$(ADECONFIGGEN) ConfigData/Module.xml \
	                /o=$(OUTPUTDIR)/PartitionConfig.cfg

grader_main4:
	-if not exist $(OUTPUTDIR)	$(MKDIR) $(OUTPUTDIR)
	@echo ***********************************************************
	$(CP) GraderExecutables\$@ \
	     $(OUTPUTDIR)\grader_main
	$(MAKE) -C Applications/Student release
	$(CP) Applications\Student\droneController_main \
	     $(OUTPUTDIR)\droneController_main
	@echo ***********************************************************
	$(ADECONFIGGEN) ConfigData/Module.xml \
	                /o=$(OUTPUTDIR)/PartitionConfig.cfg

grader_main5:
	-if not exist $(OUTPUTDIR)	$(MKDIR) $(OUTPUTDIR)
	@echo ***********************************************************
	$(CP) GraderExecutables\$@ \
	     $(OUTPUTDIR)\grader_main
	$(MAKE) -C Applications/Student release
	$(CP) Applications\Student\droneController_main \
	     $(OUTPUTDIR)\droneController_main
	@echo ***********************************************************
	$(ADECONFIGGEN) ConfigData/Module.xml \
	                /o=$(OUTPUTDIR)/PartitionConfig.cfg

grader_main6:
	-if not exist $(OUTPUTDIR)	$(MKDIR) $(OUTPUTDIR)
	@echo ***********************************************************
	$(CP) GraderExecutables\$@ \
	     $(OUTPUTDIR)\grader_main
	$(MAKE) -C Applications/Student release
	$(CP) Applications\Student\droneController_main \
	     $(OUTPUTDIR)\droneController_main
	@echo ***********************************************************
	$(ADECONFIGGEN) ConfigData/Module.xml \
	                /o=$(OUTPUTDIR)/PartitionConfig.cfg

grader_main7:
	-if not exist $(OUTPUTDIR)	$(MKDIR) $(OUTPUTDIR)
	@echo ***********************************************************
	$(CP) GraderExecutables\$@ \
	     $(OUTPUTDIR)\grader_main
	$(MAKE) -C Applications/Student release
	$(CP) Applications\Student\droneController_main \
	     $(OUTPUTDIR)\droneController_main
	@echo ***********************************************************
	$(ADECONFIGGEN) ConfigData/Module.xml \
	                /o=$(OUTPUTDIR)/PartitionConfig.cfg

clean:
	-$(RMDIR) $(RMDIR_FLAGS) $(OUTPUTDIR)
	-$(RMDIR) $(RMDIR_FLAGS) $(OUTPUTDIR_DEBUG)
	$(MAKE) -C Applications/Student $@
