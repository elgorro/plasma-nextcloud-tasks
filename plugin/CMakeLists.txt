set(plugin_SRCS
    TaskModel.cpp
    CalDavService.cpp
    ICalParser.cpp
)

qt_add_qml_module(nextcloudtasksplugin
    SHARED
    URI com.github.elgorro.nextcloudtasks
    VERSION 1.0
    SOURCES ${plugin_SRCS}
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/com/github/elgorro/nextcloudtasks
)

target_link_libraries(nextcloudtasksplugin PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Qml
    Qt6::Quick
)

set(_qml_dest "${KDE_INSTALL_QMLDIR}/com/github/elgorro/nextcloudtasks")
install(TARGETS nextcloudtasksplugin DESTINATION ${_qml_dest})
install(FILES
    ${CMAKE_BINARY_DIR}/com/github/elgorro/nextcloudtasks/qmldir
    DESTINATION ${_qml_dest}
)
install(FILES
    ${CMAKE_BINARY_DIR}/com/github/elgorro/nextcloudtasks/nextcloudtasksplugin.qmltypes
    DESTINATION ${_qml_dest}
    OPTIONAL
)
