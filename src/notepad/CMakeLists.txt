SET(SRCS
app.cc
app.h
defs.h
text_buffer.cc
text_buffer.h
text_panel.cc
text_panel.h
text_action.cc
text_action.h
)

set(TARGET_NAME notepad)

add_executable(${TARGET_NAME} WIN32 ${SRCS})

target_link_libraries(${TARGET_NAME} ${wxWidgets_LIBRARIES})
