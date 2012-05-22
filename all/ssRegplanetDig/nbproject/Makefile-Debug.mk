#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/tools.o \
	${OBJECTDIR}/dig.o \
	${OBJECTDIR}/json.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/http.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/opt/lib -lm -lcares /opt/lib/libevent.a /storage/download/code/java/lib/libevent_pthreads.a -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/ssregplanetdig

dist/Debug/GNU-Linux-x86/ssregplanetdig: /opt/lib/libevent.a

dist/Debug/GNU-Linux-x86/ssregplanetdig: /storage/download/code/java/lib/libevent_pthreads.a

dist/Debug/GNU-Linux-x86/ssregplanetdig: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ssregplanetdig ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/tools.o: nbproject/Makefile-${CND_CONF}.mk tools.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -O -I/opt/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/tools.o tools.c

${OBJECTDIR}/dig.o: nbproject/Makefile-${CND_CONF}.mk dig.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -O -I/opt/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/dig.o dig.c

${OBJECTDIR}/json.o: nbproject/Makefile-${CND_CONF}.mk json.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -O -I/opt/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/json.o json.c

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -O -I/opt/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/http.o: nbproject/Makefile-${CND_CONF}.mk http.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -O -I/opt/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/http.o http.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/ssregplanetdig

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
