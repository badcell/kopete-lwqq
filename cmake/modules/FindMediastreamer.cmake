# cmake macro to test MEDIASTREAMER LIB

# Copyright (c) 2006, Alessandro Praduroux <pradu@pradu.it>
# Copyright (c) 2007, Urs Wolfer <uwolfer @ kde.org>
# Copyright (c) 2008, Matt Rogers <mattr@kde.org>
# Copyright (c) 2009, Tiago Salem Herrmann <tiagosh@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
INCLUDE( FindPackageHandleStandardArgs )
include( FindPkgConfig )

SET( MEDIASTREAMER_FOUND FALSE )

pkg_check_modules(MEDIASTREAMER mediastreamer>=2.3.0)

if (MEDIASTREAMER_INCLUDE_DIR AND MEDIASTREAMER_LIBRARIES)
  SET(MEDIASTREAMER_FOUND TRUE)
endif(MEDIASTREAMER_INCLUDE_DIR AND MEDIASTREAMER_LIBRARIES)

IF (MEDIASTREAMER_FOUND)
  IF (NOT MEDIASTREAMER_FIND_QUIETLY)
      MESSAGE(STATUS "Found Mediastreamer: ${MEDIASTREAMER_LIBRARIES}")
  ENDIF (NOT MEDIASTREAMER_FIND_QUIETLY)
ELSE (MEDIASTREAMER_FOUND)
  IF (MEDIASTREAMER_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could NOT find mediastreamer")
  ENDIF (MEDIASTREAMER_FIND_REQUIRED)
ENDIF (MEDIASTREAMER_FOUND)

MARK_AS_ADVANCED(MEDIASTREAMER_INCLUDE_DIR MEDIASTREAMER_LIBRARIES)
