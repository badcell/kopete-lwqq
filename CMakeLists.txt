project(kopete-lwqq)

set(CMAKE_MODULE_PATH 
    ${CMAKE_SOURCE_DIR}/cmake/modules
    ${CMAKE_SOURCE_DIR}/liblwqq/cmake
    ${CMAKE_MODULE_PATH}
)

add_subdirectory( liblwqq )

find_package(KDE4 REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckPrototypeExists)
include(CheckTypeSize)
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=14210 )

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBJS REQUIRED mozjs185)


set(CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS})
set(CMAKE_CXX_FLAGS "-fpermissive")

if (WIN32)
	set(DISABLE_VIDEOSUPPORT ON)
else (WIN32)
	option(DISABLE_VIDEOSUPPORT "Build Kopete without video-support" ON)
endif (WIN32)
if (DISABLE_VIDEOSUPPORT)
	add_definitions(-DVIDEOSUPPORT_DISABLED)
endif (DISABLE_VIDEOSUPPORT)

if(WIN32)
   set(CMAKE_REQUIRED_LIBRARIES ${KDEWIN32_LIBRARIES})
   set(CMAKE_REQUIRED_INCLUDES  ${KDEWIN32_INCLUDES})
endif(WIN32)
add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH} )


find_package(Kopete REQUIRED)


#KDE4_NO_ENABLE_FINAL(kopete)


# Generate config-kopete.h
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kopete.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kopete.h )

# For config-kopete.h
#include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

link_directories( 
${KOPETE_BINARY_DIR}/lib
)



add_subdirectory( icons ) 


include_directories( 
#kopete
${KOPETE_INCLUDE_DIRS} 
${KOPETE_INCLUDE_DIRS}/ui/
${CMAKE_CURRENT_SOURCE_DIR}/ui/ 
${CMAKE_CURRENT_SOURCE_DIR}/liblwqq/lib/
)



########### next target ###############

set(kopete_webqq_PART_SRCS 
   webqqprotocol.cpp 
   qq_types.cpp
   webqqcontact.cpp 
   webqqaccount.cpp 
   webqqaddcontactpage.cpp 
   webqqeditaccountwidget.cpp 
   webqqbridgecallback.cpp
   webqqloginverifywidget.cpp
   webqquserinfoform.cpp
   webqqchatsession.cpp
   webqqdiscuchatsession.cpp
   webqqgroupchatsession.cpp
   webqqshowgetinfo.cpp
   js.cpp
   translate.cpp
   trex.cpp
   ui/webqqwebcamdialog.cpp 
)



kde4_add_ui_files(kopete_webqq_PART_SRCS
webqqaddui.ui
)

kde4_add_ui_files(kopete_webqq_PART_SRCS
webqqaccountpreferences.ui
)

kde4_add_ui_files(kopete_webqq_PART_SRCS
webqquserinfoform.ui
)

kde4_add_plugin(kopete_webqq ${kopete_webqq_PART_SRCS})


#target_link_libraries(kopete_webqq ${KDE4_KIO_LIBS} kopete  pthread lwqq sqlite3 z curl ${LIBJS_LIBRARIES} ${EV_LIBRARIES})
target_link_libraries(kopete_webqq ${KDE4_KIO_LIBS} kopete  lwqq ${LIBJS_LIBRARIES})
if(NOT DISABLE_VIDEOSUPPORT)
  target_link_libraries(kopete_webqq kopete_videodevice)
endif(NOT DISABLE_VIDEOSUPPORT)

#Urgly!!!
set(PLUGIN_INSTALL_DIR "/usr/lib/kde4/")
set(SERVICES_INSTALL_DIR "/usr/share/kde4/services/")
 
install(TARGETS kopete_webqq  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES kopete_webqq.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES webqqdiscuui.rc webqqchatui.rc webqqgroupui.rc  DESTINATION ${DATA_INSTALL_DIR}/kopete_webqq)
