#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ds18b20pic18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ds18b20pic18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/_ext/1270477542/crcutil.o ${OBJECTDIR}/_ext/1270477542/ds18b20.o ${OBJECTDIR}/_ext/1270477542/owlink.o ${OBJECTDIR}/_ext/1270477542/ownet.o
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/_ext/1270477542/crcutil.o.d ${OBJECTDIR}/_ext/1270477542/ds18b20.o.d ${OBJECTDIR}/_ext/1270477542/owlink.o.d ${OBJECTDIR}/_ext/1270477542/ownet.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/_ext/1270477542/crcutil.o ${OBJECTDIR}/_ext/1270477542/ds18b20.o ${OBJECTDIR}/_ext/1270477542/owlink.o ${OBJECTDIR}/_ext/1270477542/ownet.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/ds18b20pic18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F45K20
MP_PROCESSOR_OPTION_LD=18f45k20
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0x7dc0 -u_DEBUGCODELEN=0x240 -u_DEBUGDATASTART=0x5f4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -DPIC18 -I"C:/source/ds18b20/ds18b20pic18.X"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/main.o   main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/main.o 
	
${OBJECTDIR}/_ext/1270477542/crcutil.o: ../common/crcutil.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1270477542 
	@${RM} ${OBJECTDIR}/_ext/1270477542/crcutil.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -DPIC18 -I"C:/source/ds18b20/ds18b20pic18.X"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1270477542/crcutil.o   ../common/crcutil.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1270477542/crcutil.o 
	
${OBJECTDIR}/_ext/1270477542/ds18b20.o: ../common/ds18b20.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1270477542 
	@${RM} ${OBJECTDIR}/_ext/1270477542/ds18b20.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -DPIC18 -I"C:/source/ds18b20/ds18b20pic18.X"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1270477542/ds18b20.o   ../common/ds18b20.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1270477542/ds18b20.o 
	
${OBJECTDIR}/_ext/1270477542/owlink.o: ../common/owlink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1270477542 
	@${RM} ${OBJECTDIR}/_ext/1270477542/owlink.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -DPIC18 -I"C:/source/ds18b20/ds18b20pic18.X"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1270477542/owlink.o   ../common/owlink.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1270477542/owlink.o 
	
${OBJECTDIR}/_ext/1270477542/ownet.o: ../common/ownet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1270477542 
	@${RM} ${OBJECTDIR}/_ext/1270477542/ownet.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -DPIC18 -I"C:/source/ds18b20/ds18b20pic18.X"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1270477542/ownet.o   ../common/ownet.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1270477542/ownet.o 
	
else
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DPIC18 -I"C:/source/ds18b20/ds18b20pic18.X"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/main.o   main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/main.o 
	
${OBJECTDIR}/_ext/1270477542/crcutil.o: ../common/crcutil.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1270477542 
	@${RM} ${OBJECTDIR}/_ext/1270477542/crcutil.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DPIC18 -I"C:/source/ds18b20/ds18b20pic18.X"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1270477542/crcutil.o   ../common/crcutil.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1270477542/crcutil.o 
	
${OBJECTDIR}/_ext/1270477542/ds18b20.o: ../common/ds18b20.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1270477542 
	@${RM} ${OBJECTDIR}/_ext/1270477542/ds18b20.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DPIC18 -I"C:/source/ds18b20/ds18b20pic18.X"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1270477542/ds18b20.o   ../common/ds18b20.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1270477542/ds18b20.o 
	
${OBJECTDIR}/_ext/1270477542/owlink.o: ../common/owlink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1270477542 
	@${RM} ${OBJECTDIR}/_ext/1270477542/owlink.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DPIC18 -I"C:/source/ds18b20/ds18b20pic18.X"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1270477542/owlink.o   ../common/owlink.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1270477542/owlink.o 
	
${OBJECTDIR}/_ext/1270477542/ownet.o: ../common/ownet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1270477542 
	@${RM} ${OBJECTDIR}/_ext/1270477542/ownet.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DPIC18 -I"C:/source/ds18b20/ds18b20pic18.X"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1270477542/ownet.o   ../common/ownet.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1270477542/ownet.o 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/ds18b20pic18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG   -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/ds18b20pic18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/ds18b20pic18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w    -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/ds18b20pic18.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
